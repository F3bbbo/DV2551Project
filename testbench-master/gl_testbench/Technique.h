#pragma once
#include <vector>
#include "Material.h"
#include "RenderState.h"
#include <memory>

class Renderer;

class Technique
{
public:
	Technique(std::shared_ptr<Material> m, std::shared_ptr<RenderState> r) : material(m), renderState(r) {};
	virtual ~Technique();
	Material* getMaterial() { return material.get(); };
	RenderState* getRenderState() { return renderState.get(); };
	virtual void enable(Renderer* renderer);
protected:
	std::shared_ptr<Material> material;
	std::shared_ptr<RenderState> renderState;
};

