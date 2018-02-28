#include "Mesh.h"

Mesh::Mesh()
{
};

/*
	buffer: is a VertexBuffer*
	offset: offset of the first byte in the buffer used when binding
	size: how many elements (how many points, normals, UVs) should be read from this buffer
	inputStream: location of the binding in the VertexShader
*/
void Mesh::addIAVertexBufferBinding(
	std::shared_ptr<VertexBuffer> buffer,
	size_t offset, 
	size_t numElements, 
	size_t sizeElement, 
	unsigned int inputStream)
{
	// inputStream is unique (has to be!) for this Mesh
	buffer->incRef();
	geometryBuffers[inputStream] = { sizeElement, numElements, offset, buffer };
};

void Mesh::bindIAVertexBuffer(unsigned int location)
{
	// no checking if the key is valid...TODO
	const VertexBufferBind& vb = geometryBuffers[location];
	vb.buffer->bind(vb.offset,vb.numElements*vb.sizeElement,location);
}

// note, slot is a value set in the shader as well (registry, or binding)
void Mesh::addTexture(std::shared_ptr<Texture2D> texture, unsigned int slot)
{
	// would override the slot if there is another pointer here.
	textures[slot] = texture;
}

Mesh::~Mesh()
{
	for (auto g : geometryBuffers) {
		g.second.buffer->decRef();
	}
}

void Mesh::setTranslation(Vector3 translation)
{
	translationMatrix = Matrix::CreateTranslation(translation);
}

void Mesh::setRotation(Vector3 rotation)
{
	rotationMatrix = Matrix::CreateRotationX(rotation.x) * Matrix::CreateRotationY(rotation.y) * Matrix::CreateRotationZ(rotation.z);
}

void Mesh::setScale(Vector3 scale)
{
	scaleMatrix = Matrix::CreateScale(scale);
}

Matrix Mesh::getWorldmatrix()
{
	return translationMatrix * rotationMatrix * scaleMatrix;
}

void Mesh::setCBuffer(std::shared_ptr<ConstantBuffer> cb)
{
	WMBuffer = cb;
}

