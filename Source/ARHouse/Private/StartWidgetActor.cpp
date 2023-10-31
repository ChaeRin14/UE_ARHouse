


#include "StartWidgetActor.h"
#include "Widget_CreateRoom.h"
#include "ARBlueprintLibrary.h"
#include "Widget_HowCanI.h"
#include "Components/TextBlock.h"
#include <Sound/SoundCue.h>
#include <Kismet/GameplayStatics.h>


AStartWidgetActor::AStartWidgetActor()
{
 	
	PrimaryActorTick.bCanEverTick = true;

}


void AStartWidgetActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (widget_How)
	{
		widget_Howinst = (CreateWidget<UUserWidget>(GetWorld(), widget_How));
		if(widget_Howinst)
		{
			widget_Howinst->AddToViewport();
		}
	}

	// 사운드 재생
	if (soundBGM)
	{
		UGameplayStatics::PlaySound2D(this, soundBGM, 1.0f, 1.0f, 0.0f);
	}
}


void AStartWidgetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//ShowPlaneOutLine();
	if(bisRemove == true)
	{
		SetIndicator();
	}

	
	
	
}

void AStartWidgetActor::SetIndicator()
{
		if (previewActor == nullptr) return;
		if (indicator == nullptr) return;
		// 화면에 추적된 면이 있는지 AR 라인 트레이스로 검색한다.
		TArray<FARTraceResult> hitInfos = UARBlueprintLibrary::LineTraceTrackedObjects(FVector2D(2560 / 2, 1440 / 2), false, true, false, false);
		UWidget_HowCanI* Widget_HowCanI = NewObject<UWidget_HowCanI>();

		if (hitInfos.Num() > 0)
		{
			// 인디케이터가 없다면 생성한다.
			if (spawnedIndicator == nullptr && spawnedpreviewActor == nullptr)
			{
				if (widget_BP)
				{
					widget_inst = (CreateWidget<UUserWidget>(GetWorld(), widget_BP));
					if (widget_inst && Widget_HowCanI)
					{
						// 위젯을 화면에 추가
						widget_Howinst->SetVisibility(ESlateVisibility::Hidden);
						widget_inst->AddToViewport();
					}
				}
				spawnedIndicator = GetWorld()->SpawnActor<AActor>(indicator, hitInfos[0].GetLocalToWorldTransform());
				spawnedIndicator->SetActorHiddenInGame(true);
				
				spawnedpreviewActor = GetWorld()->SpawnActor<AActor>(previewActor, hitInfos[0].GetLocalToWorldTransform());
			}
			// 인디케이터가 있다면 Location 값만 변경한다. 
			else
			{
				widget_inst->SetVisibility(ESlateVisibility::Visible);
				widget_Howinst->SetVisibility(ESlateVisibility::Hidden);
				spawnedIndicator->SetActorHiddenInGame(true);
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
				widget_Howinst->SetVisibility(ESlateVisibility::Visible);
			}
		}

}

void AStartWidgetActor::RemoveUI()
{
	if (widget_Howinst != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Remove!!"));
// 		bisRemove = false;
// 		widget_Howinst->SetVisibility(ESlateVisibility::Hidden);
	}
	
}

void AStartWidgetActor::Preview()
{
	/*if (previewActor == nullptr) return;*/
// 	// 화면에 추적된 면이 있는지 AR 라인 트레이스로 검색한다.
// 	TArray<FARTraceResult> hitInfos = UARBlueprintLibrary::LineTraceTrackedObjects(FVector2D(2560 / 2, 1440 / 2), false, true, false, false);
// 	if (hitInfos.Num() > 0)
// 	{
		// 인디케이터가 없다면 생성한다.
/*		if (spawnedpreviewActor == nullptr)*/
/*		{*/
// 			UE_LOG(LogTemp, Warning, TEXT("Spawn Preview"));
// 			spawnedpreviewActor = GetWorld()->SpawnActor<AActor>(previewActor, GetActorLocation() , GetActorRotation());
// 			UE_LOG(LogTemp, Warning, TEXT("Spawn Complete!"));
/*		}*/
/*	}*/
}

