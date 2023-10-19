// Copyright 2019-2023, Athian Games. All Rights Reserved. 


#include "DataAssets/MaterialLibraryAsset.h"

FMaterialGroup UMaterialLibraryAsset::GetMaterialGroup(FString TypeName)
{
	int32 Index = MaterialGroups.IndexOfByPredicate([&](const FMaterialGroup& Group)
	  {
		  return Group.TypeName.Equals(TypeName);
	  });

	if(Index != INDEX_NONE)
	{
		return MaterialGroups[Index];
	}
	
	return FMaterialGroup(); // return an empty group if none was found
}

FMeshTextureGroup UMaterialLibraryAsset::GetTextureGroup(FString TypeName)
{
	int32 Index = TextureGroups.IndexOfByPredicate([&](const FMeshTextureGroup& Group)
	{
		return Group.TypeName.Equals(TypeName);
	});

	if(Index != INDEX_NONE)
	{
		return TextureGroups[Index];
	}

	return FMeshTextureGroup(); // return an empty group if none was found
}
