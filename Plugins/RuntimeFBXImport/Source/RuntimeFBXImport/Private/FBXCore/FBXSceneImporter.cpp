// Copyright 2019-2023, Athian Games. All Rights Reserved. 


#include "FBXCore/FBXSceneImporter.h"

#include "MeshNode.h"
#include "RuntimeMeshFunctionLibrary.h"
#include "Windows/AllowWindowsPlatformTypes.h" 

FbxLayerElement::EReferenceMode NormalReferenceMode(FbxLayerElement::eDirect);
FbxLayerElement::EMappingMode NormalMappingMode(FbxLayerElement::eByControlPoint);

FbxLayerElementVertexColor* LayerElementVertexColor;;
FbxLayerElement::EReferenceMode VertexColorReferenceMode(FbxLayerElement::eDirect);
FbxLayerElement::EMappingMode VertexColorMappingMode(FbxLayerElement::eByControlPoint);

FbxLayerElement::EReferenceMode TangentReferenceMode(FbxLayerElement::eDirect);
FbxLayerElement::EMappingMode TangentMappingMode(FbxLayerElement::eByControlPoint);

UFBXSceneImporter::UFBXSceneImporter()
{
	RequestDestroy = false;
}

UFBXSceneImporter::~UFBXSceneImporter()
{
	Destroy();
}

void UFBXSceneImporter::Destroy()
{
	RequestDestroy = true;

	if (lSdkManager)
	{
		DeleteObject(lSdkManager);

	}

	IsImportFinished = false;
	lImportStatus = false;

}

void UFBXSceneImporter::BeginDestroy()
{
	RequestDestroy = true;
	UObject::BeginDestroy();
}

void UFBXSceneImporter::GetAllMeshNodes(const TArray<UMeshNode*>& Nodes, TArray<UMeshNode_Geometry*>& OutMeshNodes)
{
	for (UMeshNode* Node : Nodes)
	{
		if (UMeshNode_Geometry* MeshNode = Cast<UMeshNode_Geometry>(Node))
		{
			OutMeshNodes.Add(MeshNode);
		}
		
		GetAllMeshNodes(Node->GetChildren(), OutMeshNodes);
	}
}

TArray<UMeshNode*> UFBXSceneImporter::GetRootNodes()
{
	return RootNodes;
}

void UFBXSceneImporter::AddSkinWeightsToSection(FProceduralMeshData& MeshData, FbxMesh* Mesh)
{
	int DeformerCount = Mesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int DeformerIndex = 0; DeformerIndex < DeformerCount; DeformerIndex++)
	{
		FbxSkin* Skin = static_cast<FbxSkin*>(Mesh->GetDeformer(DeformerIndex, FbxDeformer::eSkin));

		int ClusterCount = Skin->GetClusterCount();
		for (int ClusterIndex = 0; ClusterIndex < ClusterCount; ClusterIndex++)
		{
			FbxCluster* Cluster = Skin->GetCluster(ClusterIndex);

			// The linked node is actually the bone
			FbxNode* LinkedNode = Cluster->GetLink();
			if (!LinkedNode)
				continue;

			UE_LOG(LogTemp, Warning, TEXT("Found Cluster (Bone): %s"), *FString(LinkedNode->GetName()));

			int* VertexIndices = Cluster->GetControlPointIndices();
			double* VertexWeights = Cluster->GetControlPointWeights();

			int VertexCount = Cluster->GetControlPointIndicesCount();
			for (int VertexIndex = 0; VertexIndex < VertexCount; VertexIndex++)
			{
				// Add skin weights to your own data structure
				
			}
		}
	}
}

int32 ComputeUVHash(const FVector2D& UV)
{
	int32 UHash = UV.X * 10000;
	int32 VHash = UV.Y * 10000;
	return (UHash << 16) | (VHash & 0xFFFF);
}

int32 ComputeCompositeKey(int32 ControlPointIndex, const FVector2D& UV)
{
	int32 UVHash = ComputeUVHash(UV);
	return ControlPointIndex ^ UVHash; // XOR as a simple way to combine the two
}


