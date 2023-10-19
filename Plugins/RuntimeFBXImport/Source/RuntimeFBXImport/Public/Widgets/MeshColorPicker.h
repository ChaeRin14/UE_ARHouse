// Copyright 2019-2023, Athian Games. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Widgets/Layout/SBorder.h"
#include "MeshColorPicker.generated.h"

/**
 * 
 */
UCLASS()
class RUNTIMEFBXIMPORT_API UMeshColorPicker : public UWidget
{
	GENERATED_BODY()
	TSharedPtr<SBorder> MyColorPickerBorder;

	UMeshColorPicker();
	
protected:

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void OnBindingChanged(const FName& Property) override;

	virtual void SynchronizeProperties() override;

	void HandleOnValueChanged(FLinearColor newcolor);
	void HandleOnEyeDropperPickBegin();
	void HandleOnEyeDropperPickEnd();
	FLinearColor OnGetColor() const;

	
public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLinearColorChanged, const FLinearColor&, NewColor);

	
	/** Background color of the color picker. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color")
	FLinearColor BorderColorAndOpacity;

	/** A delegate being called when the Color value is changed. */
	UPROPERTY(BlueprintAssignable, Category = "ColorPicker")
	FOnLinearColorChanged OnColorValueChanged;

	/**
	* Set Initial Color value of the Color Picker.
	* @param	InitialColor  The color to set.
	*/
	UFUNCTION(BlueprintCallable, Category = "ColorPicker")
	void SetColorPicker(FLinearColor InitialColor);

	
#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

};

