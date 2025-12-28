#pragma once
#include "../../Common/Renderer/IRenderPass.h"

class BasicRenderPass : public IRenderPass
{
public:
	void Execute(
		std::shared_ptr<ID3D11DeviceContext> context,
		const Scene& scene
		// const Camera& camera // ?
	) override;
};