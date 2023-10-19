// Copyright 2019-2023, Athian Games. All Rights Reserved. 


#include "FBXCore/FBXHelper.h"


FString FBXHelper::GetFBXTexturePropertyName(EMeshTextureType Key)
{
	switch(Key)
	{
	case EMeshTextureType::Diffuse:
		return FbxSurfaceMaterial::sDiffuse;
	case EMeshTextureType::Specular:
		return FbxSurfaceMaterial::sSpecular;
	case EMeshTextureType::Ambient:
		return FbxSurfaceMaterial::sAmbient;
	case EMeshTextureType::Emissive:
		return FbxSurfaceMaterial::sEmissiveFactor;
	case EMeshTextureType::Height:
		return FbxSurfaceMaterial::sBump;
	case EMeshTextureType::Normal:
		return FbxSurfaceMaterial::sNormalMap;
	case EMeshTextureType::Opacity:
		return FbxSurfaceMaterial::sTransparentColor;
	case EMeshTextureType::Shininess:
		return FbxSurfaceMaterial::sShininess;
		default:
			return "";
	}
}

FString FBXHelper::GetFBXColorPropertyName(EMeshColorType Key)
{
	switch(Key)
	{
	case EMeshColorType::ColorDiffuse:
		return FbxSurfaceMaterial::sDiffuse;
	case EMeshColorType::ColorAmbient:
		return FbxSurfaceMaterial::sAmbient;
	case EMeshColorType::ColorSpecular:
		return FbxSurfaceMaterial::sSpecular;
	case EMeshColorType::ColorEmissive:
		return FbxSurfaceMaterial::sEmissive;
	case EMeshColorType::ColorOpacity:
		return FbxSurfaceMaterial::sTransparentColor;
		default:
			return "";
	}
}

FString FBXHelper::GetFBXScalarPropertyName(EMeshScalarType Key)
{
	switch(Key)
	{
		
	case EMeshScalarType::Diffuse:
		return FbxSurfaceMaterial::sDiffuseFactor;
	case EMeshScalarType::Specular:
		return FbxSurfaceMaterial::sSpecularFactor;
	case EMeshScalarType::Ambient:
		return FbxSurfaceMaterial::sAmbientFactor;
	case EMeshScalarType::Displacement:
		return FbxSurfaceMaterial::sDisplacementFactor;
	case EMeshScalarType::Opacity:
		return FbxSurfaceMaterial::sTransparencyFactor;
	case EMeshScalarType::BumpScaling:
		return FbxSurfaceMaterial::sBumpFactor;
	case EMeshScalarType::Shininess:
		return FbxSurfaceMaterial::sShininess;
	case EMeshScalarType::Reflectivity:
		return FbxSurfaceMaterial::sReflectionFactor;
	default:
			return "";
	}
}

FbxFileTexture* FBXHelper::FindTexture(FbxProperty Property)
{
	FbxFileTexture* FileTexture = nullptr;
	if (!Property.IsValid())
	{
		return nullptr;
	}
    
	int32 TextureCount = Property.GetSrcObjectCount<FbxTexture>();
	if (TextureCount > 0)
	{
		
		for (int32 TextureIndex = 0; TextureIndex < TextureCount; ++TextureIndex)
		{
			FbxObject* TextureObject = Property.GetSrcObject(TextureIndex);
			if(TextureObject->Is<FbxFileTexture>())
			{
				FileTexture = Property.GetSrcObject<FbxFileTexture>(TextureIndex);
					
			}
			else if(TextureObject->Is<FbxLayeredTexture>())
			{
				if (FbxLayeredTexture* LayeredTexture = Property.GetSrcObject<FbxLayeredTexture>(TextureIndex))
				{
					int32 LayeredTextureCount = LayeredTexture->GetSrcObjectCount<FbxFileTexture>();
					for (int32 LTIndex = 0; LTIndex < LayeredTextureCount; ++LTIndex)
					{
						FileTexture = LayeredTexture->GetSrcObject<FbxFileTexture>(LTIndex);
						if (FileTexture)
						{
							break;
						}
					}
				}
			}

			if (FileTexture)
			{
				break;
			}
		}
	}
	
	return FileTexture;
}

