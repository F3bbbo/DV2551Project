#pragma once
#include <d3d12.h>
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

class CameraDX12
{
private:
	Vector3 position;
	Vector3 forwardVector;
	Vector3 upVector;
	Vector3 rightVector;

	Matrix viewMatrix;
	Matrix projectionMatrix;
	Matrix VPMatrix;
public:
	CameraDX12(float width, float height, float rotationSpeed, float walkSpeed, float runSpeed);
	~CameraDX12();
	float rotationSpeed;
	float speed;
	float runSpeed;
	void moveCamera(Vector3 translation, bool run);
	void setViewMatrix(Matrix& matrix);
	void setProjectionMatrix(Matrix& matrix);
	void setRight(Vector3& vector);
	void setForward(Vector3& vector);
	void setUp(Vector3& vector);
	Matrix getViewMatrix();
	Matrix getProjectionMatrix();
	Matrix getVPMatrix();
	Vector3 getForward();
	Vector3 getPosition();
	Vector3 getRight();
	Vector3 getUp();
};