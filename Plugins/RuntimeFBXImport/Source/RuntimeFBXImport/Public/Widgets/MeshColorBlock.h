// Copyright 2019-2023, Athian Games. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Widgets/Colors/SColorBlock.h"
#include "MeshColorBlock.generated.h"

/**
 * 
 */
UCLASS()
class RUNTIMEFBXIMPORT_API UMeshColorBlock : public UWidget
{
	GENERATED_BODY()

	TSharedPtr<SColorBlock> MyColorBlock;

	UMeshColorBlock();

	FLinearColor GetColor() const;

protected:

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	
	virtual TSharedRef<SWidget> RebuildWidget() override;

public:

	/** Current color of the color block. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color")
	FLinearColor CurrentColor;


#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

	
};
