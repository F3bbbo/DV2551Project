#pragma once
#include <unordered_map>
#include "IA.h"
#include "VertexBuffer.h"
#include "Technique.h"
#include "Transform.h"
#include "ConstantBuffer.h"
#include "Texture2D.h"
#include <memory>

class Mesh
{
public:
	Mesh();
	~Mesh();

	// technique has: Material, RenderState, Attachments (color, depth, etc)
	std::shared_ptr<Technique> technique;
	// translation buffers
	std::shared_ptr<ConstantBuffer> txBuffer;
	// local copy of the translation
	std::shared_ptr<Transform> transform;

	struct VertexBufferBind {
		size_t sizeElement, numElements, offset;
		std::shared_ptr<VertexBuffer> buffer;
	};
	
	void addTexture(std::shared_ptr<Texture2D> texture, unsigned int slot);

	// array of buffers with locations (binding points in shaders)
	void addIAVertexBufferBinding(
		std::shared_ptr<VertexBuffer> buffer, 
		size_t offset, 
		size_t numElements, 
		size_t sizeElement, 
		unsigned int inputStream);

	virtual void bindIAVertexBuffer(unsigned int location);
	std::unordered_map<unsigned int, VertexBufferBind> geometryBuffers;
	std::unordered_map<unsigned int, std::shared_ptr<Texture2D>> textures;
};
