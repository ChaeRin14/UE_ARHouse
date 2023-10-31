// Copyright 2019-2023, Athian Games. All Rights Reserved. 


#include "..\..\Public\Actors\FBXMeshActor.h"

// Sets default values
AFBXMeshActor::AFBXMeshActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshIndex = 0;
	bIsImportCompleted = false;


}

// Called when the game starts or when spawned
void AFBXMeshActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFBXMeshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFBXMeshActor::SetupCollisionForMesh(const UMeshNode_Geometry* MeshNode, UProceduralMeshComponent*& Mesh)
{
	bool IsCollisionMesh = MeshNode->NodeName.StartsWith("UCX_");
	if (IsCollisionMesh && CollisionType == EMeshCollisionType::CustomCollisionFromSource)
	{
		Mesh->SetHiddenInGame(true);
	}

	if (CollisionType == EMeshCollisionType::MeshCollision || IsCollisionMesh)
	{
		Mesh->bUseComplexAsSimpleCollision = true;
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		Mesh->bUseComplexAsSimpleCollision = false;
	}

	Mesh->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	
}

void AFBXMeshActor::AddMesh(UMeshNode_Geometry* MeshNode, FTransform SpawnTransform, TArray<FMaterialRuntimeInfo> MaterialInfos)
{
	
	UWorld* world = GetWorld();
	// Ensure that the world exists
	if (!world) 
	{
		UE_LOG(LogTemp, Error, TEXT("World does not exist. Cannot add mesh to FBXActor."));
		return;
	}

	if(!MeshNode)
	{
		UE_LOG(LogTemp, Error, TEXT("MeshNode does not exist. Cannot load"));
		return;
	}

	bool IsCollisionMesh = MeshNode->NodeName.StartsWith("UCX_");
	if (IsCollisionMesh && CollisionType == EMeshCollisionType::NoCollision)
	{
		return;
	}
	

	FString MeshName = FString::Printf(TEXT("ProceduralMesh%d"), MeshIndex);
	UProceduralMeshComponent* NewProceduralMesh = NewObject<UProceduralMeshComponent>(this, *MeshName, RF_Transactional);
	NewProceduralMesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	NewProceduralMesh->bUseAsyncCooking = true;
	NewProceduralMesh->bCastDynamicShadow = true;
	NewProceduralMesh->RegisterComponentWithWorld(world);
	NewProceduralMesh->SetWorldTransform(SpawnTransform);

	MeshNodeMap.Add(NewProceduralMesh, MeshNode);
	
	UE_LOG(LogTemp, Display, TEXT("Creating %d Mesh Sections for the Mesh %s"), MeshNode->ProceduralMeshSectionData.Num(), *MeshNode->NodeName);

	for (int iIndex = 0; iIndex < MeshNode->ProceduralMeshSectionData.Num(); iIndex++)
	{
		
		FProceduralMeshData MeshData = MeshNode->ProceduralMeshSectionData[iIndex];

		NewProceduralMesh->CreateMeshSection_LinearColor(iIndex, MeshData.Vertices, MeshData.Triangles, MeshData.Normals,
			MeshData.UVs, MeshData.VertexColors, MeshData.Tangents, true);

		if(CollisionType == EMeshCollisionType::MeshCollision || IsCollisionMesh)
		{
			NewProceduralMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}

		if (IsCollisionMesh && CollisionType == EMeshCollisionType::CustomCollisionFromSource)
		{
			NewProceduralMesh->SetHiddenInGame(true);
		}
		else
		{
			UMaterialInterface* ParentMaterial = nullptr;
			FMaterialRuntimeInfo MaterialInfo;
			bool bHasValidMaterialInfo = false;
			if(iIndex <MaterialInfos.Num())
			{
				MaterialInfo = MaterialInfos[iIndex];
				ParentMaterial = MaterialInfo.BaseMaterial;
				bHasValidMaterialInfo = true;
			}
		
			if(!ParentMaterial)
			{
				ParentMaterial = MeshData.MaterialProperties.BaseMaterial;
			}
			
			if(ParentMaterial)
			{
				UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(ParentMaterial, this);

				// Assign textures
				for (const auto& TextureEntry : MeshData.MaterialProperties.Textures)
				{
					FTextureData CurrentTextureData = TextureEntry.Value;

					if (UTexture2D* Texture = UTexture2D::CreateTransient(CurrentTextureData.Width, CurrentTextureData.Height, PF_B8G8R8A8))
					{
						FTexture2DMipMap& MipMap = Texture->GetPlatformData()->Mips[0];
						void* data = MipMap.BulkData.Lock(LOCK_READ_WRITE);
						FMemory::Memcpy(data, CurrentTextureData.UnCompressedData.GetData(), CurrentTextureData.UnCompressedData.Num());
						MipMap.BulkData.Unlock();

						Texture->UpdateResource();
			
						DynamicMaterialInstance->SetTextureParameterValue(*TextureEntry.Key, Texture);
					}

					// Assign colors
					for (const auto& ColorEntry : MeshData.MaterialProperties.Colors)
					{
						DynamicMaterialInstance->SetVectorParameterValue(*ColorEntry.Key, ColorEntry.Value);
					}
				
					// Assign scalar values
					for (const auto& ScalarEntry : MeshData.MaterialProperties.Scalars)
					{
						DynamicMaterialInstance->SetScalarParameterValue(*ScalarEntry.Key, ScalarEntry.Value);
					}

					if(bHasValidMaterialInfo)
					{
						// Assign textures
						for (const auto& TexturePair : MaterialInfo.Textures)
						{
							DynamicMaterialInstance->SetTextureParameterValue(FName(*TexturePair.Key), TexturePair.Value);
						}

						// Assign colors
						for (const auto& ColorPair : MaterialInfo.Colors)
						{
							DynamicMaterialInstance->SetVectorParameterValue(FName(*ColorPair.Key), ColorPair.Value);
						}
				
						// Assign scalar values
						for (const auto& ScalarPair : MaterialInfo.Scalars)
						{
							DynamicMaterialInstance->SetScalarParameterValue(FName(*ScalarPair.Key), ScalarPair.Value);
						}
					}
	

					NewProceduralMesh->SetMaterial(iIndex, DynamicMaterialInstance);
				}
			}
		}
		
	
	}
	
	MeshIndex++;
}

