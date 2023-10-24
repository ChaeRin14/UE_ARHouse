#include "MainWidget.h"
#include "HttpRequestActor.h"
#include "Components/Button.h"
#include "DesktopPlatform/Public/IDesktopPlatform.h"
#include "DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Engine/Texture2D.h"
#include "Modules/ModuleManager.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include <ImageUtils.h>

void UMainWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 이미지 업로드
    btn_fileImage->OnClicked.AddDynamic(this, &UMainWidget::FileOn);
}

void UMainWidget::FileOn()
{
    // 파일 탐색기 열기
    FString DirectoryPath;
    FString Directory;
    bool IsSelect;

    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (!DesktopPlatform)
    {
        // Desktop Platform 모듈을 가져올 수 없는 경우 처리
        IsSelect = false;
        return;
    }

    FString DefaultPath = DirectoryPath;

    TArray<FString> OutFiles;  // 선택한 파일 경로가 여기에 들어옴

    bool bOpened = DesktopPlatform->OpenFileDialog(
        nullptr,
        TEXT("Select File"),      // 탐색기 제목
        DefaultPath,               // 열고싶은 탐색기 경로
        TEXT(""),                 // 기본 필터 (모든 파일 허용)
        TEXT("Image Files (*.png;*.jpg)|*.png;*.jpg"),  // png 및 jpeg 파일 필터
        EFileDialogFlags::None,
        OutFiles              // 선택한 파일 경로가 여기에 들어옴
    );

    if (bOpened && OutFiles.Num() > 0)
    {
        SourceFilePath = OutFiles[0];

        // 이 경로에 저장하기
        DestinationFilePath = FPaths::Combine(TEXT("D:\\ARHouse\\UE_ARHouse\\Saved\\PersistentDownloadDir"), FPaths::GetCleanFilename(SourceFilePath));

        TArray<uint8> FileData;

        // 파일 저장
        if (FFileHelper::LoadFileToArray(FileData, *SourceFilePath))
        {
            bool bSaved = FFileHelper::SaveArrayToFile(FileData, *DestinationFilePath);

            if (bSaved)
            {
                IsSelect = true;

                UE_LOG(LogTemp, Warning, TEXT("File saved to: %s"), *DestinationFilePath);
                btn_fileImage->SetVisibility(ESlateVisibility::Hidden);

                PostImageRequest();
            }
            else
            {
                IsSelect = false;
                DestinationFilePath.Empty();  // 저장 실패 시 대상 디렉토리 비우기 (선택하지 않은 것으로 처리)
                FileData.Empty();  // 메모리 해제

                UE_LOG(LogTemp, Error, TEXT("Failed to save file."));
            }

            FileData.Empty();  // 메모리 해제
        }
        else
        {
            FileData.Empty();  // 메모리 해제

            DestinationFilePath.Empty();
            btn_fileImage->SetVisibility(ESlateVisibility::Visible);

            UE_LOG(LogTemp, Error, TEXT("Failed to load file: %s"), *SourceFilePath);
        }
    }
    else
    {
        IsSelect = false;
    }

}

// 이미지 저장
void UMainWidget::PostImageRequest()
{
    // 보낼 파일 데이터 배열
    TArray<uint8> FileData;

    if (FFileHelper::LoadFileToArray(FileData, *DestinationFilePath))
    {
        FString FileExtension = FPaths::GetExtension(DestinationFilePath);

        UTexture2D* LoadedTexture = nullptr;

        // 확장자가 png라면
        if (FileExtension.Equals("png", ESearchCase::IgnoreCase))
        {
            // 바이트 배열 데이터를 텍스처로 전환한다
            LoadedTexture = FImageUtils::ImportBufferAsTexture2D(FileData);
        }
        else
        {
            // 아니면 파일 경로에서 직접 텍스처를 가져온다
            LoadedTexture = FImageUtils::ImportFileAsTexture2D(DestinationFilePath);
        }
        // 텍스처가 없을 때 찍히는 로그
        if (LoadedTexture == nullptr)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to load image file: %s"), *DestinationFilePath);
            return;
        }

        // 텍스처 사용 예시: 로그 출력
        UE_LOG(LogTemp, Warning, TEXT("Loaded texture dimensions: %dx%d"), LoadedTexture->GetSizeX(), LoadedTexture->GetSizeY());

        // png이면 png 포스트 함수로 이동
        if (FileExtension.Equals("png", ESearchCase::IgnoreCase))
        {

            // 이미지 보내는 함수로 이동
            httpReqActor->PostImage_Png(baseURL, LoadedTexture);
            UE_LOG(LogTemp, Warning, TEXT("png : %s"), *baseURL);
        }
        // jpg이면 jpg 포스트 함수로 이동
        else
        {
            // 이미지 보내는 함수로 이동
            httpReqActor->PostImage_Jpg(baseURL, LoadedTexture);
            UE_LOG(LogTemp, Warning, TEXT("jpg : %s"), *baseURL);

            save =  httpReqActor->GetActorLocation();
        }

        // 필요한 작업 수행 후 텍스처 사용이 끝나면 제거해야 함 == 메모리 누수 방지
        LoadedTexture->RemoveFromRoot();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load file data: %s"), *DestinationFilePath);
    }
}

