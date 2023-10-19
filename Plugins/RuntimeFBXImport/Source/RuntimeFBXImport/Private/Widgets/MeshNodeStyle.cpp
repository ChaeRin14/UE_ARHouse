// Copyright 2019-2023, Athian Games. All Rights Reserved. 


#include "Widgets/MeshNodeStyle.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"


TSharedPtr< FSlateStyleSet > FMeshNodeStyle::StyleSet = NULL;

FSlateColor FMeshNodeStyle::GetSlateTextColor() const
{
	return FSlateColor(TextStyle.ColorAndOpacity);
}

FSlateBrush FMeshNodeStyle::GetBackgroundBrush() const
{
	FSlateBrush Brush;
	if (BackgroundTexture)
	{
		Brush.SetResourceObject(BackgroundTexture);
	}
	return Brush;
}

void FMeshNodeStyle::Initialize()
{
	if (!StyleSet.IsValid())
	{
		StyleSet = MakeShareable(new FSlateStyleSet("MeshNodeStyle"));
		FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
        
		// define your style here
		StyleSet->Set("CustomTableRow", FTableRowStyle()
			.SetEvenRowBackgroundBrush(FSlateNoResource()) // Empty brush (transparent background)
			// Set other brushes as needed
		);
		// StyleSet->Set("CustomExpanderArrow", FTableRowStyle()
		// 	.SetExpanderArrowStyle(FButtonStyle()
		// 		.SetNormal(FSlateBrush()) // Set your brush here
		// 		// Set other brushes as needed
		// 	)
		// );
	}
}

void FMeshNodeStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
	ensure(StyleSet.IsUnique());
	StyleSet.Reset();
}

TSharedPtr< class FSlateStyleSet > FMeshNodeStyle::Get()
{
	return StyleSet;
}

