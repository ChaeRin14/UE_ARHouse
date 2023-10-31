// Copyright 2019-2023, Athian Games. All Rights Reserved. 

#pragma once

#include "Styling/SlateColor.h"
#include "Styling/SlateStyle.h"

#include "MeshNodeStyle.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct RUNTIMEFBXIMPORT_API FMeshNodeStyle : public FSlateWidgetStyle
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Text)
	FTextBlockStyle TextStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	UTexture2D* BackgroundTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	FSlateColor RowColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	FSlateBrush RowBackgroundBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	FMargin RowPadding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	float IndentPerDepth;

	FMeshNodeStyle()
		: BackgroundTexture(nullptr),
		  RowColor(FLinearColor(0.5, 0.5, 0.5, 0.1)),
		  IndentPerDepth(6)
	{
		TextStyle.ColorAndOpacity = FLinearColor(1,1,1,1);
		TextStyle.ShadowOffset = FVector2D(1,1);
		TextStyle.ShadowColorAndOpacity = FLinearColor(0.1,0.1,0.1,0.1);
		TextStyle.SetFontSize(18);
	}

	FSlateColor GetSlateTextColor() const;

	FSlateBrush GetBackgroundBrush() const;

	static void Initialize();
	static void Shutdown();

	static TSharedPtr< class FSlateStyleSet > Get();

private:
	static TSharedPtr< class FSlateStyleSet > StyleSet;
};
