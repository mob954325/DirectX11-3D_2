#pragma once
#include "../pch.h"

struct MaterialData
{
	Vector4 ambient = { 1.0f, 1.0f, 1.0f, 1.0f };
    Vector4 diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
    Vector4 specular = { 1.0f, 1.0f, 1.0f, 1.0f };

    BOOL hasDiffuse = false;
    BOOL hasEmissive = false;
    BOOL hasNormal = false;
    BOOL hasSpecular = false;

	BOOL hasMatalness = false;
	BOOL hasRoughness = false;
	BOOL hasShininess = false;
	INT pad;

    float Matalness;
    float Roughness;
    Vector2 pad2;
};