void AFBXMeshActor::FinishImport(const TArray<UMeshNode*>& InRootNodes)
{
	bIsImportCompleted = true;
	RootNodes = InRootNodes;
}

TArray<FProceduralMeshData> AFBXMeshActor::GetAllSections(const UMeshNode_Geometry* MeshNode)
{
	TArray<FProceduralMeshData>  SectionData;
	if(MeshNode)
	{
		SectionData = MeshNode->ProceduralMeshSectionData;
	}
	return SectionData;
}


TArray<UMeshNode*> AFBXMeshActor::GetRootNodes()
{
	return RootNodes;
}

void AFBXMeshActor::SelectActor()
{
	if(bIsImportCompleted)
	{
		TArray<UProceduralMeshComponent*> ProceduralMeshes;
		MeshNodeMap.GetKeys(ProceduralMeshes);
		
		for (auto Child : ProceduralMeshes)
		{
			if (Child)
			{
				Child->bRenderCustomDepth = true;
				Child->SetCustomDepthStencilValue(255);
				Child->ReregisterComponent();

			}
		}

		OnActorSelected();
	}
}

void AFBXMeshActor::ResetActorHover()
{
	if(bIsImportCompleted)
	{
		if(CurrentHoveredComponent)
		{
			CurrentHoveredComponent->SetCustomDepthStencilValue(255);
			CurrentHoveredComponent->ReregisterComponent();
		}
		
		CurrentHoveredComponent = nullptr;
	}
}

void AFBXMeshActor::HoverAtComponent(UPrimitiveComponent* HoveredComponent)
{
	if(bIsImportCompleted)
	{
		if(!CurrentSelectedComponents.Contains(HoveredComponent))
		{
			if(CurrentHoveredComponent != HoveredComponent)
			{
				if(CurrentHoveredComponent)
				{
					CurrentHoveredComponent->SetCustomDepthStencilValue(255);
					CurrentHoveredComponent->ReregisterComponent();
				}

				if(UProceduralMeshComponent* MeshComponent = Cast<UProceduralMeshComponent>(HoveredComponent))
				{
					MeshComponent->SetCustomDepthStencilValue(250);
					MeshComponent->ReregisterComponent();
					CurrentHoveredComponent = MeshComponent;
					OnComponentHovered(MeshComponent);
				}
			}
		}
	}
}

