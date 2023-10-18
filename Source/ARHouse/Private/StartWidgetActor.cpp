


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
		// ȭ�鿡 ������ ���� �ִ��� AR ���� Ʈ���̽��� �˻��Ѵ�.
		TArray<FARTraceResult> hitInfos = UARBlueprintLibrary::LineTraceTrackedObjects(FVector2D(2560 / 2, 1440 / 2), false, true, false, false);

		if (hitInfos.Num() > 0)
		{
			// �ε������Ͱ� ���ٸ� �����Ѵ�.
			if (spawnedIndicator == nullptr)
			{
				if (widget_BP)
				{
					widget_inst = (CreateWidget<UUserWidget>(GetWorld(), widget_BP));
					if (widget_inst)
					{
						// ������ ȭ�鿡 �߰�
						widget_inst->AddToViewport();
					}
				}
				spawnedIndicator = GetWorld()->SpawnActor<AActor>(indicator, hitInfos[0].GetLocalToWorldTransform());
			}
			// �ε������Ͱ� �ִٸ� Location ���� �����Ѵ�. 
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
				//����, �ε������� Hidden
				widget_inst->SetVisibility(ESlateVisibility::Hidden);
				spawnedIndicator->SetActorHiddenInGame(true);
			}
		}

}


