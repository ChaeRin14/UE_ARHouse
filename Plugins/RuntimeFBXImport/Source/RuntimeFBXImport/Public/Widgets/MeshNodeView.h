// Copyright 2019-2023, Athian Games. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Widgets/SMeshNodeTreeView.h"
#include "MeshNodeView.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRowGeneratedEvent, UUserWidget*, RowWidget, UMeshNode*, NodeData);

/**
 * 
 */
UCLASS()
class  UMeshNodeView : public UWidget
{
	GENERATED_BODY()

private:

	TSharedPtr<class SMeshNodeTreeView> TreeViewWidget;
	TArray<TSharedPtr<FMeshNodeData>> TreeItems;
	TArray<TSharedPtr<FMeshNodeData>> RootData;

public:
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> RowWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Styles")
	FMeshNodeStyle MeshNodeStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Styles")
	FMeshNodeStyle MeshNodeStyleHovered;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Styles")
	FMeshNodeStyle MeshNodeStyleSelected;
	
	UFUNCTION(BlueprintCallable, Category = "Mesh Node")
	void SetRootNodes(const TArray<UMeshNode*>& InRootNodes);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRowGeneratedEvent OnRowGenerated;
	
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	TSharedRef<SWidget> OnGenerateRow(TSharedPtr<FMeshNodeData> Item);
	void OnGetChildren(TSharedPtr<FMeshNodeData> Item, TArray<TSharedPtr<FMeshNodeData>>& OutChildren);


#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif


};

