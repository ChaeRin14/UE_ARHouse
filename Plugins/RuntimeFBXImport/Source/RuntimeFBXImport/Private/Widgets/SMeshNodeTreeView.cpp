// Copyright 2019-2023, Athian Games. All Rights Reserved. 


#include "Widgets/SMeshNodeTreeView.h"
#include "SlateOptMacros.h"
#include "Widgets/SOverlay.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SMeshNodeTreeView::Construct(const FArguments& InArgs)
{
  	ItemsSource = InArgs._ItemsSource;
	Style = InArgs._Style;
    HoveredStyle = InArgs._HoveredStyle;
    SelectedStyle = InArgs._SelectedStyle;
    OnGenerateRow = InArgs._OnGenerateRow;
    
	this->ChildSlot
	[
		SAssignNew(VerticalBox, SVerticalBox)
	];

}

void SMeshNodeTreeView::FetchChildren(TSharedPtr<FMeshNodeData> InItem, TArray<TSharedPtr<FMeshNodeData>>& OutChildren)
{
    OutChildren = InItem->Children;
}

void SMeshNodeTreeView::PopulateTreeItemsWithDepth(UMeshNode* MeshNode, int32 Depth, TSharedPtr<FMeshNodeData> ParentNodeData)
{
    TSharedPtr<FMeshNodeData> TreeNode = MakeShared<FMeshNodeData>(MeshNode, Depth);

    // Initialize the node's expansion state to collapsed
    ExpansionStates.Add(TreeNode, false);

    // If this is a root node
    if (!ParentNodeData.IsValid())
    {
        RootNodes.Add(TreeNode);
    }
    else
    {
        ParentNodeData->Children.Add(TreeNode);
    }

    TArray<UMeshNode*> Children = MeshNode->GetChildren();
    for (UMeshNode* Child : Children)
    {
        PopulateTreeItemsWithDepth(Child, Depth + 1, TreeNode);
    }
}



void SMeshNodeTreeView::SetRootNodes(const TArray<UMeshNode*>& InRootNodes)
{
    RootNodes.Reset();

    // For each UMeshNode in InRootNodes, create a new FMeshNodeData and add it to RootNodes
    for (UMeshNode* RootNode : InRootNodes)
    {
        PopulateTreeItemsWithDepth(RootNode, 0, nullptr);
    }

    // Now that we have populated RootNodes, request a refresh to update the UI
    RequestTreeRefresh();
}


void SMeshNodeTreeView::RequestTreeRefresh()
{
    // Clear existing widgets
    VerticalBox->ClearChildren();

    // Add root nodes to the tree view
    for (const TSharedPtr<FMeshNodeData>& Node : RootNodes)
    {
        VerticalBox->AddSlot()
          .AutoHeight()
          [
              SNew(SBorder)
              .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
              .BorderBackgroundColor(FLinearColor(1, 1, 1, 0)) // Half-transparent white
              .Padding(0)
              [
                  GenerateRowWidget(Node)
              ]
          ];
    }
}




const FSlateBrush* SMeshNodeTreeView::GetExpanderArrowImage(TSharedPtr<FMeshNodeData> InItem) const
{
    if (ExpansionStates.Contains(InItem) && ExpansionStates[InItem])
    {
        return FCoreStyle::Get().GetBrush("TreeArrow_Expanded");
    }
    
    return FCoreStyle::Get().GetBrush("TreeArrow_Collapsed");
}

const FSlateBrush* SMeshNodeTreeView::GetRowBackgroundBrush(TSharedPtr<FMeshNodeData, ESPMode::ThreadSafe> InItem) const
{
    if (InItem == SelectedItem)
    {
       return &SelectedStyle.RowBackgroundBrush;
    }
    if (InItem == HoveredItem)
    {
        return &HoveredStyle.RowBackgroundBrush;
    }
    
    return &Style.RowBackgroundBrush;
}

FSlateColor SMeshNodeTreeView::GetRowBackgroundColor(TSharedPtr<FMeshNodeData> InItem) const
{
    if (InItem == SelectedItem)
    {
        // If this item is selected, use the selected row color
        return SelectedStyle.RowColor;
    }
    else if (InItem == HoveredItem)
    {
        // If this item is hovered, use the hovered row color
        return HoveredStyle.RowColor;
    }
    else
    {
        // Otherwise, use the default row color
        return Style.RowColor;
    }
}

FSlateColor SMeshNodeTreeView::GetRowTextColor(TSharedPtr<FMeshNodeData> InItem) const
{
    if (InItem == SelectedItem)
    {
        // If this item is selected, use the selected text color
        return SelectedStyle.TextStyle.ColorAndOpacity;
    }
    else if (InItem == HoveredItem)
    {
        // If this item is hovered, use the hovered text color
        return HoveredStyle.TextStyle.ColorAndOpacity;
    }
    else
    {
        // Otherwise, use the default text color
        return Style.TextStyle.ColorAndOpacity;
    }
}

