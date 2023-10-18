// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ARHouseGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ARHOUSE_API AARHouseGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, Category=MySettings)
	TSubclassOf<class UMainWidget> httpWidget;

	void SetLogText(const FString msg);
	void SetImageTexture(class UTexture2D* tex);


private:
	class UMainWidget* httpUI;
	
	
};
