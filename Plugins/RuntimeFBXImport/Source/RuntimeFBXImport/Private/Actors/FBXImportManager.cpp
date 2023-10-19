// Copyright 2019-2023, Athian Games. All Rights Reserved. 


#include "Actors/FBXImportManager.h"
#include "Actors/FBXMeshActor.h"
#include "AsyncTasks/FBXAsyncTasks.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/MeshSaveGame.h"
#include "Tasks/Task.h"

// Sets default values
AFBXImportManager::AFBXImportManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CurrentScale = FVector(1,1,1);
	CurrentImportID = 0;
	bShowPivot = true;
	
}

// Called when the game starts or when spawned
void AFBXImportManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFBXImportManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	ResetTask();
	Super::EndPlay(EndPlayReason);
}



// Called every frame
void AFBXImportManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFBXImportManager::InitializeFBXLoad(FVector Scale, bool bTransformVertexToAbsolute , EFBXCoordinate Coordinate, EFBXAxis FrontVector, EFBXAxis UpVector)
{
	CurrentScale = Scale;
	TransformVertexToAbsolute = bTransformVertexToAbsolute;
	CurrentCoordinate = Coordinate;
	CurrentUpVector = UpVector;
	CurrentFrontVector = FrontVector;
}

void AFBXImportManager::ImportFBXFile(const FString& FilePath, FVector Location, bool SpawnFBXActor)
{
	TSharedPtr<FFBXImportSettings> ImportSettings = MakeShareable(new FFBXImportSettings());
	ImportSettings->ImportID = CurrentImportID;
	ImportSettings->SpawnTransform = FTransform( FRotator(), Location, CurrentScale);
	ImportSettings->Filepath = FilePath;
	ImportSettings->FBXAxis = FVector(static_cast<uint8>(CurrentCoordinate), static_cast<uint8>(CurrentFrontVector), static_cast<uint8>(CurrentUpVector));
	ImportSettings->SpawnFBXActor = SpawnFBXActor;
	ImportSettings->MaterialImportSettings = MaterialImportSettings;
	ImportSettings->bTransformVertexToAbsolute = TransformVertexToAbsolute;
	
	UFBXSceneImporter* FBXSceneImporter = NewObject<UFBXSceneImporter>();
	FBXSceneImporter->SetImportSettings(ImportSettings);

	if(SpawnFBXActor)
	{
		UClass* CurrentActorClass;
		if(IsValid(FBXActorClass))
		{
			CurrentActorClass = FBXActorClass;
		}
		else
		{
			CurrentActorClass = AFBXMeshActor::StaticClass();
		}
		
		FActorSpawnParameters SpawnParams;
		AFBXMeshActor* FBXActor = GetWorld()->SpawnActor<AFBXMeshActor>(CurrentActorClass, ImportSettings->SpawnTransform.GetLocation(), FRotator::ZeroRotator, SpawnParams);
	
		ImportActorMap.Add(CurrentImportID, FBXActor);
		OnFBXActorCreated(FBXActor);
		
	}

	FBXSceneImporter->OnImportProgressChanged.AddDynamic(this, &AFBXImportManager::HandleImportProgressChanged);
	FBXSceneImporter->OnNodeProcessingChanged.AddDynamic(this, &AFBXImportManager::HandleNodeProcessingChanged);
	FBXSceneImporter->OnMeshNodeCreated.AddDynamic(this, &AFBXImportManager::HandleMeshNodeCreated);

	ResetTask();
	
	ImportFBXTask = MakeShareable(new FAsyncTask<FImportFBXTask>(this, FBXSceneImporter));
	ImportFBXTask->StartBackgroundTask();

	OnImportStarted(CurrentImportID, ImportSettings->GetFileName());

	// Increment CurrentImportID for this new import operation
	CurrentImportID++;

	FBXSceneImporters.Add(FBXSceneImporter);

}

void AFBXImportManager::AddMeshesToActor(AFBXMeshActor* Actor, UMeshNode* Node, const FTransform& SpawnTransform)
{
	if (UMeshNode_Geometry* MeshNode = Cast<UMeshNode_Geometry>(Node))
	{
		TArray<FMaterialRuntimeInfo> MaterialInfoArray;
		Actor->AddMesh(MeshNode, SpawnTransform, MaterialInfoArray);  // Assuming BaseMaterial is defined somewhere accessible
	}

	// Recursively call this function for each child node
	for (UMeshNode* ChildNode : Node->GetChildren())
	{
		AddMeshesToActor(Actor, ChildNode,SpawnTransform);
	}
}

void AFBXImportManager::ResetTask()
{
	if(ImportFBXTask.IsValid())
	{
		ImportFBXTask->TryAbandonTask();
		ImportFBXTask->Cancel();
		ImportFBXTask->EnsureCompletion();
		ImportFBXTask.Reset();
	}
}