TMap<int32, FMaterialProperties> UFBXSceneImporter::CreateNodeMaterials(FbxMesh* MeshNode)
{
	FbxNode* Node = MeshNode->GetNode();
	int32 MaterialCount = Node->GetMaterialCount();
	
	TArray<FbxSurfaceMaterial*> UsedSurfaceMaterials;
	TSet<int32> UsedMaterialIndexes;

	TMap<int32, FMaterialProperties> MaterialPropertiesMap;

	if (MeshNode)
	{
		for (int32 ElementMaterialIndex = 0; ElementMaterialIndex < MeshNode->GetElementMaterialCount(); ++ElementMaterialIndex)
		{
			FbxLayerElementMaterial *ElementMaterial = MeshNode->GetLayer(0)->GetMaterials();

			
			int trianglecount = ElementMaterial->GetIndexArray().GetCount();
			switch (ElementMaterial->GetMappingMode())
			{
			case FbxLayerElement::eAllSame:
				{
					if (ElementMaterial->GetIndexArray().GetCount() > 0)
					{
						UsedMaterialIndexes.Add(ElementMaterial->GetIndexArray()[0]);
					}
				}
				break;
			case FbxLayerElement::eByPolygon:
				{
					for (int32 MaterialIndex = 0; MaterialIndex < trianglecount; MaterialIndex++)
					{
						UsedMaterialIndexes.Add(ElementMaterial->GetIndexArray()[MaterialIndex]);
					}
				}
				break;
			default: ;
			}
		}
	}


	for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
	{
		if (MeshNode == nullptr || UsedMaterialIndexes.Contains(MaterialIndex))
		{
			if (FbxSurfaceMaterial *FbxMaterial = Node->GetMaterial(MaterialIndex))
			{
				FMaterialProperties MProperties = FBXImportSettings->GetMaterialProperties(FbxMaterial, Node->GetName());
				MaterialPropertiesMap.Add(MaterialIndex, MProperties);
			}
		}
	}

	if(MaterialPropertiesMap.Num() == 0)
	{
		FMaterialProperties MProperties;
		MaterialPropertiesMap.Add(0, MProperties);
		UE_LOG(LogTemp, Warning, TEXT("No Material Found for Node: %s"), *FString(Node->GetName()));
	}
	
	return MaterialPropertiesMap;
}

