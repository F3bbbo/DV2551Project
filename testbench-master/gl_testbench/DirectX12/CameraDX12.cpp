#include "CameraDX12.h"
#include "IA.h"

CameraDX12::CameraDX12(float width, float height, float rotationSpeed, float walkSpeed, float runSpeed)
{
	this->rotationSpeed = rotationSpeed;
	speed = walkSpeed;
	this->runSpeed = runSpeed;

	position = Vector3(0.f, 0.f, -1.f);
	rightVector = Vector3(1.f, 0.f, 0.f);
	upVector = Vector3(0.f, 1.f, 0.f);
	forwardVector = Vector3(0.f, 0.f, 1.f);

	viewMatrix = DirectX::XMMatrixLookAtLH(position, forwardVector + position, upVector);
	projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(3.141592f * 0.5f, width / height, 0.1f, 10000.f);
	VPMatrix = viewMatrix * projectionMatrix;
}

CameraDX12::~CameraDX12()
{

}

void CameraDX12::moveCamera(Vector3 translation, bool run)
{
	position += translation * (run ? runSpeed : speed);
}

void CameraDX12::setCBuffer(std::shared_ptr<ConstantBuffer> cb)
{
	cBuffer = cb;
}

void CameraDX12::setViewMatrix(Matrix& matrix)
{
	viewMatrix = matrix;
	VPMatrix = viewMatrix * projectionMatrix;
}

void CameraDX12::setProjectionMatrix(Matrix& matrix)
{
	projectionMatrix = matrix;
	VPMatrix = viewMatrix * projectionMatrix;
}

void CameraDX12::setRight(Vector3 & vector)
{
	rightVector = vector;
}

void CameraDX12::setForward(Vector3 & vector)
{
	forwardVector = vector;
}

void CameraDX12::setUp(Vector3 & vector)
{
	upVector = vector;
}

void CameraDX12::bind()
{
	cBuffer->bind();
}

void CameraDX12::update()
{
	viewMatrix = DirectX::XMMatrixLookAtLH(position, forwardVector + position, upVector);
	VPMatrix = viewMatrix * projectionMatrix;
	VPMatrix = VPMatrix.Transpose();
	
	cBuffer->setData(&VPMatrix, sizeof(VPMatrix), nullptr, VPMATRIX_SLOT);
//	cBuffer->bind();
}

Matrix CameraDX12::getViewMatrix()
{
	return viewMatrix;
}

Matrix CameraDX12::getProjectionMatrix()
{
	return projectionMatrix;
}

Matrix CameraDX12::getVPMatrix()
{
	return VPMatrix;
}

Vector3 CameraDX12::getForward()
{
	return forwardVector;
}

Vector3 CameraDX12::getPosition()
{
	return position;
}

Vector3 CameraDX12::getRight()
{
	return rightVector;
}

Vector3 CameraDX12::getUp()
{
	return upVector;
}
