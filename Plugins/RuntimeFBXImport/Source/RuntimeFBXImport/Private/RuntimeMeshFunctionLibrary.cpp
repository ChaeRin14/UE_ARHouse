// Copyright 2019-2023, Athian Games. All Rights Reserved. 


#include "RuntimeMeshFunctionLibrary.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "MeshNode.h"
#include "Misc/FileHelper.h"
#include "Engine/Engine.h"

#if PLATFORM_WINDOWS
#include "Windows/MinWindows.h"
#include <commdlg.h>
#include <shellapi.h>
#endif

UWorld* URuntimeMeshFunctionLibrary::GetGameWorld()
{
	if (UGameViewportClient* Viewport = GEngine->GameViewport)
	{
		FWorldContext* worldcontext = GEngine->GetWorldContextFromGameViewport(Viewport);
		UWorld* world = worldcontext->World();
		return world;
	}

	return nullptr;
}

bool URuntimeMeshFunctionLibrary::OpenFBXFileDialogue(FString& OutFilePath)
{
#if PLATFORM_WINDOWS
	
	// Set up the file dialogue
	OPENFILENAME ofn;
	TCHAR szFile[260] = { 0 };
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = _T("FBX Files (*.fbx)\0*.fbx\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Show the open file dialogue. If the result is nonzero, a file was selected.
	if (GetOpenFileName(&ofn) == TRUE)
	{
		OutFilePath = FString(ofn.lpstrFile);
		return true;
	}
	
#elif PLATFORM_LINUX
	// Using Zenity for Linux (a common program for creating simple dialogues)
	// Requires Zenity to be installed on the system
	FILE* pipe = popen("zenity --file-selection", "r");
	if (pipe)
	{
		char buffer[128];
		while (!feof(pipe))
		{
			if (fgets(buffer, 128, pipe) != nullptr)
			{
				filePath += buffer;
			}
		}
		pclose(pipe);
		return true;
	}
#elif PLATFORM_MAC
	// Using Cocoa for Mac, compiled with Objective-C++
	// Requires the source file to be .mm instead of .cpp
	// The @autoreleasepool is necessary to avoid leaking memory, as suggested by Cocoa conventions
	@autoreleasepool {
		NSOpenPanel* panel = [NSOpenPanel openPanel];
		[panel setCanChooseFiles:YES];
		[panel setCanChooseDirectories:NO];

		if ([panel runModal] == NSOKButton)
		{
			NSArray* urls = [panel URLs];
			for (NSURL* url in urls)
			{
				filePath = [url path];
				break; // Only using the first selected file
			}
			return true;
		}
	}
#else
#error "File dialogues are not supported on this platform."
#endif
   return false;

}

IImageWrapperModule* ImageWrapperModule = nullptr;

void URuntimeMeshFunctionLibrary::CreateImageWrapperModule()
{
	ImageWrapperModule = &FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
}

FTextureData URuntimeMeshFunctionLibrary::LoadTextureDatafromByteArray(TArray<uint8> RawFileData)
{
	FTextureData TextureData = FTextureData();
	if (!ImageWrapperModule)
	{
		CreateImageWrapperModule();
	}

	if (ImageWrapperModule)
	{
		EImageFormat ImageFormat = ImageWrapperModule->DetectImageFormat(RawFileData.GetData(), RawFileData.Num());

		if (ImageFormat == EImageFormat::Invalid || ImageFormat == EImageFormat::EXR)
		{
			return TextureData;
		}

		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule->CreateImageWrapper(ImageFormat);

		if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
		{
			TArray<uint8> UncompressedBGRA;
			if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
			{
				TextureData.Width = ImageWrapper->GetWidth();
				TextureData.Height = ImageWrapper->GetHeight();
				TextureData.UnCompressedData = UncompressedBGRA;
			}
		}
	}
	
	return TextureData;
}

FTextureData URuntimeMeshFunctionLibrary::LoadTextureDatafromPath(FString FilePath, TArray<uint8>& RawFileData)
{
	FTextureData TextureData = FTextureData();
	if (FFileHelper::LoadFileToArray(RawFileData, *FilePath))
	{
		TextureData = LoadTextureDatafromByteArray(RawFileData);
	}
	
	return TextureData;
}