FReply SMeshNodeTreeView::HandleRowClicked(TSharedPtr<FMeshNodeData> InItem)
{
    SelectedItem = InItem;
    return FReply::Handled();
}

TSharedRef<SWidget> SMeshNodeTreeView::GenerateRowWidget(TSharedPtr<FMeshNodeData> InItem)
{
    
    TSharedPtr<SWidget> RowWidget;
    if (OnGenerateRow.IsBound())
    {
        RowWidget = OnGenerateRow.Execute(InItem);
    }
    else
    {
        RowWidget = SNew(STextBlock)
            .ColorAndOpacity(this, &SMeshNodeTreeView::GetRowTextColor, InItem)
            .Text(FText::FromString(InItem->MeshNode->NodeName))
            .TextStyle(&Style.TextStyle);
    }
    
    // Fetch children nodes
    TArray<TSharedPtr<FMeshNodeData>> Children;
    FetchChildren(InItem, Children);

    float Indentation =  InItem->Depth * Style.IndentPerDepth;

    TSharedRef<SWidget> Widget =
        SNew(SHorizontalBox)
        // Add the padding to the text block and arrow only if the node has children
        + SHorizontalBox::Slot()
        .Padding(FMargin(Indentation, 0, 0, 0)) 
        .AutoWidth()
            [
                SNew(SButton)
                .ButtonStyle(FCoreStyle::Get(), "NoBorder")
                .Visibility(this, &SMeshNodeTreeView::GetExpanderArrowVisibility, InItem)
                .ContentPadding(0)
                .OnClicked(this, &SMeshNodeTreeView::HandleExpanderArrowClicked, InItem)
                .Content()
                [
                    SNew(SImage).Image(this, &SMeshNodeTreeView::GetExpanderArrowImage, InItem)
                ]
            ]
            + SHorizontalBox::Slot()
            .Padding(2, 0)
            .VAlign(VAlign_Center)
            [
                RowWidget.ToSharedRef()
            ];

    // Create a vertical box to hold the child nodes
    TSharedPtr<SVerticalBox> ChildrenContainer = SNew(SVerticalBox);

    // Add children to the children container
    if (ExpansionStates.Contains(InItem) && ExpansionStates[InItem])
    {
        for (const TSharedPtr<FMeshNodeData>& Child : Children)
        {
            ChildrenContainer->AddSlot()
            .AutoHeight()
            .Padding(2, 0)
            [
                GenerateRowWidget(Child)
            ];
        }
    }

    // Return the button with the row widget and the children container
    return 
        SNew(SButton)
        .ButtonStyle(FCoreStyle::Get(), "NoBorder") // Transparent button
        .OnClicked(this, &SMeshNodeTreeView::HandleRowClicked, InItem) // Handle click events
        .OnHovered_Lambda([this, InItem]() { HoveredItem = InItem; }) // Update hovered item on hover
        .OnUnhovered_Lambda([this, InItem]() { if (HoveredItem == InItem) { HoveredItem.Reset(); } }) // Clear hovered item on unhover
        [
                SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SBorder)
                   .BorderImage(this, &SMeshNodeTreeView::GetRowBackgroundBrush, InItem)  // Set background color based on state
                   .BorderBackgroundColor(this, &SMeshNodeTreeView::GetRowBackgroundColor, InItem)
                   .Padding(2)
                   [
                        Widget   
                   ]
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    // Add the children container here
                    ChildrenContainer.ToSharedRef()
                ]
        ];
}

FReply SMeshNodeTreeView::HandleRowMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, TSharedPtr<FMeshNodeData> InItem)
{
    // Update the hovered item
    HoveredItem = InItem;
    return FReply::Handled();
}



void SMeshNodeTreeView::CollapseChildren(TSharedPtr<FMeshNodeData> InItem)
{
    // Fetch children nodes
    TArray<TSharedPtr<FMeshNodeData>> Children;
    FetchChildren(InItem, Children);
    
    for (const TSharedPtr<FMeshNodeData>& Child : Children)
    {
        // Collapse child
        ExpansionStates[Child] = false;
        
        // Collapse children of child
        CollapseChildren(Child);
    }
}

FReply SMeshNodeTreeView::HandleExpanderArrowClicked(TSharedPtr<FMeshNodeData> InItem)
{
    if(ExpansionStates.Contains(InItem))
    {
        // Toggle the expansion state of the item
        bool bIsExpanded =  ExpansionStates[InItem];
        ExpansionStates[InItem] = !bIsExpanded;

        // If it was expanded, collapse it
        if(bIsExpanded) 
        {
            CollapseChildren(InItem);
        }
    }

    // Refresh the tree to apply the new expansion state
    RequestTreeRefresh();

    return FReply::Handled();
}

EVisibility SMeshNodeTreeView::GetExpanderArrowVisibility(TSharedPtr<FMeshNodeData> InItem) const
{
    // If this item has no children, don't show the expander arrow
    if (InItem->Children.Num() == 0)
    {
        return EVisibility::Hidden;
    }

    return EVisibility::Visible;
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION
