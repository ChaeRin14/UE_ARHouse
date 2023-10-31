// Copyright 2019-2023, Athian Games. All Rights Reserved. 


#include "DataAssets/MaterialImportSettings.h"

bool UMaterialImportSettings::ResolveTexturePath(FString MeshFilePath, FString& TexturePath)
{
	// Get the filename from the incorrect path
	FString Filename = FPaths::GetCleanFilename(TexturePath);
	FString MeshFolder = FPaths::GetPath(MeshFilePath);
	FString BaseFileName = FPaths::GetBaseFilename(MeshFilePath);
	
	for (const FTexturePathMapping& Mapping : TexturePathMappings)
	{
		FString PossiblePath = FPaths::Combine(Mapping.RelativePath , Filename);
		if(Mapping.AbsolutePath.IsEmpty())
		{
			// Convert to absolute path
			PossiblePath = FPaths::Combine(MeshFolder, PossiblePath);
		}
		else
		{
			PossiblePath = FPaths::Combine(Mapping.AbsolutePath , PossiblePath);
		}
		
		// Check if the texture exists at this path
		if (FPaths::FileExists(PossiblePath))
		{
			// If the texture exists at this path, update the TexturePath and return true
			TexturePath = PossiblePath;
			return true;
		}
	}

	if (FPaths::FileExists(TexturePath))
	{
		return true;
	}
	
	FString DefaultPath = FPaths::Combine(MeshFolder, BaseFileName + ".fbm",  Filename);
	if (FPaths::FileExists(DefaultPath))
	{
		TexturePath = DefaultPath;
		return true;
	}
	
	// If no mapping was found, check the original path
	FString OriginalPath = FPaths::ConvertRelativePathToFull(TexturePath);
	if (FPaths::FileExists(OriginalPath))
	{
		TexturePath = OriginalPath;
		return true;
	}

	// Check in the same folder as the FBX file
	FString PossiblePath = MeshFolder / Filename;
	if (FPaths::FileExists(PossiblePath))
	{
		TexturePath = PossiblePath;
		return true;
	}

	// If texture was not found in any path, return false
	return false;
}
