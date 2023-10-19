// Copyright 2019-2023, Athian Games. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RuntimeMeshFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class RUNTIMEFBXIMPORT_API URuntimeMeshFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	static void CreateImageWrapperModule();

public:
	
	static UWorld* GetGameWorld();
	static FTextureData LoadTextureDatafromByteArray(TArray<uint8> RawFileData);

	UFUNCTION(BlueprintCallable, Category = "FBX")
	static bool OpenFBXFileDialogue(FString& OutFilePath);
	
	UFUNCTION(BlueprintCallable, Category = "Texture")
	static FTextureData LoadTextureDatafromPath(FString FilePath, TArray<uint8>& RawFileData);

};