void UFBXSceneImporter::AddVertexToSection(FProceduralMeshData& MeshData, const FbxMesh* Mesh, int32 TriangleIndex, int32 CornerIndex, TMap<int32, int32>& IndexMap,
	const FbxAMatrix& TotalMatrix, const FbxAMatrix& TotalMatrixForNormal)
{
	int ControlPointIndex = Mesh->GetPolygonVertex(TriangleIndex, CornerIndex);

	FVector2D NewUV(0,0);
	if (const FbxLayerElementUV* LayerElementUV = Mesh->GetLayer(0)->GetUVs())
	{
		int UVMapIndex = (LayerElementUV->GetMappingMode() == FbxLayerElement::eByControlPoint) ?
			ControlPointIndex : TriangleIndex * 3 + CornerIndex;

		int32 UVIndex = (LayerElementUV->GetReferenceMode() == FbxLayerElement::eDirect) ?
			UVMapIndex : LayerElementUV->GetIndexArray().GetAt(UVMapIndex);

		FbxVector2	UVVector = LayerElementUV->GetDirectArray().GetAt(UVIndex);
		NewUV = FVector2D(static_cast<float>(UVVector[0]), 1.f - static_cast<float>(UVVector[1]));
	}

	int32 CompositeKey = ComputeCompositeKey(ControlPointIndex, NewUV);

	if (int32* ExistingIndex = IndexMap.Find(CompositeKey))
	{
		MeshData.Triangles.Add(*ExistingIndex);
	}
	else
	{
		MeshData.UVs.Add(NewUV);
		
		FbxVector4 FbxPosition = Mesh->GetControlPoints()[ControlPointIndex];

		// Transform it using TotalMatrix
		FbxVector4 FinalPosition = TotalMatrix.MultT(FbxPosition);

		// Convert to FVector
		FVector VertexPosition = FBXHelper::ConvertPos(FinalPosition, FBXImportSettings->SpawnTransform.GetScale3D());

		int vertex_index = MeshData.Vertices.Add(VertexPosition);
		MeshData.Triangles.Add(vertex_index);
	
		if (const FbxLayerElementNormal* LayerElementNormal = Mesh->GetLayer(0)->GetNormals())
		{
			NormalMappingMode = LayerElementNormal->GetMappingMode();
			NormalReferenceMode = LayerElementNormal->GetReferenceMode();

			int TriangleCornerIndex = TriangleIndex * 3 + CornerIndex;
			int NormalMapIndex = (NormalMappingMode == FbxLayerElement::eByControlPoint) ? ControlPointIndex : TriangleCornerIndex;
			int NormalValueIndex = (NormalReferenceMode == FbxLayerElement::eDirect) ? NormalMapIndex : LayerElementNormal->GetIndexArray().GetAt(NormalMapIndex);

			FbxVector4 TempValue = LayerElementNormal->GetDirectArray().GetAt(NormalValueIndex);

			// Use inverse transpose for transforming the normal
			FbxAMatrix InverseTranspose = TotalMatrixForNormal.Transpose();
			InverseTranspose = InverseTranspose.Inverse();
			TempValue = InverseTranspose.MultT(TempValue);

			FVector TangentZ = FBXHelper::ConvertDir(TempValue);
			MeshData.Normals.Add(TangentZ.GetSafeNormal());
		}

		if (const FbxLayerElementTangent* LayerElementTangent = Mesh->GetLayer(0)->GetTangents())
		{
			int32 TangentMappingIndex = (TangentMappingMode == FbxLayerElement::eByControlPoint) ?
			ControlPointIndex : TriangleIndex * 3 + CornerIndex;

			int32 TangentValueIndex = (TangentReferenceMode == FbxLayerElement::eDirect) ?
			TangentMappingIndex : LayerElementTangent->GetIndexArray().GetAt(TangentMappingIndex);

			FbxVector4 TangentValue = LayerElementTangent->GetDirectArray().GetAt(TangentValueIndex);

			FVector TangentX = FBXHelper::ConvertDir(TangentValue);
			FProcMeshTangent PTangent = FProcMeshTangent(TangentX.GetSafeNormal(), false);
			MeshData.Tangents.Add(PTangent);
		}

		if (const FbxLayerElementVertexColor* LayerElementVertexColor = Mesh->GetLayer(0)->GetVertexColors())
		{
			int32 VertexColorMappingIndex = (LayerElementVertexColor->GetMappingMode() == FbxLayerElement::eByControlPoint) ? ControlPointIndex : Mesh->GetPolygonVertex(TriangleIndex, CornerIndex);
			int32 VertexColorValueIndex = (LayerElementVertexColor->GetReferenceMode() == FbxLayerElement::eDirect) ? VertexColorMappingIndex : LayerElementVertexColor->GetIndexArray().GetAt(VertexColorMappingIndex);

			FbxColor VertexColor = LayerElementVertexColor->GetDirectArray().GetAt(VertexColorValueIndex);

			MeshData.VertexColors.Add(FColor(
				static_cast<uint8>(255.f * VertexColor.mRed),
				static_cast<uint8>(255.f * VertexColor.mGreen),
				static_cast<uint8>(255.f * VertexColor.mBlue),
				static_cast<uint8>(255.f * VertexColor.mAlpha)
			));
		}
		else
		{
			MeshData.VertexColors.Add(FColor::White);
		}

		IndexMap.Add(ControlPointIndex, vertex_index);
	}
}

