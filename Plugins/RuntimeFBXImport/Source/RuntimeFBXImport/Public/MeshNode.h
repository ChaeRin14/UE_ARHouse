// Copyright 2019-2023, Athian Games. All Rights Reserved. 

#pragma once


#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ProceduralMeshComponent.h"
#include "FBXCore/FBXHelper.h"
#include "MeshNode.generated.h"

USTRUCT(BlueprintType)
struct FTextureData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Width;

	UPROPERTY()
	int32 Height;

	UPROPERTY()
	TArray<uint8> UnCompressedData;
	
};

USTRUCT(BlueprintType)
struct FMaterialRuntimeInfo
{
	GENERATED_BODY()

	UPROPERTY()
	UMaterialInterface* BaseMaterial;
	
	UPROPERTY()
	TMap<FString, UTexture*> Textures;

	UPROPERTY()
	TMap<FString, FLinearColor> Colors;

	UPROPERTY()
	TMap<FString, float> Scalars;
};


USTRUCT(BlueprintType)
struct FMaterialProperties
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Defaults")
	FString MaterialName;
	
	UPROPERTY(BlueprintReadOnly, Category = "Defaults")
	TMap<FString, FTextureData> Textures;
	
	UPROPERTY(BlueprintReadOnly, Category = "Defaults")
	TMap<FString, FLinearColor> Colors;

	UPROPERTY(BlueprintReadOnly, Category = "Defaults")
	TMap<FString, float> Scalars;

	UPROPERTY(BlueprintReadOnly, Category = "Defaults")
	UMaterialInterface* BaseMaterial;

	FMaterialProperties()
	{
		BaseMaterial = nullptr;
		MaterialName = "";
	}

};

USTRUCT(BlueprintType)
struct FProceduralMeshData 
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Procedural Mesh Data")
	TArray<FVector> Vertices;

	UPROPERTY(BlueprintReadOnly, Category = "Procedural Mesh Data")
	TArray<int32> Triangles;

	UPROPERTY(BlueprintReadOnly, Category = "Procedural Mesh Data")
	TArray<FVector> Normals;

	UPROPERTY(BlueprintReadOnly, Category = "Procedural Mesh Data")
	TArray<FVector2D> UVs;

	UPROPERTY(BlueprintReadOnly, Category = "Procedural Mesh Data")
	TArray<FLinearColor> VertexColors;

	UPROPERTY(BlueprintReadOnly, Category = "Procedural Mesh Data")
	TArray<FProcMeshTangent> Tangents;

	UPROPERTY(BlueprintReadOnly, Category = "Thumbnail")
	UTexture2D* Thumbnail;
	
	UPROPERTY(BlueprintReadOnly, Category = "Node")
	FMaterialProperties MaterialProperties;

	FProceduralMeshData(): Thumbnail(nullptr)
	{
	}
};


UCLASS(BlueprintType, Blueprintable)
class RUNTIMEFBXIMPORT_API UMeshNode : public UObject
{
	GENERATED_BODY()

public:

	FbxNode* Node;
	
	UPROPERTY(BlueprintReadOnly, Category = "Mesh Node")
	FString NodeName;

	UPROPERTY(BlueprintReadOnly, Category = "Mesh Node")
	UMeshNode* Parent;

	UPROPERTY(BlueprintReadOnly, Category = "Mesh Node")
	FTransform PivotTransform;

	// Add this function to get the children nodes
	UFUNCTION(BlueprintCallable, Category = "Mesh Node")
	const TArray<UMeshNode*>&GetChildren() const { return Children; }

	void AddChild(UMeshNode* Child);

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Mesh Node")
	TArray<UMeshNode*> Children;

};

UCLASS(BlueprintType, Blueprintable)
class RUNTIMEFBXIMPORT_API UMeshNode_Geometry : public UMeshNode
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category = "Mesh Node")
	TArray<FProceduralMeshData> ProceduralMeshSectionData;

};