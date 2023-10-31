// Copyright 2019-2023, Athian Games. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MaterialLibraryAsset.generated.h"

USTRUCT(BlueprintType)
struct FMaterialInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	UTexture2D* Thumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	UMaterialInterface* Material;

};

USTRUCT(BlueprintType)
struct FTextureInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	UTexture2D* Texture;

};

USTRUCT(BlueprintType)
struct FMaterialGroup
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	FString TypeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	TArray<FMaterialInfo> Materials;
};

USTRUCT(BlueprintType)
struct FMeshTextureGroup
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	FString TypeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	TArray<FTextureInfo> Textures;
};

/**
 * 
 */
UCLASS()
class RUNTIMEFBXIMPORT_API UMaterialLibraryAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	TArray<FMaterialGroup> MaterialGroups;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	TArray<FMeshTextureGroup> TextureGroups;

	UFUNCTION(BlueprintCallable, Category = "Materials")
	FMaterialGroup GetMaterialGroup(FString TypeName);

	UFUNCTION(BlueprintCallable, Category = "Textures")
	FMeshTextureGroup GetTextureGroup(FString TypeName);
};
