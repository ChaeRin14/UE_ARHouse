

#include "Widget_CreateRoom.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "ARCharacter.h"
#include <Kismet/GameplayStatics.h>
#include "Components/Image.h"
#include "Components/Throbber.h"
#include "UObject/UObjectGlobals.h"
#include "Components/Progressbar.h"
#include "Bed.h"
#include "MapToolWidget.h"


void UWidget_CreateRoom::NativeConstruct()
{
	btn_MakeRoom->OnClicked.AddDynamic(this, &UWidget_CreateRoom::OnClickCreateButton);

	img_Loading->SetVisibility(ESlateVisibility::Hidden);
	text_bulid->SetVisibility(ESlateVisibility::Hidden);
	pro_Loading->SetVisibility(ESlateVisibility::Hidden);


}

void UWidget_CreateRoom::OnClickCreateButton()
{
	PlayAnimation(a_fade);
	img_Loading->SetVisibility(ESlateVisibility::Visible);
	text_bulid->SetVisibility(ESlateVisibility::Visible);
	btn_MakeRoom->SetVisibility(ESlateVisibility::Hidden);
	pro_Loading->SetVisibility(ESlateVisibility::Visible);
	PlayAnimation(a_Loading);
	FTimerHandle DelayHandle;
	GetWorld()->GetTimerManager().SetTimer(DelayHandle, FTimerDelegate::CreateLambda([&]()
		{
			AsyncLevelLoad("/Game/Map/", "ARHouse"); 
			isLevelLoad = true;
		}
	), 5, false);

	isObjSpawn = true;
}

void UWidget_CreateRoom::AsyncLevelLoad(const FString& levelPath, const FString& levelName)
{
	LoadPackageAsync(levelPath + levelName,
		FLoadPackageAsyncDelegate::CreateLambda([=](const FName& packageName, UPackage* loadPackage, EAsyncLoadingResult::Type result)
			{
				if (result == EAsyncLoadingResult::Succeeded)
				{
					OnFinishedLevelLoad(levelName);
				}
			}),
			0,
			PKG_ContainsMap);
}

void UWidget_CreateRoom::OnFinishedLevelLoad(const FString levelName)
{
	UGameplayStatics::OpenLevel(this, FName(*levelName));
}



