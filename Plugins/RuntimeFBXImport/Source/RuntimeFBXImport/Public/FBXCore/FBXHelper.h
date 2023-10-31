// Copyright 2019-2023, Athian Games. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include <fbxsdk.h>
#include "DataAssets/MaterialImportSettings.h"

/**
 * 
 */
class RUNTIMEFBXIMPORT_API FBXHelper
{
	
public:

	
	static FbxFileTexture* FindTexture(FbxProperty Property);

	static FbxAMatrix CalculateGlobalTransform(FbxAMatrix lParentGX, FbxNode* Node);
	
	static void ComputeTotalMatrix(bool bTransformVertexToAbsolute, FbxNode* Node, FbxAMatrix& ParentGlobalMatrix, FbxAMatrix& OutGlobalTransformMatrix, FbxAMatrix& OutTotalMatrix, FbxAMatrix
		& OutTotalMatrixForNormal);
	static void ConvertScene(FbxScene* Scene, FVector CurrentFBXAxis);
	static FVector ConvertPos(FbxVector4 Vector, FVector Scale);
	static FVector ConvertDir(FbxVector4 Vector);

	static FString GetFBXTexturePropertyName(EMeshTextureType Key);
	static FString GetFBXColorPropertyName(EMeshColorType Key);
	static FString GetFBXScalarPropertyName(EMeshScalarType Key);

};