void UFBXSceneImporter::ProcessMesh(FbxMesh* Mesh, FbxAMatrix TotalMatrix, FbxAMatrix TotalMatrixForNormal, UMeshNode_Geometry*& GeometryNode)
{
	
	Mesh->RemoveBadPolygons();

	if (!Mesh->IsTriangleMesh())
	{
		FbxGeometryConverter GeometryConverter(Mesh->GetFbxManager());
		FbxNodeAttribute* ConvertedNode = GeometryConverter.Triangulate(Mesh->GetNode()->GetNodeAttribute(), true);

		if (ConvertedNode != nullptr && ConvertedNode->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			Mesh = static_cast<FbxMesh*>(ConvertedNode);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Found polygon that is not a triangle!"));
			return;
		}
	}

	const FbxLayer* BaseLayer = Mesh->GetLayer(0);
	if (!BaseLayer)
	{
		UE_LOG(LogTemp, Error, TEXT("No Layers in the Mesh"));
		return;
	}

	const FbxLayerElementMaterial* LayerElementMaterial = BaseLayer->GetMaterials();
	FbxLayerElement::EMappingMode MaterialMappingMode = LayerElementMaterial ?
		LayerElementMaterial->GetMappingMode() : FbxLayerElement::eByPolygon;

	int32 MaterialCount = Mesh->GetNode()->GetMaterialCount();

	int32 TriangleCount = Mesh->GetPolygonCount();
	if (TriangleCount == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Triangle Count is 0"));
		return;
	}

	TMap<int32, FMaterialProperties> MaterialPropertiesMap = CreateNodeMaterials(Mesh);
	TMap<int32, FProceduralMeshData> ProcMeshSectionMap;
	

	TMap<int32, TMap<int32, int32>> MaterialIndexToIndexMap;
	for (int TriangleIndex = 0; TriangleIndex < TriangleCount; TriangleIndex++)
	{
		if(RequestDestroy)
		{
			break;
		}
		
		int32 MaterialIndex = 0;
		if (MaterialCount > 0)
		{
			if (LayerElementMaterial)
			{
				switch (MaterialMappingMode)
				{
				case FbxLayerElement::eAllSame:
					{
						MaterialIndex = LayerElementMaterial->GetIndexArray().GetAt(0);
					}
					break;
				case FbxLayerElement::eByPolygon:
					{
						MaterialIndex = LayerElementMaterial->GetIndexArray().GetAt(TriangleIndex);
					}
					break;
				default: ;
				}
			}
		}

		if (MaterialIndex >= MaterialCount || MaterialIndex < 0)
		{
			MaterialIndex = 0;
		}

		if (!ProcMeshSectionMap.Contains(MaterialIndex))
		{
			FProceduralMeshData ProceduralMeshData;

			if (MaterialPropertiesMap.Contains(MaterialIndex))
			{
				ProceduralMeshData.MaterialProperties = MaterialPropertiesMap[MaterialIndex];
			}
        
			ProcMeshSectionMap.Add(MaterialIndex, ProceduralMeshData);

			// Initialize the IndexMap for this MaterialIndex
			MaterialIndexToIndexMap.Add(MaterialIndex);
		}

		FProceduralMeshData& ProceduralMeshData = ProcMeshSectionMap[MaterialIndex];
		TMap<int32, int32>& IndexMap = MaterialIndexToIndexMap[MaterialIndex];

		int polyVertCount = Mesh->GetPolygonSize(TriangleIndex);
		for (int CornerIndex = 0; CornerIndex < polyVertCount; CornerIndex++)
		{
			if(RequestDestroy)
			{
				break;
			}
			
			AddVertexToSection(ProceduralMeshData, Mesh, TriangleIndex, CornerIndex, IndexMap, TotalMatrix, TotalMatrixForNormal);
		}
	}

	if(!GeometryNode)
	{
		GeometryNode = NewObject<UMeshNode_Geometry>();
	}
	
	TArray<int32> SectionIndices;
	ProcMeshSectionMap.GetKeys(SectionIndices);
	for (int32 Index : SectionIndices)
	{
		if(RequestDestroy)
		{
			break;
		}
		GeometryNode->ProceduralMeshSectionData.Add(ProcMeshSectionMap[Index]);
	}

}

