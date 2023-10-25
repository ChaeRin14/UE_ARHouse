// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpRequestActor.generated.h"

UCLASS()
class ARHOUSE_API AHttpRequestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHttpRequestActor();

	// url을 요청할 수 있는 함수
	void SendRequest(const FString url);
	// 서버로 보내는 함수
	void PostRequest(const FString url);
	// 제이슨 파일로 저장하기
	void SaveJson(const FString jsonData);
	// 이미지 가져오기
	void GetImage(const FString url);
	// 이미지 저장하기
	void SaveImage(const UTexture2D* tex);
	// 이미지 보내기
	void PostImage_Png(const FString& url, const UTexture2D* tex);
	void PostImage_Jpg(const FString url, const UTexture2D* tex);
	// FBX 파일 받기
	void GetFBXFile(const FString& url, const FString& savePath);

	UPROPERTY(BlueprintReadWrite)
	FString saveFBXurl = "D:\\ARHouse\\UE_ARHouse\\Saved\\PersistentDownloadDir";

	UPROPERTY(BlueprintReadWrite)
	bool isFBXLoad = false;
	
private:
	// 간혹가다 안 먹힐 때가 있다 안 먹히면 원래 방식으로 하면 됨
	//void OnReceivedData(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bConnectedSuccessfully);

	void OnReceivedData(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	void OnPostData(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	void OnGetImageData(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	void OnPostImageData(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	void FBXFileDownload(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	UPROPERTY()
	class AARHouseGameModeBase* gm;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
