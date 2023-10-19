// Copyright 2019-2023, Athian Games. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "MeshNode.h"
#include "RuntimeMeshFunctionLibrary.h"
#include "FBXCore/FBXHelper.h"
#include "Misc/Paths.h"
#include "Misc/EngineVersion.h"
#include "Async/Async.h"
#include "FBXSceneImporter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnImportProgressChanged, int32, ImportID, float, Progress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnImportStarted, int32, ImportID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNodeProcessingChanged, int32, ImportID, const FString&, NodeName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnMeshNodeCreated, int32, ImportID, UMeshNode_Geometry*, NewMeshNode, const FTransform&, SpawnTransform);



USTRUCT()
struct FFBXImportSettings
{
	GENERATED_BODY()

	UPROPERTY()
	int32 ImportID;

	UPROPERTY()
	FVector FBXAxis;

	UPROPERTY()
	FTransform SpawnTransform;

	UPROPERTY()
	UMaterialImportSettings* MaterialImportSettings;

	UPROPERTY()
	bool bTransformVertexToAbsolute;
	
	UPROPERTY()
	FString Filepath;

	UPROPERTY()
	bool SpawnFBXActor;

	
	FString GetFileName()
	{
	    return FPaths::GetBaseFilename(Filepath);
	}

	FMaterialProperties GetMaterialProperties(FbxSurfaceMaterial* Material, const FString& NodeName)
	{
		FMaterialProperties MaterialProperties;
	    MaterialProperties.MaterialName = FString(Material->GetNameOnly());
	
		if(MaterialImportSettings && MaterialImportSettings->IsValidLowLevel())
		{
			UMaterialInterface* CustomMaterial = nullptr;

			for (int iIndex = 0; iIndex < MaterialImportSettings->NodeMaterialMappings.Num(); iIndex++)
			{
				if (MaterialImportSettings->NodeMaterialMappings[iIndex].NodeName == NodeName)
				{
					CustomMaterial = MaterialImportSettings->NodeMaterialMappings[iIndex].CustomMaterial;
					break;
				}
			}
			
			bool bHasAlphaSource = false;
			bool bHasOpacityTexture = false;
			FbxDouble3 TransparentColor = FbxDouble3(-1.0, -1.0, -1.0);
			FbxDouble TransparencyFactor = -1.0;
			
			// Loop through each texture type in the import settings
			for (const FMeshTextureTypeMapping& Mapping : MaterialImportSettings->TextureTypeMappings)
			{
				const char* FBXPropertyName = TCHAR_TO_UTF8(*FBXHelper::GetFBXTexturePropertyName(Mapping.Key));
				FbxProperty Prop = Material->FindProperty(FBXPropertyName);

				if (FbxFileTexture* FileTexture = FBXHelper::FindTexture(Prop))
				{
					// Check if this is an Opacity map, and if so, update material accordingly
					if (Mapping.Key == EMeshTextureType::Opacity) 
					{
						bHasOpacityTexture = true;
						if(FileTexture->GetAlphaSource() != FbxTexture::eNone)
						{
							// The texture has an alpha channel, so the material is translucent
							bHasAlphaSource = true;
						}
						
					}

					FString TexturePath = FileTexture->GetFileName();
					
					// Resolve the texture path using the import settings
					if (MaterialImportSettings->ResolveTexturePath(Filepath, TexturePath))
					{
						TArray<uint8> RawFileData;
						FTextureData TextureData = URuntimeMeshFunctionLibrary::LoadTextureDatafromPath(TexturePath, RawFileData);
						MaterialProperties.Textures.Add(Mapping.Value, TextureData);
					}
					
				}
			}

			for (const auto& Mapping : MaterialImportSettings->ColorTypeMappings)
			{
				const char*  FBXPropertyName = TCHAR_TO_UTF8(*FBXHelper::GetFBXColorPropertyName(Mapping.Key));
				FbxProperty Prop = Material->FindProperty(FBXPropertyName);
				if (Prop.IsValid())
				{
					FbxDouble3 FbxColorValue = Prop.Get<FbxDouble3>();

					if (Mapping.Key == EMeshColorType::ColorOpacity)
					{
						TransparentColor = FbxColorValue;
					}
					FLinearColor UnrealColorValue(FbxColorValue[0], FbxColorValue[1], FbxColorValue[2]);
					MaterialProperties.Colors.Add(Mapping.Value, UnrealColorValue);
				
				}
			}

			for (const auto& Mapping : MaterialImportSettings->ScalarTypeMappings)
			{
				const char*  FBXPropertyName = TCHAR_TO_UTF8(*FBXHelper::GetFBXScalarPropertyName(Mapping.Key));
				FbxProperty Prop = Material->FindProperty(FBXPropertyName);
				if (Prop.IsValid())
				{
					
					FbxDouble FbxScalarValue = Prop.Get<FbxDouble>();

					if (Mapping.Key == EMeshScalarType::Opacity) 
					{
						TransparencyFactor = FbxScalarValue;
					}
					
					float UnrealScalarValue = static_cast<float>(FbxScalarValue);
					MaterialProperties.Scalars.Add(Mapping.Value, UnrealScalarValue);
				}
			}

			if (CustomMaterial)
			{
				// If a custom material was defined for this node, use it
				MaterialProperties.BaseMaterial = CustomMaterial;
			}
			else
			{
				// Calculate the total transparency value
				FbxDouble TotalTransparency = FMath::Max(TransparencyFactor, FMath::Max(TransparentColor[0], FMath::Max(TransparentColor[1], TransparentColor[2])));
				if(bHasOpacityTexture)
				{
					if (TotalTransparency > 0.0 && TotalTransparency < 1.0)
					{
						// If the total transparency is between 0.0 and 1.0, the material is Translucent
						MaterialProperties.BaseMaterial = MaterialImportSettings->BaseMaterialTranslucent;
					}
					else
					{
						if(bHasAlphaSource)
						{
							// The texture has an alpha channel, so the material is translucent
							MaterialProperties.BaseMaterial = MaterialImportSettings->BaseMaterialTranslucent;
						}
						else
						{
							MaterialProperties.BaseMaterial = MaterialImportSettings->BaseMaterialMasked;
						}
					}
				}
				else if (TotalTransparency > 0.0 && TotalTransparency < 1.0)
				{
					// If the total transparency is between 0.0 and 1.0, the material is Translucent
					MaterialProperties.BaseMaterial = MaterialImportSettings->BaseMaterialTranslucent;
				}
				else
				{
					// Otherwise, the material is Opaque
					MaterialProperties.BaseMaterial = MaterialImportSettings->BaseMaterialOpaque;
				}
			}
			
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("MaterialImportSettings not valid"));
		}
		return MaterialProperties;
	}
};

