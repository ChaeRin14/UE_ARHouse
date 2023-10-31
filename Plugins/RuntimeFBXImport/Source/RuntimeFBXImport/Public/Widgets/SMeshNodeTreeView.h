// Copyright 2019-2023, Athian Games. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "MeshNode.h"
#include "Widgets/MeshNodeStyle.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"

struct FMeshNodeData
{
	UMeshNode* MeshNode;
	int32 Depth;
	TArray<TSharedPtr<FMeshNodeData>> Children;
	TSharedPtr<FMeshNodeData> Parent;

	FMeshNodeData(UMeshNode* InNode, int32 InDepth)
		: MeshNode(InNode), Depth(InDepth)
	{
	}
};

DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SWidget>, FOnGenerateRow, TSharedPtr<FMeshNodeData> Item);

class SMeshNodeTreeView : public SCompoundWidget
{
public:
	
    SLATE_BEGIN_ARGS(SMeshNodeTreeView) {}
        SLATE_ARGUMENT(TArray<TSharedPtr<FMeshNodeData>>, ItemsSource)
        SLATE_ARGUMENT(FMeshNodeStyle, Style)
		SLATE_ARGUMENT(FMeshNodeStyle, HoveredStyle)
		SLATE_ARGUMENT(FMeshNodeStyle, SelectedStyle)
	    SLATE_EVENT(FOnGenerateRow, OnGenerateRow)
    SLATE_END_ARGS()


    void Construct(const FArguments& InArgs);
	void SetRootNodes(const TArray<UMeshNode*>& NewRootNodes);
	void RequestTreeRefresh();


    const FSlateBrush* GetRowBackgroundBrush(TSharedPtr<FMeshNodeData, ESPMode::ThreadSafe> InItem) const;
    TSharedRef<SWidget> GenerateRowWidget(TSharedPtr<FMeshNodeData> InItem);
    void FetchChildren(TSharedPtr<FMeshNodeData> InItem, TArray<TSharedPtr<FMeshNodeData>>& OutChildren);
	FReply HandleExpanderArrowClicked(TSharedPtr<FMeshNodeData> InItem);
	EVisibility GetExpanderArrowVisibility(TSharedPtr<FMeshNodeData> InItem) const;
	const FSlateBrush* GetExpanderArrowImage(TSharedPtr<FMeshNodeData> InItem) const;
	void PopulateTreeItemsWithDepth(UMeshNode* MeshNode, int32 Depth, TSharedPtr<FMeshNodeData> ParentNodeData);
	void CollapseChildren(TSharedPtr<FMeshNodeData> InItem);

	FSlateColor GetRowBackgroundColor(TSharedPtr<FMeshNodeData> InItem) const;
	FSlateColor GetRowTextColor(TSharedPtr<FMeshNodeData> InItem) const;
	FReply HandleRowClicked(TSharedPtr<FMeshNodeData> InItem);
	FReply HandleRowMouseMove(const FGeometry& Geometry, const FPointerEvent& Event, TSharedPtr<FMeshNodeData> InItem);

private:

	FOnGenerateRow OnGenerateRow;
	
    TArray<TSharedPtr<FMeshNodeData>> ItemsSource;
    FMeshNodeStyle Style;
	FMeshNodeStyle HoveredStyle;
	FMeshNodeStyle SelectedStyle;
	
    TMap<TSharedPtr<FMeshNodeData>, bool> ExpansionStates;  // Store expansion state of each node
	TArray<TSharedPtr<FMeshNodeData>> RootNodes;
	TSharedPtr<SVerticalBox> VerticalBox;

	TMap<TSharedPtr<FMeshNodeData>, TSharedPtr<SExpandableArea>> ExpandableAreas;

	TSharedPtr<FMeshNodeData> SelectedItem;
	TSharedPtr<FMeshNodeData> HoveredItem;
};