FbxAMatrix FBXHelper::CalculateGlobalTransform(FbxAMatrix lParentGX, FbxNode* Node)
	{
		FbxAMatrix lTranlationM, lScalingM, lScalingPivotM, lScalingOffsetM, lRotationOffsetM, lRotationPivotM, \
			lPreRotationM, lRotationM, lPostRotationM, lTransform;
		FbxAMatrix lGlobalT, lGlobalRS;
		
		if (!Node)
		{
			lTransform.SetIdentity();
			return lTransform;
		}
		
		// Construct translation matrix
		FbxVector4 lTranslation = Node->LclTranslation.Get();
		lTranlationM.SetT(lTranslation);

		// Construct rotation matrices
		FbxVector4 lRotation = Node->LclRotation.Get();
		lRotationM.SetR(lRotation);

		FbxVector4 lPreRotation = Node->PreRotation.Get();
		lPreRotationM.SetR(lPreRotation);

		FbxVector4 lPostRotation = Node->PostRotation.Get();
		lPostRotationM.SetR(lPostRotation);

		// Construct scaling matrix
		FbxVector4 lScaling = Node->LclScaling.Get();
		lScalingM.SetS(lScaling);


		// Construct offset and pivot matrices
		FbxVector4 lScalingOffset = Node->ScalingOffset.Get();
		lScalingOffsetM.SetT(lScalingOffset);

		FbxVector4 lScalingPivot = Node->ScalingPivot.Get();
		lScalingPivotM.SetT(lScalingPivot);

		FbxVector4 lRotationOffset = Node->RotationOffset.Get();
		lRotationOffsetM.SetT(lRotationOffset);

		FbxVector4 lRotationPivot = Node->RotationPivot.Get();
		lRotationPivotM.SetT(lRotationPivot);

		//// Calculate the global transform matrix of the parent node
		FbxNode* lParentNode = Node->GetParent();

		//Construct Global Rotation
		FbxAMatrix lParentGRM;
		FbxVector4 lParentGR = lParentGX.GetR();
		lParentGRM.SetR(lParentGR);
		FbxAMatrix lLRM = lPreRotationM * lRotationM * lPostRotationM;

		FbxAMatrix lParentTM;
		FbxVector4 lParentGT = lParentGX.GetT();
		lParentTM.SetT(lParentGT);
		FbxAMatrix lParentGRSM = lParentTM.Inverse() * lParentGX;
		FbxAMatrix lParentGSM = lParentGRM.Inverse() * lParentGRSM;
		FbxAMatrix lLSM = lScalingM;
	    
		FbxTransform::EInheritType lInheritType = Node->InheritType.Get();
		if (lInheritType == FbxTransform::eInheritRrSs)
		{
			lGlobalRS = lParentGRM * lLRM * lParentGSM * lLSM;
		}
		else if (lInheritType == FbxTransform::eInheritRSrs)
		{
			lGlobalRS = lParentGRM * lParentGSM * lLRM * lLSM;
		}
		else if (lInheritType == FbxTransform::eInheritRrs)
		{
			FbxAMatrix lParentLSM;

			if (lParentNode && lParentNode->LclScaling.IsValid())
			{
				FbxVector4 lParentLS = lParentNode->LclScaling.Get();
				lParentLSM.SetS(lParentLS);

			}

			FbxAMatrix lParentGSM_noLocal = lParentGSM * lParentLSM.Inverse();
			lGlobalRS = lParentGRM * lLRM * lParentGSM_noLocal * lLSM;
		}

		lTransform = lTranlationM * lRotationOffsetM * lRotationPivotM * lPreRotationM * lRotationM * lPostRotationM * lRotationPivotM.Inverse()\
			* lScalingOffsetM * lScalingPivotM * lScalingM * lScalingPivotM.Inverse();
		FbxVector4 lLocalTWithAllPivotAndOffsetInfo = lTransform.GetT();

		FbxVector4 lGlobalTranslation = lParentGX.MultT(lLocalTWithAllPivotAndOffsetInfo);
		lGlobalT.SetT(lLocalTWithAllPivotAndOffsetInfo);

		lTransform = lGlobalT * lGlobalRS;

		return lTransform;
}

