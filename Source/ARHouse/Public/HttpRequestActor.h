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

	// url�� ��û�� �� �ִ� �Լ�
	void SendRequest(const FString url);
	// ������ ������ �Լ�
	void PostRequest(const FString url);
	// ���̽� ���Ϸ� �����ϱ�
	void SaveJson(const FString jsonData);
	// �̹��� ��������
	void GetImage(const FString url);
	// �̹��� �����ϱ�
	void SaveImage(const UTexture2D* tex);
	// �̹��� ������
	void PostImage_Png(const FString& url, const UTexture2D* tex);
	void PostImage_Jpg(const FString url, const UTexture2D* tex);
	// FBX ���� �ޱ�
	void GetFBXFile(const FString& url, const FString& savePath);

	UPROPERTY(BlueprintReadWrite)
	FString saveFBXurl = "D:\\ARHouse\\UE_ARHouse\\Saved\\PersistentDownloadDir";

	UPROPERTY(BlueprintReadWrite)
	bool isFBXLoad = false;
	
private:
	// ��Ȥ���� �� ���� ���� �ִ� �� ������ ���� ������� �ϸ� ��
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
