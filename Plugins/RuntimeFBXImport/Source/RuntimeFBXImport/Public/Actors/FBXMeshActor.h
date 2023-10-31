// Copyright 2019-2023, Athian Games. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeshNode.h"
#include "ProceduralMeshComponent.h"
#include "DataAssets/MaterialLibraryAsset.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "FBXMeshActor.generated.h"

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaterialSettings, int32, ImportID, float, Progress);

UENUM(BlueprintType)
enum class EMeshCollisionType : uint8
{
	NoCollision   UMETA(DisplayName = "NoCollision"),
	MeshCollision 	    UMETA(DisplayName = "MeshCollision"),
	MeshCollisionFromSource 	    UMETA(DisplayName = "MeshCollisionFromSource"),
	CustomCollisionFromSource   UMETA(DisplayName = "CustomCollisionFromSource"),

};

UCLASS(BlueprintType)
class RUNTIMEFBXIMPORT_API AFBXMeshActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFBXMeshActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	int32 MeshIndex;


protected:
	
	TArray<UProceduralMeshComponent*> CurrentSelectedComponents;
	UProceduralMeshComponent* CurrentHoveredComponent;

	UPROPERTY()
	TArray<UMeshNode*> RootNodes;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mesh")
	EMeshCollisionType CollisionType;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Mesh")
	FString MeshPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MateriaLibrary")
	FString MaterialLibraryTypeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MateriaLibrary")
	FString TextureLibraryTypeName;
	
	UPROPERTY()
	bool bIsImportCompleted;
	
	
	void SetupCollisionForMesh(const UMeshNode_Geometry* MeshNode, UProceduralMeshComponent*& Mesh);

	void AddMesh(UMeshNode_Geometry* MeshNode, FTransform SpawnTransform, TArray<FMaterialRuntimeInfo> MaterialInfos);

	void FinishImport(const TArray<UMeshNode*>& InRootNodes);

	TArray<FProceduralMeshData> GetAllSections(const UMeshNode_Geometry* MeshNode);
	

	UPROPERTY()
	TMap<UProceduralMeshComponent*, UMeshNode_Geometry*> MeshNodeMap;

		
	UFUNCTION(BlueprintImplementableEvent, Category = "Mesh")
	void OnActorSelected();

	UFUNCTION(BlueprintImplementableEvent, Category = "Mesh")
	void OnActorSelectionReset();

	UFUNCTION(BlueprintImplementableEvent, Category = "Mesh")
	void OnComponentHovered(UProceduralMeshComponent* MeshComponent);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Mesh")
	void OnComponentSelected(UProceduralMeshComponent* MeshComponent);

	UFUNCTION(BlueprintImplementableEvent, Category = "Mesh")
	void OnSectionReceived(const TArray<FProceduralMeshData>& SectionData, const TArray<FMaterialInfo>& MaterialInfo, const TArray<FTextureInfo>& TextureInfo);
	
	UFUNCTION(BlueprintPure, Category = "Mesh")
	TArray<UMeshNode*> GetRootNodes();

	UFUNCTION(BlueprintCallable, Category = "Mesh")
	void SelectActor();

	UFUNCTION(BlueprintCallable, Category = "Mesh")
	void ResetActorHover();
	
	UFUNCTION(BlueprintCallable, Category = "Mesh")
	void HoverAtComponent(UPrimitiveComponent* HoveredComponent);
	
	UFUNCTION(BlueprintCallable, Category = "Mesh")
	void ResetActorSelection();

	UFUNCTION(BlueprintCallable, Category = "Mesh")
	void DeSelectCurrentComponents();

	UFUNCTION(BlueprintCallable, Category = "Mesh")
	void SelectComponent(UPrimitiveComponent* HitComponent, TArray<FMaterialInfo> MaterialInfo, TArray<FTextureInfo> TextureInfo, bool bDeselectRest = true);

	UFUNCTION(BlueprintCallable, Category = "Mesh")
	void UpdateSectionMaterial(int32 SectionId, UMaterialInterface* Material, UProceduralMeshComponent* MeshComponent);

	UFUNCTION(BlueprintCallable, Category = "Mesh")
	void UpdateTextureParameter(int32 SectionId, FName ParameterName, UTexture2D* Texture, UProceduralMeshComponent* MeshComponent);

	UFUNCTION(BlueprintCallable, Category = "Mesh")
	void UpdateScalarParameter(int32 SectionId, FName ParameterName, float Value, UProceduralMeshComponent* MeshComponent);

	UFUNCTION(BlueprintCallable, Category = "Mesh")
	void UpdateVectorParameter(int32 SectionId, FName ParameterName, FLinearColor Value, UProceduralMeshComponent* MeshComponent);

};
