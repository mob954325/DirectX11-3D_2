#include "Shared.hlsli"

static const float PI = 3.141592;
static const float Epsilon = 0.00001;

// D(h) (Normal Distribution Function) : Trowbridge-Reitz GGX
float NDFGGXTR(float3 normal, float3 halfVec, float roughness)
{
    float cosLh = dot(normal, halfVec);
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;
    
    float demon = PI * pow((cosLh * cosLh) * (alphaSq - 1) + 1, 2);
    
    return alphaSq / demon;
}

// F(v, h) Fresnel equation : Fresnel-Schlick approximation 
float3 FresnelSchlick(float3 F0, float cosTheta) // F0 == relfection factor
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// G(l,v,h) 
float GSchlickGGX(float3 norm, float3 viewVec, float k) // K : ���� ���� ����
{
    float NdotV = dot(norm, viewVec);    
    float denom = NdotV * (1.0 - k) + k;
    return NdotV / denom;
}

// G(n, v, l, k) : Smith's Method
float GSmithMethod(float3 norm, float3 viewVec, float3 lightVec, float roughness)
{
    float kDir = (roughness + 1) * (roughness + 1);     // for directional lighting
    //float kIBL = roughness* roughness / 2.0;          // IBL lighting
    kDir /= 8.0;
    
    return GSchlickGGX(norm, viewVec, kDir) * GSchlickGGX(norm, lightVec, kDir);
}

