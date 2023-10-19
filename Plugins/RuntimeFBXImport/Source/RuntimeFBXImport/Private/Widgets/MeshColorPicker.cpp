// Copyright 2019-2023, Athian Games. All Rights Reserved. 


#include "Widgets/MeshColorPicker.h"
#include "Widgets/Colors/SColorPicker.h"

#define LOCTEXT_NAMESPACE "UMG"

UMeshColorPicker::UMeshColorPicker()
{
	BorderColorAndOpacity = FLinearColor(0.3 , 0.3 , 0.3 , 0.8);
}

void UMeshColorPicker::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyColorPickerBorder.Reset();
	
}

FLinearColor UMeshColorPicker::OnGetColor() const
{
	return BorderColorAndOpacity;
}

void UMeshColorPicker::OnBindingChanged(const FName& property)
{
	Super::OnBindingChanged(property);

}

TSharedRef<SWidget> UMeshColorPicker::RebuildWidget()
{
	MyColorPickerBorder = SNew(SBorder);
	
	return MyColorPickerBorder.ToSharedRef();
}

void UMeshColorPicker::SetColorPicker(FLinearColor InitialColor)
{
	TSharedPtr<SColorPicker> MyColorPicker = SNew(SColorPicker)
		.TargetColorAttribute(InitialColor)
		.UseAlpha(true)
		.OnlyRefreshOnMouseUp(false).OnInteractivePickBegin(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnEyeDropperPickBegin))
		.OnInteractivePickEnd(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnEyeDropperPickEnd))
		.OnlyRefreshOnOk(false)
		.ExpandAdvancedSection(true)
		.OnColorCommitted(BIND_UOBJECT_DELEGATE(FOnLinearColorValueChanged, HandleOnValueChanged));

	if (MyColorPickerBorder.IsValid())
	{
		MyColorPickerBorder->SetContent(MyColorPicker.ToSharedRef());
	}
}


void UMeshColorPicker::HandleOnValueChanged(FLinearColor newcolor)
{
	SynchronizeProperties();
	OnColorValueChanged.Broadcast(newcolor);
	
}


void UMeshColorPicker::HandleOnEyeDropperPickBegin()
{
}

void UMeshColorPicker::HandleOnEyeDropperPickEnd()
{

}


void UMeshColorPicker::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	MyColorPickerBorder->SetBorderBackgroundColor(BorderColorAndOpacity);
}

#if WITH_EDITOR

const FText UMeshColorPicker::GetPaletteCategory()
{
	return LOCTEXT("ColoPicker", "ColoPicker");
}

#endif


#undef LOCTEXT_NAMESPACE
