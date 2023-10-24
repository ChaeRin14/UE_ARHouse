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

    // �̹��� ���ε�
    btn_fileImage->OnClicked.AddDynamic(this, &UMainWidget::FileOn);
}

void UMainWidget::FileOn()
{
    // ���� Ž���� ����
    FString DirectoryPath;
    FString Directory;
    bool IsSelect;

    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (!DesktopPlatform)
    {
        // Desktop Platform ����� ������ �� ���� ��� ó��
        IsSelect = false;
        return;
    }

    FString DefaultPath = DirectoryPath;

    TArray<FString> OutFiles;  // ������ ���� ��ΰ� ���⿡ ����

    bool bOpened = DesktopPlatform->OpenFileDialog(
        nullptr,
        TEXT("Select File"),      // Ž���� ����
        DefaultPath,               // ������� Ž���� ���
        TEXT(""),                 // �⺻ ���� (��� ���� ���)
        TEXT("Image Files (*.png;*.jpg)|*.png;*.jpg"),  // png �� jpeg ���� ����
        EFileDialogFlags::None,
        OutFiles              // ������ ���� ��ΰ� ���⿡ ����
    );

    if (bOpened && OutFiles.Num() > 0)
    {
        SourceFilePath = OutFiles[0];

        // �� ��ο� �����ϱ�
        DestinationFilePath = FPaths::Combine(TEXT("D:\\ARHouse\\UE_ARHouse\\Saved\\PersistentDownloadDir"), FPaths::GetCleanFilename(SourceFilePath));

        TArray<uint8> FileData;

        // ���� ����
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
                DestinationFilePath.Empty();  // ���� ���� �� ��� ���丮 ���� (�������� ���� ������ ó��)
                FileData.Empty();  // �޸� ����

                UE_LOG(LogTemp, Error, TEXT("Failed to save file."));
            }

            FileData.Empty();  // �޸� ����
        }
        else
        {
            FileData.Empty();  // �޸� ����

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

// �̹��� ����
void UMainWidget::PostImageRequest()
{
    // ���� ���� ������ �迭
    TArray<uint8> FileData;

    if (FFileHelper::LoadFileToArray(FileData, *DestinationFilePath))
    {
        FString FileExtension = FPaths::GetExtension(DestinationFilePath);

        UTexture2D* LoadedTexture = nullptr;

        // Ȯ���ڰ� png���
        if (FileExtension.Equals("png", ESearchCase::IgnoreCase))
        {
            // ����Ʈ �迭 �����͸� �ؽ�ó�� ��ȯ�Ѵ�
            LoadedTexture = FImageUtils::ImportBufferAsTexture2D(FileData);
        }
        else
        {
            // �ƴϸ� ���� ��ο��� ���� �ؽ�ó�� �����´�
            LoadedTexture = FImageUtils::ImportFileAsTexture2D(DestinationFilePath);
        }
        // �ؽ�ó�� ���� �� ������ �α�
        if (LoadedTexture == nullptr)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to load image file: %s"), *DestinationFilePath);
            return;
        }

        // �ؽ�ó ��� ����: �α� ���
        UE_LOG(LogTemp, Warning, TEXT("Loaded texture dimensions: %dx%d"), LoadedTexture->GetSizeX(), LoadedTexture->GetSizeY());

        // png�̸� png ����Ʈ �Լ��� �̵�
        if (FileExtension.Equals("png", ESearchCase::IgnoreCase))
        {

            // �̹��� ������ �Լ��� �̵�
            httpReqActor->PostImage_Png(baseURL, LoadedTexture);
            UE_LOG(LogTemp, Warning, TEXT("png : %s"), *baseURL);
        }
        // jpg�̸� jpg ����Ʈ �Լ��� �̵�
        else
        {
            // �̹��� ������ �Լ��� �̵�
            httpReqActor->PostImage_Jpg(baseURL, LoadedTexture);
            UE_LOG(LogTemp, Warning, TEXT("jpg : %s"), *baseURL);

            save =  httpReqActor->GetActorLocation();
        }

        // �ʿ��� �۾� ���� �� �ؽ�ó ����� ������ �����ؾ� �� == �޸� ���� ����
        LoadedTexture->RemoveFromRoot();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load file data: %s"), *DestinationFilePath);
    }
}

