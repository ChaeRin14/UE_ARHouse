


#include "StartWidgetActor.h"
#include "Widget_CreateRoom.h"
#include "ARBlueprintLibrary.h"


AStartWidgetActor::AStartWidgetActor()
{
 	
	PrimaryActorTick.bCanEverTick = true;

}


void AStartWidgetActor::BeginPlay()
{
	Super::BeginPlay();
	
}


void AStartWidgetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//ShowPlaneOutLine();
	SetIndicator();
}

void AStartWidgetActor::SetIndicator()
{
		if (indicator == nullptr) return;
		// 화면에 추적된 면이 있는지 AR 라인 트레이스로 검색한다.
		TArray<FARTraceResult> hitInfos = UARBlueprintLibrary::LineTraceTrackedObjects(FVector2D(2560 / 2, 1440 / 2), false, true, false, false);

		if (hitInfos.Num() > 0)
		{
			// 인디케이터가 없다면 생성한다.
			if (spawnedIndicator == nullptr)
			{
				if (widget_BP)
				{
					widget_inst = (CreateWidget<UUserWidget>(GetWorld(), widget_BP));
					if (widget_inst)
					{
						// 위젯을 화면에 추가
						widget_inst->AddToViewport();
					}
				}
				spawnedIndicator = GetWorld()->SpawnActor<AActor>(indicator, hitInfos[0].GetLocalToWorldTransform());
			}
			// 인디케이터가 있다면 Location 값만 변경한다. 
			else
			{
				widget_inst->SetVisibility(ESlateVisibility::Visible);
				spawnedIndicator->SetActorHiddenInGame(false);
				spawnedIndicator->SetActorTransform(hitInfos[0].GetLocalToWorldTransform());
			}
		}
		else
		{
			if (spawnedIndicator != nullptr)
			{
				//위젯, 인디케이터 Hidden
				widget_inst->SetVisibility(ESlateVisibility::Hidden);
				spawnedIndicator->SetActorHiddenInGame(true);
			}
		}

}