void AFBXMeshActor::ResetActorSelection()
{
	TArray<UProceduralMeshComponent*> ProceduralMeshes;
	MeshNodeMap.GetKeys(ProceduralMeshes);
	
	for (auto& Child : ProceduralMeshes)
	{
		if (Child)
		{
			Child->bRenderCustomDepth = false;
			Child->ReregisterComponent();

		}
	}
	
	CurrentSelectedComponents.Empty();
	OnActorSelectionReset();
}

void AFBXMeshActor::DeSelectCurrentComponents()
{
	for (auto Child : CurrentSelectedComponents)
	{
		if (Child)
		{
			Child->SetCustomDepthStencilValue(255);
			Child->ReregisterComponent();
		}
	}

	CurrentSelectedComponents.Empty();
}

void AFBXMeshActor::SelectComponent(UPrimitiveComponent* HitComponent, TArray<FMaterialInfo> MaterialInfo, TArray<FTextureInfo> TextureInfo, bool bDeselectRest)
{
	if(bIsImportCompleted)
	{
		if(bDeselectRest)
		{
			DeSelectCurrentComponents();
		}

		if(UProceduralMeshComponent* MeshComponent = Cast<UProceduralMeshComponent>(HitComponent))
		{
			if(UMeshNode_Geometry** SelectedNode = MeshNodeMap.Find(MeshComponent))
			{
				
				TArray<FProceduralMeshData> SectionData = GetAllSections(*SelectedNode);
				OnSectionReceived(SectionData, MaterialInfo, TextureInfo);
			}
			
			MeshComponent->SetCustomDepthStencilValue(100);
			MeshComponent->ReregisterComponent();
			
			CurrentSelectedComponents.Add(MeshComponent);
			OnComponentSelected(MeshComponent);

			CurrentHoveredComponent = nullptr;
		}
	}
}

void AFBXMeshActor::UpdateSectionMaterial(int32 SectionId, UMaterialInterface* Material, UProceduralMeshComponent* MeshComponent)
{
	if (Material && MeshComponent)
	{
		if(MeshComponent->GetNumSections() > SectionId)
		{
			UMaterialInstanceDynamic* DynamicMat = Cast<UMaterialInstanceDynamic>(Material);
			if (!DynamicMat)
			{
				DynamicMat = UMaterialInstanceDynamic::Create(Material, this);
			}

			if (DynamicMat)
			{
				MeshComponent->SetMaterial(SectionId, DynamicMat);
			}
		}
	}
}

void AFBXMeshActor::UpdateTextureParameter(int32 SectionId, FName ParameterName, UTexture2D* Texture, UProceduralMeshComponent* MeshComponent)
{
	if (Texture)
	{
		if(MeshComponent && MeshComponent->GetNumSections() > SectionId)
		{
			if (UMaterialInstanceDynamic* SectionMaterial = Cast<UMaterialInstanceDynamic>(MeshComponent->GetMaterial(SectionId)))
			{
				SectionMaterial->SetTextureParameterValue(ParameterName, Texture);
			}
		}
	}
}

void AFBXMeshActor::UpdateScalarParameter(int32 SectionId, FName ParameterName, float Value, UProceduralMeshComponent* MeshComponent)
{
	if(MeshComponent && MeshComponent->GetNumSections() > SectionId)
	{
		if (UMaterialInstanceDynamic* SectionMaterial = Cast<UMaterialInstanceDynamic>(MeshComponent->GetMaterial(SectionId)))
		{
			SectionMaterial->SetScalarParameterValue(ParameterName, Value);
		}
	}

}

void AFBXMeshActor::UpdateVectorParameter(int32 SectionId, FName ParameterName, FLinearColor Value, UProceduralMeshComponent* MeshComponent)
{
	if(MeshComponent && MeshComponent->GetNumSections() > SectionId)
	{
		if (UMaterialInstanceDynamic* SectionMaterial = Cast<UMaterialInstanceDynamic>(MeshComponent->GetMaterial(SectionId)))
		{
			SectionMaterial->SetVectorParameterValue(ParameterName, Value);
		}
	}
}


