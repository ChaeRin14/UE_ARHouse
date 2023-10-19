// Fill out your copyright notice in the Description page of Project Settings.


#include "HttpRequestActor.h"
#include "HttpModule.h"
#include "JsonParseLibrary.h"
#include <ImageUtils.h>
#include <DesktopPlatform/Public/IDesktopPlatform.h>
#include <DesktopPlatform/Public/DesktopPlatformModule.h>
#include "../ARHouseGameModeBase.h"
#include "MainWidget.h"



// ��ſ� Ŭ����
AHttpRequestActor::AHttpRequestActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHttpRequestActor::BeginPlay()
{
	Super::BeginPlay();

	gm = GetWorld()->GetAuthGameMode<AARHouseGameModeBase>();

}

// Called every frame
void AHttpRequestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHttpRequestActor::SendRequest(const FString url)
{

	// ��� ����
	FHttpModule& httpModule = FHttpModule::Get();
	// ����� �����ϰ� ������Ʈ �ν��Ͻ��� �����Ѵ�.
	TSharedPtr<IHttpRequest> req = httpModule.CreateRequest();

	// ��û�ϱ� ���� ������ �����Ѵ�
	req->SetURL(url);
	// ������ ���� ���������� GET�� ���� ����
	req->SetVerb("GET");
	// ���� ���������� �������� ������ Ÿ��
	// ������ Ÿ�԰� ���̽� �������� �޾ƿ� �Ŵ�
	// ���� �̹����� ���������� �̹����� ���� Ȯ���� ���� ���´�
	// req->SetHeader(TEXT("Content-Type"), TEXT("image/png"));
	req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// ��û�� �Ϸ�Ǿ��� �� ����� �Լ��� ���ε��Ѵ�

	// �� ������ ����� ���� BindUFunction�� �Ἥ ���� �Է��ϱ�
	// �ƴϸ� BindUObject�� �Լ� �ҷ��ͼ� �츮 �ϴ� ������� ���ε��ϱ�
	// req->OnProcessRequestComplete().BindUFunction(this, FName("OnReceivedData"));
	req->OnProcessRequestComplete().BindUObject(this, &AHttpRequestActor::OnReceivedData);

	// ������ ��û�� ������
	req->ProcessRequest();
}


//  bool bConnectedSuccessfully : �����ߴ��� �� �ߴ����� ���� Ȯ�� ���
void AHttpRequestActor::OnReceivedData(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	UE_LOG(LogTemp, Warning, TEXT("OnReceivedData"));
	// ���� ���������� �������ٸ�
	if (bConnectedSuccessfully)
	{
		// ���ڿ��� �ޱ� 
		FString res = Response->GetContentAsString();
		// �Է¹��� ���� �����ؼ� parsedData�� �־��ش�
		FString parsedData = UJsonParseLibrary::JsonParse(res);
		// res �����͸� �Ѱ��ش�
		gm->SetLogText(parsedData);
	}
	else
	{
		gm->SetLogText("Failed");
	}
}


// POST �޼ҵ� ��û�ϱ�
void AHttpRequestActor::PostRequest(const FString url)
{
	TMap<FString, FString> studentData;
	studentData.Add("ImagePath", "AAA");

	// ���̽� ���·� �ٲ㼭 �־��ְڴ�
	FString myJsonData = UJsonParseLibrary::MakeJson(studentData);
	gm->SetLogText(myJsonData);

	// ��û ����
	FHttpModule& httpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest> req = httpModule.CreateRequest();
	// url�� �����ڴ�
	req->SetURL(url);
	req->SetVerb(TEXT("POST"));
	req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	req->SetContentAsString(myJsonData);
	req->OnProcessRequestComplete().BindUObject(this, &AHttpRequestActor::OnPostData);
	req->ProcessRequest();

}


// ����Ʈ ��û �Լ�
void AHttpRequestActor::OnPostData(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		FString receivedData = Response->GetContentAsString();
		// ���� �����͸� ȭ�鿡 ���
		gm->SetLogText(receivedData);
		// ���� �����͸� ���Ϸ� ����
		SaveJson(receivedData);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NotStarted"));
		// ��û ���� ���� Ȯ��
		EHttpRequestStatus::Type status = Request->GetStatus();
		switch (status)
		{
			// ������ �� �� ���
		case EHttpRequestStatus::NotStarted:
			break;
			// ���� ������Ʈ �Ϸ��� üũ ���� ���
		case EHttpRequestStatus::Processing:
			UE_LOG(LogTemp, Warning, TEXT("Processing"));
			break;
			// ������ �� ���
		case EHttpRequestStatus::Failed:
			UE_LOG(LogTemp, Warning, TEXT("Failed"));
			break;
			// ���ͳ� ���ῡ ������ �� ���
		case EHttpRequestStatus::Failed_ConnectionError:
			UE_LOG(LogTemp, Warning, TEXT("Failed_ConnectionError"));
			break;
			// ������ ���
		case EHttpRequestStatus::Succeeded:
			UE_LOG(LogTemp, Warning, TEXT("Succeeded"));
			break;
		default:
			break;
		}

		// ���� �ڵ� Ȯ��
		int32 responseCode = Response->GetResponseCode();
		gm->SetLogText(FString::Printf(TEXT("Response Code: %d"), responseCode));
	}
}