void AFBXImportManager::HandleImportCompleted(UFBXSceneImporter* SceneImporter)
{
	// Check if the SceneImporter is valid and the FBXActor is valid (if it needs to be)
	if(SceneImporter && SceneImporter->IsValidLowLevel())
	{
		FBXSceneImporters.Remove(SceneImporter);
		
		// Get the root nodes from the importer
		TArray<UMeshNode*> RootNodes = SceneImporter->GetRootNodes();
		TSharedPtr<FFBXImportSettings> ImportSettings = SceneImporter->GetImportSettings();

		if(AFBXMeshActor** ImportActor = ImportActorMap.Find(ImportSettings->ImportID))
		{
			(*ImportActor)->FinishImport(RootNodes);
			(*ImportActor)->MeshPath = ImportSettings->Filepath;
			OnImportCompleted(*ImportActor);
		}
	}
}

void AFBXImportManager::HandleImportProgressChanged(int32 ImportID, float progress)
{
	UE_LOG(LogTemp, Log, TEXT("Import progress: %.2f%% for ImportID: %d"), progress * 100.0f, ImportID);
	OnImportProgressChanged(ImportID, progress);
}

void AFBXImportManager::HandleNodeProcessingChanged(int32 ImportID, const FString&  node_name)
{
	UE_LOG(LogTemp, Log, TEXT("Processing node: %s for ImportID: %d"), *node_name, ImportID);
	OnNodeProcessingChanged(ImportID, node_name);
}

void AFBXImportManager::HandleMeshNodeCreated(int32 ImportID, UMeshNode_Geometry* NewMeshNode, const FTransform& SpawnTransform)
{
	// Check if the mesh node is not null
	if (NewMeshNode != nullptr)
	{
		if(AFBXMeshActor** ImportActor = ImportActorMap.Find(ImportID))
		{
			TArray<FMaterialRuntimeInfo> MaterialInfoArray;
			(*ImportActor)->AddMesh(NewMeshNode, SpawnTransform, MaterialInfoArray);
		}
	}
}


void AFBXImportManager::SelectActor(AFBXMeshActor* ActorToSelect, UPrimitiveComponent* SelectedComponent)
{
	if(ActorToSelect)
	{
		if(CurrentSelectedActor != ActorToSelect)
		{
			ResetActorSelection();
			CurrentSelectedActor = ActorToSelect;
			CurrentSelectedActor->SelectActor();
		}
		else
		{
			if(SelectedComponent)
			{
				TArray<FMaterialInfo> MaterialInfo = LoadMaterialsFromType(CurrentSelectedActor->MaterialLibraryTypeName);
				TArray<FTextureInfo> TextureInfo = LoadTexturesFromType(CurrentSelectedActor->TextureLibraryTypeName);
				
				CurrentSelectedActor->SelectComponent(SelectedComponent, MaterialInfo, TextureInfo);
			}
			
		}
	}
}

void AFBXImportManager::HoverAtActor(AFBXMeshActor* ActorToHover, UPrimitiveComponent* HoveredComponent)
{
	if(ActorToHover)
	{
		if(CurrentSelectedActor == ActorToHover)
		{
			CurrentSelectedActor->HoverAtComponent(HoveredComponent);
		}
	}
}

void AFBXImportManager::ResetHoveredActor()
{
	if(CurrentSelectedActor)
	{
		CurrentSelectedActor->ResetActorHover();
	}
}

void AFBXImportManager::ResetActorSelection()
{
	if(CurrentSelectedActor)
	{
		CurrentSelectedActor->ResetActorSelection();
		CurrentSelectedActor = nullptr;
	}
}


