// Copyright 2019-2023, Athian Games. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "FBXMeshActor.h"
#include "PivotActorRotation.h"
#include "PivotActorScale.h"
#include "PivotActorTranslation.h"
#include "Actors/PivotActor.h"
#include "Engine/World.h"
#include "AsyncTasks/FBXAsyncTasks.h"
#include "GameFramework/Actor.h"

#include "FBXCore/FBXSceneImporter.h"

#include "FBXImportManager.generated.h"


UENUM(BlueprintType)
enum class EFBXCoordinate : uint8
{
	RightHanded   UMETA(DisplayName = "RightHanded"),
	LeftHanded 	    UMETA(DisplayName = "LeftHanded"),
};


UENUM(BlueprintType)
enum class EFBXAxis : uint8
{
	Front   UMETA(DisplayName = "Front"),
	Right 	UMETA(DisplayName = "Right"),
	Up 	    UMETA(DisplayName = "Up"),

};



UCLASS()
class RUNTIMEFBXIMPORT_API AFBXImportManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFBXImportManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	FVector CurrentScale;
	bool TransformVertexToAbsolute;
	EFBXCoordinate CurrentCoordinate;
	EFBXAxis CurrentUpVector;
	EFBXAxis CurrentFrontVector;

	UPROPERTY()
	AFBXMeshActor* CurrentSelectedActor;

	UPROPERTY()
	APivotActor* CurrentPivotActor;
	
	UPROPERTY(BlueprintReadOnly, Category="Import")
	TMap<int32, AFBXMeshActor*> ImportActorMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Import")
	UMaterialImportSettings* MaterialImportSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Import")
	TSubclassOf<AFBXMeshActor> FBXActorClass;

	UPROPERTY(EditAnywhere, Category = "Pivot")
	bool bShowPivot;
	
	UPROPERTY(EditAnywhere, Category = "Pivot", meta = (EditCondition = "bShowPivot"))
	TSubclassOf<APivotActorTranslation> PivotTranslationClass;

	UPROPERTY(EditAnywhere, Category = "Pivot", meta = (EditCondition = "bShowPivot"))
	TSubclassOf<APivotActorRotation> PivotRotationClass;

	UPROPERTY(EditAnywhere, Category = "Pivot", meta = (EditCondition = "bShowPivot"))
	TSubclassOf<APivotActorScale> PivotScaleClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Library")
	UMaterialLibraryAsset* MaterialLibrary;

	UFUNCTION(BlueprintPure, Category = "Mesh")
	TArray<FMaterialInfo> LoadMaterialsFromType(FString TypeName);

	UFUNCTION(BlueprintPure, Category = "Mesh")
	TArray<FTextureInfo> LoadTexturesFromType(FString TypeName);


	// An integer to keep track of the ImportID for each imported file.
	int32 CurrentImportID;
	int32 TotalNodeCount;
	TSharedPtr<FAsyncTask<FImportFBXTask>> ImportFBXTask;

	void ResetTask();
	
public:

	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	TArray<UFBXSceneImporter*> FBXSceneImporters;
	
	UFUNCTION(BlueprintCallable, Category = "Import")
	void InitializeFBXLoad(FVector Scale = FVector(1,1,1), bool TransformVertexToAbsolute = true, EFBXCoordinate Coordinate = EFBXCoordinate::RightHanded, EFBXAxis FrontVector = EFBXAxis::Front,
	                       EFBXAxis UpVector = EFBXAxis::Up);

	
	UFUNCTION(BlueprintCallable, Category = "Actor")
	void SelectActor(AFBXMeshActor* ActorToSelect, UPrimitiveComponent* SelectedComponent);

	UFUNCTION(BlueprintCallable, Category = "Actor")
	void HoverAtActor(AFBXMeshActor* ActorToHover, UPrimitiveComponent* HoveredComponent);

	UFUNCTION(BlueprintCallable, Category = "Actor")
	void ResetHoveredActor();

	UFUNCTION(BlueprintCallable, Category = "Actor")
	void ResetActorSelection();

	UFUNCTION()
	void HandleImportProgressChanged(int32 ImportID, float progress);

	UFUNCTION()
	void HandleNodeProcessingChanged(int32 ImportID, const FString& node_name);

	UFUNCTION()
	void HandleMeshNodeCreated(int32 ImportID, UMeshNode_Geometry* NewMeshNode, const FTransform& SpawnTransform);

	UFUNCTION()
	void HandleImportCompleted(UFBXSceneImporter* SceneImporter);

	UFUNCTION(BlueprintCallable, Category = "Import")
	void ImportFBXFile(const FString& FilePath, FVector Location, bool SpawnFBXActor = true);
	
	UFUNCTION(BlueprintCallable, Category = "Save")
	void SaveMeshActors(FString SaveSlotName, int32 SaveGameIndex);

	UFUNCTION(BlueprintCallable, Category = "Save")
	void LoadMeshActors(FString SaveSlotName, int32 SaveGameIndex);
	
	void AddMeshesToActor(AFBXMeshActor* Actor, UMeshNode* Node, const FTransform& SpawnTransform);

	UFUNCTION(BlueprintImplementableEvent, Category = "Import")
	void OnImportStarted(int32 ImportID, const FString& FileName);

	UFUNCTION(BlueprintImplementableEvent)
	void OnImportProgressChanged(int32 ImportID, float Progress);

	UFUNCTION(BlueprintImplementableEvent)
	void OnNodeProcessingChanged(int32 ImportID, const FString& NodeName);

	UFUNCTION(BlueprintImplementableEvent)
	void OnImportCompleted(AFBXMeshActor* MeshActor);

	UFUNCTION(BlueprintImplementableEvent)
	void OnFBXActorCreated(AFBXMeshActor* MeshActor);

	UPROPERTY(BlueprintReadWrite)
	AFBXMeshActor* FBXActor;
};