void UFBXSceneImporter::ProcessNode(UMeshNode* CurrentNode, FbxAMatrix ParentGlobalMatrix, int TotalNodeCount, int& ProcessedNodeCount)
{
    if(RequestDestroy)
    {
        return;
    }

	if(CurrentNode)
	{
		if(FbxNode* Node = CurrentNode->Node)
		{
			FbxAMatrix GlobalTransformMatrix, TotalMatrix, TotalMatrixForNormal;
			FBXHelper::ComputeTotalMatrix(FBXImportSettings->bTransformVertexToAbsolute, Node, ParentGlobalMatrix, GlobalTransformMatrix,
				TotalMatrix, TotalMatrixForNormal);

			if(UMeshNode_Geometry* NewMeshNode = Cast<UMeshNode_Geometry>(CurrentNode))
			{
				if(FbxMesh* Mesh = NewMeshNode->Node->GetMesh())
				{
					ProcessMesh(Mesh, TotalMatrix, TotalMatrixForNormal, NewMeshNode);
					AsyncTask(ENamedThreads::GameThread, [this, NewMeshNode]()
					{
						OnMeshNodeCreated.Broadcast(FBXImportSettings->ImportID, NewMeshNode, FBXImportSettings->SpawnTransform);
					});
			
				}
		
			}

			for (UMeshNode* ChildNode : CurrentNode->GetChildren())
			{
				if(RequestDestroy)
				{
					break;
				}
				// Pass down the global transform matrix to the child nodes
				ProcessNode(ChildNode, GlobalTransformMatrix, TotalNodeCount, ProcessedNodeCount);
			}
				
			ProcessedNodeCount++;

			AsyncTask(ENamedThreads::GameThread, [this, ProcessedNodeCount, TotalNodeCount, Node]()
			{
				float Progress = (static_cast<float>(ProcessedNodeCount) / TotalNodeCount);
				OnNodeProcessingChanged.Broadcast(FBXImportSettings->ImportID, Node->GetName());
				OnImportProgressChanged.Broadcast(FBXImportSettings->ImportID, Progress);
			});

		}
    
	}
	
}

int32 UFBXSceneImporter::CreateNodes(FbxNode* Node, UMeshNode* ParentNode, int32 NodeIndex)
{
	int32 Count = 1; 

	UMeshNode* CurrentNode;

	// Fetching the Global Transformation for the current node.
	FbxAMatrix GlobalTransform = Node->EvaluateGlobalTransform();

	FbxGlobalSettings& GlobalSettings = Scene->GetGlobalSettings();
	FbxSystemUnit SceneUnit = GlobalSettings.GetSystemUnit();
	const double CentimeterScale = SceneUnit.GetScaleFactor() / FbxSystemUnit::cm.GetScaleFactor();
	
	// Extracting translation and rotation from the GlobalTransform.
	FbxVector4 FBXTranslation = GlobalTransform.GetT();
	FbxVector4 FBXRotation = GlobalTransform.GetR();

	// Converting FBX Translation to Unreal's coordinate system.
	FVector FBXPivotLocation(FBXTranslation[0] * CentimeterScale, -FBXTranslation[2] * CentimeterScale, FBXTranslation[1] * CentimeterScale);

	// Converting FBX Rotation to Unreal's coordinate system.
	FRotator FBXPivotRotation(FBXRotation[0], -FBXRotation[2], FBXRotation[1]);

	// Composing the transformation in Unreal's format.
	FTransform FBXPivotTransform(FBXPivotRotation, FBXPivotLocation, FVector(1.0f, 1.0f, 1.0f)); 

	// Combine it with your spawn transform.
	FTransform PivotTransform = FBXPivotTransform * FBXImportSettings->SpawnTransform;

	
	// Check if the node itself is a mesh
	if (Node->GetMesh())
	{
		UMeshNode_Geometry* NewMeshNode = NewObject<UMeshNode_Geometry>();
	
		// Set the node name
		NewMeshNode->NodeName = FString(Node->GetName());
		NewMeshNode->Node = Node;
		NewMeshNode->PivotTransform = PivotTransform;
		// Set the parent-child relationship for the mesh nodes
		if (ParentNode)
		{
			ParentNode->AddChild(NewMeshNode);
		}
		else
		{
			RootNodes.Add(NewMeshNode);
		}

		CurrentNode = NewMeshNode;

	}
	else
	{
		// If the node is not a mesh, then process it as a regular node
		CurrentNode = NewObject<UMeshNode>();
		CurrentNode->NodeName = FString(Node->GetName());
		CurrentNode->Node = Node;
		CurrentNode->PivotTransform = PivotTransform;
		
		if (ParentNode)
		{
			ParentNode->AddChild(CurrentNode);
		}
		else
		{
			RootNodes.Add(CurrentNode);
		}
	}

	// Process each child node
	for (int ChildIndex = 0; ChildIndex < Node->GetChildCount(); ChildIndex++)
	{
		if(RequestDestroy)
		{
			break;
		}
		// Pass down the global transform matrix to the child nodes
		Count += CreateNodes(Node->GetChild(ChildIndex), CurrentNode, ChildIndex);
	}

	return Count;
}