void AFBXImportManager::SaveMeshActors(FString SaveSlotName, int32 SaveGameIndex)
{
	if(UMeshSaveGame* SaveGameInstance = Cast<UMeshSaveGame>(UGameplayStatics::CreateSaveGameObject(UMeshSaveGame::StaticClass())))
	{
		for(auto& MeshActorPair : ImportActorMap)
		{
			if(AFBXMeshActor* MeshActor = MeshActorPair.Value)
			{
				// Create a new MeshSaveData object for each actor
				FMeshSaveData MeshSaveData;
				MeshSaveData.ActorTransform = MeshActor->GetActorTransform();
				MeshSaveData.RootNodes = MeshActor->GetRootNodes();

				TArray<UProceduralMeshComponent*> ProceduralMeshes;
				MeshActor->MeshNodeMap.GetKeys(ProceduralMeshes);
				
				// Iterate over the procedural mesh components in the actor
				for (UProceduralMeshComponent* MeshComponent : ProceduralMeshes)
				{
					// Create a new MeshComponentSaveData object for each mesh component
					FMeshComponentSaveData ComponentSaveData;
					ComponentSaveData.RelativeTransform = MeshComponent->GetRelativeTransform();
					ComponentSaveData.ProceduralMeshSectionData = MeshActor->MeshNodeMap[MeshComponent]->ProceduralMeshSectionData;

					int32 NumMaterials = MeshComponent->GetNumMaterials();
					for (int32 i = 0; i < NumMaterials; ++i)
					{
						if (UMaterialInstanceDynamic* MaterialInstance = Cast<UMaterialInstanceDynamic>(MeshComponent->GetMaterial(i)))
						{
							FMaterialRuntimeInfo MaterialInfo;
							MaterialInfo.BaseMaterial = MeshComponent->GetMaterial(i);
							
							// Assuming you have some way of getting parameter names
							TArray<FMaterialParameterInfo> TextureParameterInfo;
							TArray<FMaterialParameterInfo> ScalarParameterInfo;
							TArray<FMaterialParameterInfo> ColorParameterInfo;
							TArray<FGuid> OutGuids;
							MaterialInstance->GetAllTextureParameterInfo(TextureParameterInfo, OutGuids);
							MaterialInstance->GetAllScalarParameterInfo(ScalarParameterInfo, OutGuids);
							MaterialInstance->GetAllTextureParameterInfo(ColorParameterInfo, OutGuids);
					
							// Store textures
							for (const FMaterialParameterInfo& ParamInfo : TextureParameterInfo)
							{
								UTexture* Texture;
								MaterialInstance->GetTextureParameterValue(ParamInfo.Name, Texture);
								if (Texture)
									MaterialInfo.Textures.Add(ParamInfo.Name.ToString(), Texture);
							}

							// Store colors
							for (const FMaterialParameterInfo& ParamInfo : ColorParameterInfo)
							{
								FLinearColor Color;
								MaterialInstance->GetVectorParameterValue(ParamInfo.Name, Color);
								MaterialInfo.Colors.Add(ParamInfo.Name.ToString(), Color);
							}

							// Store scalars
							for (const FMaterialParameterInfo& ParamInfo : ScalarParameterInfo)
							{
								float Scalar;
								MaterialInstance->GetScalarParameterValue(ParamInfo.Name, Scalar);
								MaterialInfo.Scalars.Add(ParamInfo.Name.ToString(), Scalar);
							}

							// Add to the runtime info array
							ComponentSaveData.MaterialRuntimeInfoArray.Add(MaterialInfo);
						}
					}
					MeshSaveData.MeshComponentSaveDataArray.Add(ComponentSaveData);
				}

				// Add the MeshSaveData to the array in the save game object
				SaveGameInstance->SavedMeshes.Add(MeshActorPair.Key, MeshSaveData);
				
			}
		}
		
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveSlotName, SaveGameIndex);
	}
}

void AFBXImportManager::LoadMeshActors(FString SaveSlotName, int32 SaveGameIndex)
{
    if (UMeshSaveGame* SaveGameInstance = Cast<UMeshSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveGameIndex)))
    {
        for (auto& SavedMeshPair : SaveGameInstance->SavedMeshes)
        {
            int32 ImportID = ImportActorMap.Num() + SavedMeshPair.Key;
            FMeshSaveData& MeshSaveData = SavedMeshPair.Value;

            // Create a new FBXActor for each saved mesh
            AFBXMeshActor* MeshActor = GetWorld()->SpawnActor<AFBXMeshActor>(AFBXMeshActor::StaticClass(), MeshSaveData.ActorTransform);
        	MeshActor->FinishImport(MeshSaveData.RootNodes);

        	// Restore the mesh components and their associated data
        	for (const FMeshComponentSaveData& ComponentSaveData : MeshSaveData.MeshComponentSaveDataArray)
        	{
        		UMeshNode_Geometry* MeshNode = NewObject<UMeshNode_Geometry>();
        		MeshNode->ProceduralMeshSectionData = ComponentSaveData.ProceduralMeshSectionData;
        		MeshActor->AddMesh(MeshNode, MeshSaveData.ActorTransform, ComponentSaveData.MaterialRuntimeInfoArray);
        	}
        	
            // Store the new actor in the ImportActorMap
            ImportActorMap.Add(ImportID, MeshActor);
        }
    }
}

TArray<FMaterialInfo> AFBXImportManager::LoadMaterialsFromType(FString TypeName)
{
	TArray<FMaterialInfo> MaterialInfo;
	if(MaterialLibrary)
	{
		for (auto& materialgroup : MaterialLibrary->MaterialGroups)
		{
			if(materialgroup.TypeName.Equals(TypeName))
			{
				MaterialInfo = materialgroup.Materials;
				break;
			}
		}
	}
	return MaterialInfo;
}

TArray<FTextureInfo> AFBXImportManager::LoadTexturesFromType(FString TypeName)
{
	TArray<FTextureInfo> TextureInfo;
	if(MaterialLibrary)
	{
		for (auto& texturegroup : MaterialLibrary->TextureGroups)
		{
			if(texturegroup.TypeName.Equals(TypeName))
			{
				TextureInfo = texturegroup.Textures;
				break;
			}
		}
	}
	return TextureInfo;
}
