// Copyright 2019-2023, Athian Games. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "MeshNode.h"
#include "GameFramework/SaveGame.h"
#include "MeshSaveGame.generated.h"


USTRUCT(BlueprintType)
struct FMeshComponentSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FProceduralMeshData> ProceduralMeshSectionData;
	
	UPROPERTY()
	FTransform RelativeTransform;
	
	UPROPERTY()
	FProceduralMeshData MeshData;

	UPROPERTY()
	TArray<FMaterialRuntimeInfo> MaterialRuntimeInfoArray;
};

USTRUCT(BlueprintType)
struct FMeshSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<UMeshNode*> RootNodes;

	UPROPERTY()
	FTransform ActorTransform;

	UPROPERTY()
	TArray<FMeshComponentSaveData> MeshComponentSaveDataArray;
};

/**
 * 
 */
UCLASS()
class RUNTIMEFBXIMPORT_API UMeshSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	
	UPROPERTY(VisibleAnywhere, Category="SaveGame")
	TMap<int32, FMeshSaveData> SavedMeshes; // map of mesh actor names to their save data

	
};