void FBXHelper::ComputeTotalMatrix(bool bTransformVertexToAbsolute, FbxNode* Node, FbxAMatrix& ParentGlobalMatrix, FbxAMatrix &OutGlobalTransformMatrix, FbxAMatrix& OutTotalMatrix, FbxAMatrix& OutTotalMatrixForNormal)
{
	// Compute the node's local transformation matrix
	FbxAMatrix Geometry;
	FbxVector4 Translation = Node->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 Rotation = Node->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 Scaling = Node->GetGeometricScaling(FbxNode::eSourcePivot);
	Geometry.SetT(Translation);
	Geometry.SetR(Rotation);
	Geometry.SetS(Scaling);

	// Compute the node's global transformation matrix
	OutGlobalTransformMatrix = ParentGlobalMatrix * Node->EvaluateLocalTransform();
    
	// If bTransformVertexToAbsolute is false, then account for pivot geometry
	if (!bTransformVertexToAbsolute)
	{
		FbxAMatrix PivotGeometry;
		FbxVector4 RotationPivot = Node->GetRotationPivot(FbxNode::eSourcePivot);
		FbxVector4 FullPivot;
		FullPivot[0] = -RotationPivot[0];
		FullPivot[1] = -RotationPivot[1];
		FullPivot[2] = -RotationPivot[2];
		PivotGeometry.SetT(FullPivot);
		Geometry = Geometry * PivotGeometry;
	}

	// Compute the total matrix
	OutTotalMatrix = OutGlobalTransformMatrix * Geometry;

	// Compute the inverse transpose of the total matrix for normal transformation
	OutTotalMatrixForNormal = OutTotalMatrix.Inverse();
	OutTotalMatrixForNormal = OutTotalMatrixForNormal.Transpose();
}

void FBXHelper::ConvertScene(FbxScene* Scene, FVector CurrentFBXAxis)
{
	if (Scene)
	{
		
		FbxAxisSystem::ECoordSystem CoordSystem = CurrentFBXAxis.X == 0 ? FbxAxisSystem::eRightHanded : FbxAxisSystem::eLeftHanded;
		FbxAxisSystem::EUpVector UpVector = FbxAxisSystem::EUpVector();

		switch (int upaxis = CurrentFBXAxis.Z)
		{
			case 0:
				UpVector = FbxAxisSystem::eXAxis;
				break;
			case 1:
				UpVector = FbxAxisSystem::eYAxis;
				break;

			case 2:
				UpVector = FbxAxisSystem::eZAxis;
				break;
			default: ;
		};

		FbxAxisSystem::EFrontVector FrontVector = FbxAxisSystem::EFrontVector();

		 switch (int yaxis = CurrentFBXAxis.Y)
		 {
		 	case 0:
		 		FrontVector = static_cast<FbxAxisSystem::EFrontVector>(FbxAxisSystem::eXAxis);
		 		break;
		 	case 1:
		 		FrontVector = static_cast<FbxAxisSystem::EFrontVector>(-FbxAxisSystem::eYAxis);
		 		break;
		 	case 2:
		 		FrontVector = static_cast<FbxAxisSystem::EFrontVector>(FbxAxisSystem::eZAxis);
		 		break;
		 	default: ;
		 };
	
		// Define the Unreal Engine coordinate system (Z-up, Y-forward, X-right)
		FbxAxisSystem UnrealEngineAxisSystem(UpVector, FrontVector, CoordSystem);

		// Convert the scene to the Unreal Engine coordinate system
		FbxAxisSystem SceneAxisSystem = Scene->GetGlobalSettings().GetAxisSystem();
		if (SceneAxisSystem != UnrealEngineAxisSystem)
		{
			UnrealEngineAxisSystem.ConvertScene(Scene);
		}

		// Set the units and time mode (this part remains the same as in your code)
		Scene->GetGlobalSettings().SetSystemUnit(FbxSystemUnit::cm);
		Scene->GetGlobalSettings().SetTimeMode(FbxTime::eDefaultMode);
		
		Scene->GetAnimationEvaluator()->Reset();
	
	}
}

FVector FBXHelper::ConvertPos(FbxVector4 Vector, FVector Scale)
{
	FVector Out;
	Out[0] = Vector[0];
	Out[1] = -Vector[1];
	Out[2] = Vector[2];

	Out *= Scale;
	return FVector(-Out.Y, Out.X, Out.Z);
}

FVector FBXHelper::ConvertDir(FbxVector4 Vector)
{
	FVector Out;
	Out[0] = Vector[0];
	Out[1] = -Vector[1];
	Out[2] = Vector[2];
	return Out;
}