// Copyright 2019-2023, Athian Games. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "FBXCore/FBXSceneImporter.h"
#include "Async/AsyncWork.h"

class AFBXImportManager;

class FImportFBXTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FImportFBXTask>;
	
	AFBXImportManager* FBXImportManager;
	UFBXSceneImporter* FBXSceneImporter;

public:

	FImportFBXTask(AFBXImportManager* InFBXImportManager, UFBXSceneImporter* InImporter);

	void DoWork();
	
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FImportFBXTask, STATGROUP_ThreadPoolAsyncTasks);
	}

	// Implement the CanAbandon and Abandon methods
	bool CanAbandon()
	{
		return true;
	}

	void Abandon();
};
