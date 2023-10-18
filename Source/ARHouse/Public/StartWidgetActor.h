

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StartWidgetActor.generated.h"

UCLASS()
class ARHOUSE_API AStartWidgetActor : public AActor
{
	GENERATED_BODY()
	
public:	

	AStartWidgetActor();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = MySettings)
	TSubclassOf<AActor> indicator;

	UPROPERTY(EditAnywhere, Category = MySettings)
	TSubclassOf<UUserWidget> widget_BP; 

private:
	AActor* spawnedIndicator;
	UUserWidget* widget_inst;

	void SetIndicator();
};
