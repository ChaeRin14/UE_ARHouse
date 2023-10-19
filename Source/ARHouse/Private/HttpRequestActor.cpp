// Fill out your copyright notice in the Description page of Project Settings.


#include "HttpRequestActor.h"
#include "HttpModule.h"
#include "JsonParseLibrary.h"
#include <ImageUtils.h>
#include <DesktopPlatform/Public/IDesktopPlatform.h>
#include <DesktopPlatform/Public/DesktopPlatformModule.h>
#include "../ARHouseGameModeBase.h"
#include "MainWidget.h"



// 통신용 클래스
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

	// 모듈 생성
	FHttpModule& httpModule = FHttpModule::Get();
	// 모듈을 생성하고 리퀘스트 인스턴스를 생성한다.
	TSharedPtr<IHttpRequest> req = httpModule.CreateRequest();

	// 요청하기 위한 정보를 설정한다
	req->SetURL(url);
	// 지금은 값을 가져오려고 GET을 적은 거임
	req->SetVerb("GET");
	// 내가 가져오려는 데이터의 종류와 타입
	// 콘텐츠 타입과 제이슨 형식으로 받아올 거다
	// 만약 이미지를 가져오려면 이미지로 적고 확장자 명을 적는다
	// req->SetHeader(TEXT("Content-Type"), TEXT("image/png"));
	req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// 요청이 완료되었을 때 실행될 함수를 바인딩한다

	// 두 가지의 방법이 있음 BindUFunction을 써서 직접 입력하기
	// 아니면 BindUObject로 함수 불러와서 우리 하던 방식으로 바인딩하기
	// req->OnProcessRequestComplete().BindUFunction(this, FName("OnReceivedData"));
	req->OnProcessRequestComplete().BindUObject(this, &AHttpRequestActor::OnReceivedData);

	// 서버에 요청을 보낸다
	req->ProcessRequest();
}


//  bool bConnectedSuccessfully : 성공했는지 안 했는지에 대한 확인 방법
void AHttpRequestActor::OnReceivedData(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	UE_LOG(LogTemp, Warning, TEXT("OnReceivedData"));
	// 만약 성공적으로 보내졌다면
	if (bConnectedSuccessfully)
	{
		// 문자열로 받기 
		FString res = Response->GetContentAsString();
		// 입력받은 값을 분해해서 parsedData에 넣어준다
		FString parsedData = UJsonParseLibrary::JsonParse(res);
		// res 데이터를 넘겨준다
		gm->SetLogText(parsedData);
	}
	else
	{
		gm->SetLogText("Failed");
	}
}


// POST 메소드 요청하기
void AHttpRequestActor::PostRequest(const FString url)
{
	TMap<FString, FString> studentData;
	studentData.Add("ImagePath", "AAA");

	// 제이슨 형태로 바꿔서 넣어주겠다
	FString myJsonData = UJsonParseLibrary::MakeJson(studentData);
	gm->SetLogText(myJsonData);

	// 요청 설정
	FHttpModule& httpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest> req = httpModule.CreateRequest();
	// url을 보내겠다
	req->SetURL(url);
	req->SetVerb(TEXT("POST"));
	req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	req->SetContentAsString(myJsonData);
	req->OnProcessRequestComplete().BindUObject(this, &AHttpRequestActor::OnPostData);
	req->ProcessRequest();

}


// 포스트 요청 함수
void AHttpRequestActor::OnPostData(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		FString receivedData = Response->GetContentAsString();
		// 받은 데이터를 화면에 출력
		gm->SetLogText(receivedData);
		// 받은 데이터를 파일로 저장
		SaveJson(receivedData);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NotStarted"));
		// 요청 전송 상태 확인
		EHttpRequestStatus::Type status = Request->GetStatus();
		switch (status)
		{
			// 시작이 안 된 경우
		case EHttpRequestStatus::NotStarted:
			break;
			// 아직 리퀘스트 하려고 체크 중인 경우
		case EHttpRequestStatus::Processing:
			UE_LOG(LogTemp, Warning, TEXT("Processing"));
			break;
			// 에러가 난 경우
		case EHttpRequestStatus::Failed:
			UE_LOG(LogTemp, Warning, TEXT("Failed"));
			break;
			// 인터넷 연결에 에러가 난 경우
		case EHttpRequestStatus::Failed_ConnectionError:
			UE_LOG(LogTemp, Warning, TEXT("Failed_ConnectionError"));
			break;
			// 성공한 경우
		case EHttpRequestStatus::Succeeded:
			UE_LOG(LogTemp, Warning, TEXT("Succeeded"));
			break;
		default:
			break;
		}

		// 응답 코드 확인
		int32 responseCode = Response->GetResponseCode();
		gm->SetLogText(FString::Printf(TEXT("Response Code: %d"), responseCode));
	}
}



// 제이슨 파일로 저장하기
void AHttpRequestActor::SaveJson(const FString jsonData)
{
	// 만일 지정된 이름의 폴더가 없으면 해당 폴더를 만든다
	FPlatformFileManager& fileManager = FPlatformFileManager::Get();
	IPlatformFile& platformFile = fileManager.GetPlatformFile();

	FString dirPath = FPaths::ProjectConfigDir() + "JsonData";
	if (!platformFile.DirectoryExists(*dirPath))
	{
		platformFile.CreateDirectory(*dirPath);
	}

	// 제이슨 데이터를 파일로 저장한다
	FString fullPath = dirPath + "MyJson.json";
	UE_LOG(LogTemp, Warning, TEXT("save Path: %s"), *fullPath);
	bool bIsSaved = FFileHelper::SaveStringToFile(jsonData, *fullPath);
	gm->SetLogText(FString::Printf(TEXT("%s"), bIsSaved ? *FString("Json Saved Successfully!") : *FString("Failed saveing file")));
}

// 이미지 요청 함수
void AHttpRequestActor::GetImage(const FString url)
{
	TSharedRef<IHttpRequest> req = FHttpModule::Get().CreateRequest();
	req->SetURL(url);
	req->SetVerb("GET");
	req->SetHeader(TEXT("Content-Type"), TEXT("image/jpeg"));
	req->OnProcessRequestComplete().BindUObject(this, &AHttpRequestActor::OnGetImageData);
	req->ProcessRequest();


}

// 이미지 가져오기
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


// 이미지 저장
void AHttpRequestActor::SaveImage(const UTexture2D* tex)
{

}

// 텍스처 포스트 함수
void AHttpRequestActor::PostImage(const FString url, const UTexture2D* tex)
{
	// 텍스처의 각 픽셀 컬러 정보를 배열에 담는다
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

		// 이미지 바이트 배열을 압축한다
		TArray<uint8> compressedImage;
		FImageUtils::CompressImageArray(width, height, imgArr, compressedImage);

		// 이미지 바이트 배열을 포스트 한다
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


