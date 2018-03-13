#include "ThreadFunctions.h"

unsigned int __stdcall  threadfunctionloadingdata(void* data)
{
	int i = 0;
	threadinfo * threadInfo = (threadinfo*)data;
	//	std::cout << threadinformation->data[i]->position.x << std::endl;
	//	Object** testdata = (Object**)data;
	//	std::cout << testdata[0]->position.x << std::endl;
	int x = 0;
	int y = 0;
	//float4 triNor[3] = { { 0.0f,  0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f, 0.0f } };
	//int triInd[3] = { 0, 1, 2 };
	//float2 triUV[3] = { { 0.5f,  -0.99f },{ 1.49f, 1.1f },{ -0.51, 1.1f } };
	int amount = threadInfo->size;
	int key = threadInfo->key;
	std::vector<std::shared_ptr<Mesh>>* outMeshList = threadInfo->meshes;
	DirectX12Renderer *renderer = threadInfo->renderer;
	for (int i = 0; i < amount; i++)
	{
		//Load the meshes
		int beforeSize = outMeshList->size();
		threadInfo->reader->LoadFromFile(threadInfo->data[i]->Name, threadInfo->data[i]->textureFileName, *outMeshList, key);

		//Fill data of alla added meshes from the file
		for (int j = beforeSize; j < outMeshList->size(); j++)
		{
			(*outMeshList)[j]->technique = *(threadInfo->technique);
			(*outMeshList)[j]->setRotation(threadInfo->data[i]->rotation);
			(*outMeshList)[j]->setScale(threadInfo->data[i]->scale);
			(*outMeshList)[j]->setTranslation(threadInfo->data[i]->position);
		}

		//std::shared_ptr<ConstantBuffer> cbmesh;
		// triangle geometry:
		//float4 triPos[3] = { { x * cellWidth,  0.05, y * cellHeight, 1.0f },{ x * cellWidth + 0.05, -0.05, y * cellHeight, 1.0f },{ x * cellWidth - 0.05, -0.05, y * cellHeight, 1.0f } };

		//std::shared_ptr<VertexBuffer> trianglePos = renderer->makeVertexBuffer(sizeof(triPos), VertexBuffer::DATA_USAGE::DONTCARE);
		//std::shared_ptr<VertexBuffer> triangleNor = renderer->makeVertexBuffer(sizeof(triNor), VertexBuffer::DATA_USAGE::DONTCARE);
		//std::shared_ptr<VertexBuffer> triangleUvs = renderer->makeVertexBuffer(sizeof(triUV), VertexBuffer::DATA_USAGE::DONTCARE);
		//std::shared_ptr<VertexBuffer> triangleInd = renderer->makeVertexBuffer(sizeof(triInd), VertexBuffer::DATA_USAGE::DONTCARE);

		////Create mesh
		//std::shared_ptr<Mesh> mesh = renderer->makeMesh(1);
		//trianglePos->setData(triPos, sizeof(triPos), 0);
		//mesh->addIAVertexBufferBinding(trianglePos, 0, ARRAYSIZE(triPos), sizeof(float4), POS);

		//triangleNor->setData(triNor, sizeof(triNor), 0);
		//mesh->addIAVertexBufferBinding(triangleNor, 0, ARRAYSIZE(triNor), sizeof(float4), NORM);

		//triangleUvs->setData(triUV, sizeof(triUV), 0);
		//mesh->addIAVertexBufferBinding(triangleUvs, 0, ARRAYSIZE(triUV), sizeof(float2), UVCOORD);

		//triangleInd->setData(triInd, sizeof(triInd), 0);
		//mesh->addIAVertexBufferBinding(triangleInd, 0, ARRAYSIZE(triInd), sizeof(float), INDEXBUFF);

		//mesh->technique = triangleT;
		//mesh->setRotation(threadinformation->data[i]->rotation);
		//mesh->setScale(threadinformation->data[i]->scale);
		//mesh->setTranslation(threadinformation->data[i]->position);
		//	mesh->setCBuffer(cbmesh);
		//	scene.push_back(mesh);

	}
	//Execute copy the data
	renderer->executeCopyCommandList(key);
	renderer->signalCopy(FENCEDONE, key);
	//Change state of the textures
	for (unsigned int i = 0; i < (*outMeshList).size(); i++)
	{
		//Mesh commandlists to directCommandList
		(*outMeshList)[i].get();
		Mesh* m = ((*outMeshList)[i]).get();
		renderer->setDirectList(m, key);
		Texture2DDX12* texture = dynamic_cast<Texture2DDX12*>((*outMeshList)[i]->textures[DIFFUSETEX_SLOT].get());
		if (texture == nullptr)
		{
			OutputDebugStringA("Error: Texture not of type Texture2DDX12.");
			return -1;
		}
		texture->setBindState();
	}

	//Wait until copy queue is done before exiting.
	renderer->waitForCopy(FENCEDONE, INFINITY, key);
	renderer->resetCopyCommandList(key);
	return 1;
}