void UFBXSceneImporter::ImportFBX()
{
	if(!FBXImportSettings)
		FBXImportSettings = MakeShareable(new FFBXImportSettings());
	
	lSdkManager = FbxManager::GetDefaultManager();

	if (!lSdkManager)
	{
		lSdkManager = FbxManager::Create();
	}
	
	if (lSdkManager)
	{
	
		FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
		lSdkManager->SetIOSettings(ios);

		FbxImporter* Importer = FbxImporter::Create(lSdkManager, "");

		ErrorMessage.Empty();

		std::string FilePathStr(TCHAR_TO_UTF8(*FBXImportSettings->Filepath));
		const char* lFilename = FilePathStr.c_str();

		if(Importer->Initialize(lFilename, - 1, lSdkManager->GetIOSettings()))
		{
			std::string fname(TCHAR_TO_UTF8(*("MyScene" + FString::FromInt(FBXImportSettings->ImportID))));
			char* fn = _strdup(fname.c_str());

			Scene = FbxScene::Create( lSdkManager, fn);
			Importer->Import(Scene);
			Importer->Destroy();
			ios->Destroy();
		
			if(Scene)
			{
				// // create scene info
				// FbxDocumentInfo* SceneInfo =  FbxDocumentInfo::Create(lSdkManager,"SceneInfo");
				// SceneInfo->mTitle = "Unreal FBX Importer";
				// SceneInfo->mSubject = "Import FBX meshes to Unreal";
				// SceneInfo->Original_ApplicationVersion.Set( TCHAR_TO_UTF8(*FEngineVersion::Current().ToString()) );
				// SceneInfo->LastSaved_ApplicationVersion.Set( TCHAR_TO_UTF8(*FEngineVersion::Current().ToString()) );
				//
				// Scene->SetSceneInfo(SceneInfo);

				FBXHelper::ConvertScene(Scene, FBXImportSettings->FBXAxis);

				AsyncTask(ENamedThreads::GameThread, [this]()
				{
					OnImportProgressChanged.Broadcast(FBXImportSettings->ImportID, 0.0f);
				});
			
				RootNodes.Empty();
			
				if (FbxNode* RootNode = Scene->GetRootNode())
				{
					int TotalNodeCount = CreateNodes(RootNode, nullptr, 0);
					if(RootNodes.Num() == 1)
					{
						int ProcessedNodeCount = 0;
						FbxAMatrix lParentGX;
						lParentGX.SetIdentity();
						ProcessNode(RootNodes[0], lParentGX, TotalNodeCount, ProcessedNodeCount);
			
						Destroy();
					}
				}
			}
			else
			{
				ErrorMessage = FString(Importer->GetStatus().GetErrorString());
				UE_LOG(LogTemp, Error, TEXT("Error Importing FBX file: %s"), *FString(ErrorMessage));
			}

		}
	}


}


