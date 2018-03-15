#include "CameraDX12.h"
#include "IA.h"

CameraDX12::CameraDX12(float width, float height, float rotationSpeed, float walkSpeed, float runSpeed)
{
	this->rotationSpeed = rotationSpeed;
	speed = walkSpeed;
	this->runSpeed = runSpeed;
	rotatecounter = 0;
	rotatecounterPitch = 0;
	position = Vector3(0.f, 0.f, -1.f);
	rightVector = Vector3(1.f, 0.f, 0.f);
	upVector = Vector3(0.f, 1.f, 0.f);
	forwardVector = Vector3(0.f, 0.f, 1.f);
	camPitch = 0;
	camYaw = 0;
	camRoll = 0;
	viewMatrix = DirectX::XMMatrixLookAtLH(position, forwardVector + position, upVector);
	projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(3.141592f * 0.5f, width / height, 0.1f, 10000.f);
	VPMatrix = viewMatrix * projectionMatrix;
}

CameraDX12::~CameraDX12()
{

}

void CameraDX12::moveCamera(Vector3 translation, bool run,float dt)
{
	position += translation * (run ? runSpeed : speed)*dt;
}

void CameraDX12::moveCamera(Vector3 positionchange)
{
	position = positionchange ;
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
	
	DirectX::SimpleMath::Vector3 test = DirectX::XMVector3Normalize(DirectX::XMVector3TransformCoord(forwardVector,DirectX::XMMatrixRotationRollPitchYaw(camPitch, camYaw, camRoll)));
	DirectX::XMMATRIX rotation;
	rotation = DirectX::XMMatrixRotationY(camYaw);
//	rightVector =DirectX::XMVector3TransformCoord(rightVector,rotation);
	upVector = DirectX::XMVector3TransformCoord(upVector, rotation);

//	forwardVector = DirectX::XMVector3TransformCoord(forwardVector, rotation);
	viewMatrix = DirectX::XMMatrixLookAtLH(position, test + position, upVector);
	VPMatrix = viewMatrix * projectionMatrix;
	VPMatrix = VPMatrix.Transpose();
	
	cBuffer->setData(&VPMatrix, sizeof(VPMatrix), nullptr, VPMATRIX_SLOT);
//	cBuffer->bind();
}

bool CameraDX12::rotatecameracamYaw(float rotatedegree)
{



	if (rotatecounter >= rotatedegree-0.1 && rotatecounter <= rotatedegree +0.1)
	{
		rotatecounter = 0;
		return true;
	}
	else
	{
		if (rotatedegree < 0)
		{
			camYaw = camYaw - 0.001;
			rotatecounter = rotatecounter - 0.001;
		}
		else
		{
		camYaw = camYaw + 0.001;
		rotatecounter = rotatecounter + 0.001;
		}
		return false;
	}
}


bool CameraDX12::rotatecameracamPitch(float rotatedegree)
{
	if (rotatecounterPitch >= rotatedegree)
	{
		rotatecounterPitch = 0;
		return true;
	}
	else
	{
		if (rotatedegree < 0)
		{
			camPitch = camPitch - 0.001;
		}
		else
		camPitch = camPitch + 0.001;
		rotatecounterPitch = rotatecounterPitch + 0.001;
		return false;
	}
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
