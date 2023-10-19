// Copyright 2019-2023, Athian Games. All Rights Reserved. 


#include "AsyncTasks/FBXAsyncTasks.h"

#include "Actors/FBXImportManager.h"


FImportFBXTask::FImportFBXTask(AFBXImportManager* InFBXImportManager, UFBXSceneImporter* InImporter):
 FBXImportManager(InFBXImportManager),FBXSceneImporter(InImporter)
{
}

void FImportFBXTask::DoWork()
{
	// Call your FBXImporter function here, which will process the FBX file
	FBXSceneImporter->ImportFBX();
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		if(FBXImportManager)
		{
			FBXImportManager->HandleImportCompleted(FBXSceneImporter);
		}
		
	});

}

void FImportFBXTask::Abandon()
{
	if(FBXSceneImporter)
	{
		FBXSceneImporter->RequestDestroy = true;
	}
}

