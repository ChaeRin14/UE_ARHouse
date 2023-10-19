// Copyright 2019-2023, Athian Games. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Materials/MaterialInterface.h"
#include "MaterialImportSettings.generated.h"


UENUM(BlueprintType)
enum class EMeshTextureType : uint8
{
	Diffuse             UMETA(DisplayName = "Diffuse"),
	Specular            UMETA(DisplayName = "Specular"),
	Ambient             UMETA(DisplayName = "Ambient"),
	Emissive            UMETA(DisplayName = "Emissive"),
	Height              UMETA(DisplayName = "Height"),
	Normal              UMETA(DisplayName =  "Normal"),
	Shininess           UMETA(DisplayName = "Shininess"),
	Opacity             UMETA(DisplayName = "Opacity"),
	
};

UENUM(BlueprintType)
enum class EMeshColorType : uint8
{
	ColorDiffuse UMETA(DisplayName = "Color Diffuse"),
	ColorAmbient UMETA(DisplayName = "Color Ambient"),
	ColorOpacity UMETA(DisplayName = "Color Opacity"),
	ColorSpecular UMETA(DisplayName = "Color Specular"),
	ColorEmissive UMETA(DisplayName = "Color Emissive"),
};


UENUM(BlueprintType)
enum class EMeshScalarType : uint8
{
	Diffuse UMETA(DisplayName = "Diffuse"),
	Specular UMETA(DisplayName = "Specular"),
	Ambient UMETA(DisplayName = "Ambient"),
	Displacement UMETA(DisplayName = "Displacement"),
	Opacity UMETA(DisplayName = "Opacity"),
	BumpScaling UMETA(DisplayName = "BumpScaling"),
	Shininess UMETA(DisplayName = "Shininess"),
	Reflectivity UMETA(DisplayName = "Reflectivity"),
};

USTRUCT(BlueprintType)
struct FMeshTextureTypeMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	EMeshTextureType Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	FString Value;
};

USTRUCT(BlueprintType)
struct FMeshColorTypeMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	EMeshColorType Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	FString Value;
};

USTRUCT(BlueprintType)
struct FMeshScalarTypeMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	EMeshScalarType Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	FString Value;
};


USTRUCT(BlueprintType)
struct FTexturePathMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FBX Import Settings")
	FString RelativePath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FBX Import Settings")
	FString AbsolutePath;
};

USTRUCT(BlueprintType)
struct FNodeMaterialMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	FString NodeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* CustomMaterial;
};

/**
 * 
 */
UCLASS()
class RUNTIMEFBXIMPORT_API UMaterialImportSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	TArray<FMeshTextureTypeMapping> TextureTypeMappings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	TArray<FMeshColorTypeMapping> ColorTypeMappings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	TArray<FMeshScalarTypeMapping> ScalarTypeMappings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FBX Import Settings")
	TArray<FTexturePathMapping> TexturePathMappings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Settings")
	TArray<FNodeMaterialMapping> NodeMaterialMappings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Settings")
	UMaterialInterface* BaseMaterialOpaque;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Settings")
	UMaterialInterface* BaseMaterialMasked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Settings")
	UMaterialInterface* BaseMaterialTranslucent;
	
	bool ResolveTexturePath(FString MeshFilePath, FString& TexturePath);
};