float4 main(PS_INPUT input) : SV_TARGET
{
    // �׸���ó�� �κ� =====================================================================
    float finalShadow = 1.0f;
    // ���� NDC ��ǥ�迡���� ��ǥ�� ��������� �����Ƿ� ����Ѵ�.
    float currentShadowDepth = input.PositionShadow.z / input.PositionShadow.w;
    
    // ���� NDC ��ǥ�迡���� x(-1 ~ 1), y(-1 ~ 1)
    float2 uv = input.PositionShadow.xy / input.PositionShadow.w;
    
    // NDC��ǥ�迡�� Texture ��ǥ��� ��ȯ
    uv.y = -uv.y;
    uv = uv * 0.5 + 0.5;
    
    if (uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0)
    {
        float sampleShadowDepth = txShadow.Sample(samLinear, uv).r;
        
        // currentShadowDepth�� �� ũ�� �� �ʿ� �����Ƿ� ������ ����
        if (currentShadowDepth > sampleShadowDepth + 0.001)
        {
            finalShadow = 0.0f;
        }
    }
    
    // ����ó�� �κ� =====================================================================
    // base(diffuse) texture Sampling 
    float4 albedo = txDiffuse.Sample(samLinear, input.Tex);
    if (!hasDiffuse)
    {
        albedo = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    
    if (albedo.a < 0.5f) // alpha cliping
        discard;
    
    albedo.rgb = pow(albedo.rgb, 2.2); // gamma -> linear
    
    // specularSample
    float specularIntensity = txSpec.Sample(samLinear, input.Tex).r;
    if (!hasSpecular)
    {
        specularIntensity = 1.0f;
    }
    
    // EmissionSample
    float4 textureEmission = txEmission.Sample(samLinear, input.Tex);
    if (!hasEmissive)
    {
        textureEmission = float4(0.0f, 0.0f, 0.0f, 0.0f);
    }
    
    // normalSample
    float3x3 TBN = float3x3(input.Tangent, input.Bitangent, input.Norm);
    float3 normalMapSample = txNormal.Sample(samLinear, input.Tex).rgb;
    // normal map�� ���� ��츦 ���
    if (!hasNormal)
    {
        normalMapSample = float3(0.5f, 0.5f, 1.0f); // flat normal (no perturbation)
    }
    float3 normalTexture = normalize(DecodeNormal(normalMapSample)); //  Convert normal map color (RGB [0,1]) to normal vector in range [-1,1] 
    float3 finalNorm = normalize(mul(normalTexture, TBN));  
    
    // MetalnessSample
    float metalnessSample = txMetalness.Sample(samLinear, input.Tex).r; // grayScale
    if(!hasMetalness)
    {
        metalnessSample = 1;
    }
    
    float finalMetalness = metalnessSample * Metalness;
    
    // RoughnessSample
    float roughnessSample = txRoughness.Sample(samLinear, input.Tex).r; // grayScale
    if(!hasRoughness)
    {
        roughnessSample = hasShininess ? 1 - roughnessSample : 1;
    }
    
    float finalRoughness = roughnessSample * Roughness;
    
    // ambient occlusion
    float ambientOcclusionFactor = txAmbientOcclusion.Sample(samLinear, input.Tex).r;
    if (!hasAmbientOcclusion)
    {
        ambientOcclusionFactor = ambientOcclusion;
    }

    float3 directLighting = 0.0f;
    
    // Cook-Torrance Specular BRDF
    float3 norm = finalNorm;
    
    float3 Lo = normalize(CameraPos - (float3) input.World); // ���� ������ ���� ���� : ���� ��ġ -> eye ( view Vector )
    float3 Li = -(float3)LightDirection;    // �� ����
    float3 Lh = normalize(Li + Lo); // half-vector between Li and Lo        
    
    float NdotL = max(0.0, dot(norm, Li));  // dot(Normal, Light Direction)
    float NdotO = max(0.0, dot(norm, Lo));  // dot(Normal, View)
    
    // �⺻ �ݻ���(F0) = lerp(��ݼ� ��� �ݻ�, baseColor(�ؽ�ó), matalness)
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), (float3) albedo, finalMetalness);
    
    {   
        float D = NDFGGXTR(norm, Lh, max(0.001, finalRoughness));
        float F = FresnelSchlick(F0, max(0, dot(Lh, Lo)));
        float G = GSmithMethod(norm, Lo, Li, finalRoughness);
   
        // ǥ�� ���
        float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), finalMetalness);
    
        // Lambert diffuse BRDF  
        float3 diffuseBRDF = kd * (float3) albedo / PI;
    
        // specular BRDF
        float3 specularBRDF = (D * F * G) / max(Epsilon, 4.0 * NdotL * NdotO);
        
        directLighting = (diffuseBRDF + specularBRDF * specularIntensity) * NdotL * finalShadow + (float3)textureEmission;
    }

    
    float3 inDirectLighting = 0.0f;
    // IBL
    {
        float3 irradiance = txIBLIrradiance.Sample(samLinear, norm).rgb;
    
        // IBL diffuse    
        float3 F = FresnelSchlick(F0, NdotO);
        float3 kd = lerp(1.0 - F, 0.0, finalMetalness);
        
        float3 diffuseIBL = kd * (float3)albedo * irradiance / PI; // irradiance map�� 1/pi�� ���ԵǾ������� ���� ������ 1/pi �߰��ϱ�
    
        // IBL specular        
        uint specularTexureLevels, width, height;        
        txIBLSepcualar.GetDimensions(0, width, height, specularTexureLevels);
        
        float3 PrefilteredColor = txIBLSepcualar.SampleLevel(samLinear, reflect(-Lo, norm), finalRoughness * specularTexureLevels).rgb;
        
        // dot(Normal,View) , roughness�� �ؼ���ǥ�� �̸����� F*G , G ��հ��� ���ø��Ѵ�  
        float2 specularBRDF = txIBLLookUpTable.Sample(samLinear, float2(NdotO, finalRoughness)).rg;
        
        // ���䷱�� Spceular BRDF �ٻ��
        float3 specularIBL = PrefilteredColor * (F0 * specularBRDF.x + specularBRDF.y); // x : normal dot view, y : roughtness
        
        inDirectLighting = (diffuseIBL + specularIBL) * ambientOcclusionFactor;
    }
    
    return float4(pow(float3(directLighting + inDirectLighting), 1.0 / 2.2), 1.0); // linear -> gamma 
}