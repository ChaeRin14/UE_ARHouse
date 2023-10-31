// Copyright 2019-2023, Athian Games. All Rights Reserved. 


#include "Widgets/MeshColorBlock.h"

#define LOCTEXT_NAMESPACE "ColorBlock"

UMeshColorBlock::UMeshColorBlock()
{
	CurrentColor = FLinearColor(1, 1, 1, 1);
}

FLinearColor UMeshColorBlock::GetColor() const
{
	return CurrentColor;
}

void UMeshColorBlock::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyColorBlock.Reset();
}


TSharedRef<SWidget> UMeshColorBlock::RebuildWidget()
{
	auto OnGetColor = [&]()
	{
		return CurrentColor;
	};

	MyColorBlock = SNew(SColorBlock)
		.ShowBackgroundForAlpha(false).Color_Lambda(OnGetColor)
		.AlphaDisplayMode(EColorBlockAlphaDisplayMode::Ignore)
		.ColorIsHSV(false)
		.Size(FVector2D(70.0f, 12.0f));


	return MyColorBlock.ToSharedRef();

}

#if WITH_EDITOR

const FText UMeshColorBlock::GetPaletteCategory()
{
	return LOCTEXT("ColoPicker", "ColoPicker");
}

#endif

#undef LOCTEXT_NAMESPACE