// ���̽� ���Ϸ� �����ϱ�
void AHttpRequestActor::SaveJson(const FString jsonData)
{
	// ���� ������ �̸��� ������ ������ �ش� ������ �����
	FPlatformFileManager& fileManager = FPlatformFileManager::Get();
	IPlatformFile& platformFile = fileManager.GetPlatformFile();

	FString dirPath = FPaths::ProjectConfigDir() + "JsonData";
	if (!platformFile.DirectoryExists(*dirPath))
	{
		platformFile.CreateDirectory(*dirPath);
	}

	// ���̽� �����͸� ���Ϸ� �����Ѵ�
	FString fullPath = dirPath + "MyJson.json";
	UE_LOG(LogTemp, Warning, TEXT("save Path: %s"), *fullPath);
	bool bIsSaved = FFileHelper::SaveStringToFile(jsonData, *fullPath);
	gm->SetLogText(FString::Printf(TEXT("%s"), bIsSaved ? *FString("Json Saved Successfully!") : *FString("Failed saveing file")));
}

// �̹��� ��û �Լ�
void AHttpRequestActor::GetImage(const FString url)
{
	TSharedRef<IHttpRequest> req = FHttpModule::Get().CreateRequest();
	req->SetURL(url);
	req->SetVerb("GET");
	req->SetHeader(TEXT("Content-Type"), TEXT("image/jpeg"));
	req->OnProcessRequestComplete().BindUObject(this, &AHttpRequestActor::OnGetImageData);
	req->ProcessRequest();


}

// �̹��� ��������
void AHttpRequestActor::OnGetImageData(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		TArray<uint8> texBites = Response->GetContent();
		FString imagePath = FPaths::ProjectPersistentDownloadDir() + "/MyGetImage.jpg";
		FFileHelper::SaveArrayToFile(texBites, *imagePath);
		UTexture2D* realTex = FImageUtils::ImportBufferAsTexture2D(texBites);
		gm->SetImageTexture(realTex);
	}
	else
	{
		gm->SetLogText(FString::Printf(TEXT("failed \nResponse Code: %d"), Response->GetResponseCode()));
	}
}


// �̹��� ����
void AHttpRequestActor::SaveImage(const UTexture2D* tex)
{

}

// �ؽ�ó ����Ʈ �Լ�
void AHttpRequestActor::PostImage(const FString url, const UTexture2D* tex)
{
	// �ؽ�ó�� �� �ȼ� �÷� ������ �迭�� ��´�
	FTexture2DMipMap mipData = tex->GetPlatformData()->Mips[0];

	TArray<FColor> imgArr;
	int32 width = mipData.SizeX;
	int32 height = mipData.SizeY;
	imgArr.AddUninitialized(width * height);

	void* pixelData = mipData.BulkData.Lock(LOCK_READ_ONLY);

	if (pixelData != nullptr)
	{
		FMemory::Memcpy(imgArr.GetData(), pixelData, imgArr.Num() * sizeof(FColor));
		mipData.BulkData.Unlock();

		// �̹��� ����Ʈ �迭�� �����Ѵ�
		TArray<uint8> compressedImage;
		FImageUtils::CompressImageArray(width, height, imgArr, compressedImage);

		// �̹��� ����Ʈ �迭�� ����Ʈ �Ѵ�
		TSharedRef<IHttpRequest> req = FHttpModule::Get().CreateRequest();
		req->SetURL(url);
		req->SetVerb("POST");
		req->SetHeader(TEXT("Content-Type"), TEXT("image/jpeg"));
		req->SetContent(compressedImage);
		req->OnProcessRequestComplete().BindUObject(this, &AHttpRequestActor::OnPostImageData);
		req->ProcessRequest();
	}
}

void AHttpRequestActor::OnPostImageData(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), bConnectedSuccessfully ? *Response->GetContentAsString() : *FString::Printf(TEXT("Response code: %d"), Response->GetResponseCode()));

}


