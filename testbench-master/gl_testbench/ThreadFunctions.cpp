#include "ThreadFunctions.h"

unsigned int __stdcall  threadfunctionloadingdata(void* data)
{
	threadinfo * threadInfo = (threadinfo*)data;
	for (;;)
	{
		threadInfo->done = true;
		if (threadInfo->shutDown)
			return 1;
		SuspendThread(GetCurrentThread());
		if (threadInfo->shutDown)
			return 1;
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
		}
		//Execute copy the data
		renderer->executeCopyCommandList(key);
		renderer->signalCopy(FENCEDONE, key);
		
		//Wait for direct queue if not done yet
		renderer->waitForDirect(FENCEDONE, INFINITY, key);
		renderer->resetDirectCommandList(key);
		//Change state of the textures
		for (unsigned int i = 0; i < (*outMeshList).size(); i++)
		{
			//Mesh commandlists to directCommandList
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
		//Change commandlist to mainthreads commandlist
		for (unsigned int i = 0; i < (*outMeshList).size(); i++)
		{
			Mesh* m = ((*outMeshList)[i]).get();
			renderer->setDirectList(m, MAIN_THREAD);
		}
		//Wait until copy queue is done before exiting.
		renderer->waitForCopy(FENCEDONE, INFINITY, key);
		renderer->resetCopyCommandList(key);
	}
}