UCLASS()
class RUNTIMEFBXIMPORT_API UFBXSceneImporter : public UObject
{
	GENERATED_BODY()


	FbxManager* lSdkManager;
	FbxScene* Scene;
	bool IsImportFinished;
	bool lImportStatus;
	FbxAnimEvaluator* AnimEvaluator;


public:

	UPROPERTY()
	bool RequestDestroy;

	FOnImportProgressChanged OnImportProgressChanged;
	FOnNodeProcessingChanged OnNodeProcessingChanged;
	FOnMeshNodeCreated OnMeshNodeCreated;

	UPROPERTY()
	TArray<UMeshNode*> RootNodes;

	TSharedPtr<FFBXImportSettings> FBXImportSettings;

	UPROPERTY()
	FString ErrorMessage;

	UFBXSceneImporter();
	virtual ~UFBXSceneImporter() override;
	void Destroy();

	virtual void BeginDestroy() override;

    void SetImportSettings(TSharedPtr<FFBXImportSettings> InImportSettings)
    {
    	FBXImportSettings = InImportSettings;
    }

	TSharedPtr<FFBXImportSettings> GetImportSettings()
    {
	    return FBXImportSettings;
    }


	
	void ImportFBX();

	TMap<int32, FMaterialProperties> CreateNodeMaterials(FbxMesh* MeshNode);
	void AddSkinWeightsToSection(FProceduralMeshData& MeshData, FbxMesh* Mesh);
	void GetAllMeshNodes(const TArray<UMeshNode*>& Nodes, TArray<UMeshNode_Geometry*>& OutMeshNodes);

	TArray<UMeshNode*>  GetRootNodes();
	void ProcessMesh(FbxMesh* Mesh, FbxAMatrix TotalMatrix, FbxAMatrix TotalMatrixForNormal, UMeshNode_Geometry*& GeometryNode);
	void ProcessNode(UMeshNode* CurrentNode, FbxAMatrix ParentGlobalMatrix, int TotalNodeCount, int& ProcessedNodeCount);

	int32 CreateNodes(FbxNode* Node, UMeshNode* ParentNode, int32 NodeIndex);

	void AddVertexToSection(
		FProceduralMeshData& MeshData,
		const FbxMesh* Mesh, int32 TriangleIndex, int32 CornerIndex, TMap<int32, int32>& IndexMap, const FbxAMatrix&
		TotalMatrix, const FbxAMatrix& TotalMatrixForNormal);
};
