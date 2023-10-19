// Copyright 2019-2023, Athian Games. All Rights Reserved. 


#include "Widgets/MeshNodeView.h"
#include "MeshNode.h"
#include "Blueprint/UserWidget.h"

#define LOCTEXT_NAMESPACE "MeshNodeView"

void UMeshNodeView::SetRootNodes(const TArray<UMeshNode*>& InRootNodes)
{
	// Start with a fresh tree
	TreeItems.Empty();

	if (TreeViewWidget.IsValid())
	{
		TreeViewWidget->SetRootNodes(InRootNodes);
	}
}


void UMeshNodeView::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	// Release the Slate TreeView widget
	TreeViewWidget.Reset();
}

TSharedRef<SWidget> UMeshNodeView::RebuildWidget()
{
		
	TreeViewWidget = SNew(SMeshNodeTreeView).Style(MeshNodeStyle).HoveredStyle(MeshNodeStyleHovered).SelectedStyle(MeshNodeStyleSelected)
	.OnGenerateRow_UObject(this, &UMeshNodeView::OnGenerateRow);
	return TreeViewWidget.ToSharedRef();
}


TSharedRef<SWidget> UMeshNodeView::OnGenerateRow(TSharedPtr<FMeshNodeData> Item)
{
	TSharedPtr<SWidget> RowContent;

	if(RowWidgetClass != nullptr)
	{
		UWorld* World = GetWorld();
		if(World != nullptr)
		{
			UUserWidget* RowWidget = CreateWidget<UUserWidget>(World, RowWidgetClass);

			if(RowWidget != nullptr)
			{
				// Trigger the OnRowGenerated event
				OnRowGenerated.Broadcast(RowWidget, Item->MeshNode);

				// Get the Slate Widget from the UserWidget and assign it to RowContent
				RowContent = RowWidget->TakeWidget();
			}
		}
	}
    
	// If RowContent is not set, create a default TextBlock widget
	if (!RowContent.IsValid())
	{
		RowContent = SNew(STextBlock).Text(FText::FromString(Item->MeshNode->NodeName));
	}

	return RowContent.ToSharedRef();
}

void UMeshNodeView::OnGetChildren(TSharedPtr<FMeshNodeData> Item, TArray<TSharedPtr<FMeshNodeData>>& OutChildren)
{
	OutChildren = Item->Children;
}


#if WITH_EDITOR

const FText UMeshNodeView::GetPaletteCategory()
{
	return LOCTEXT("Views", "Views");
}

#endif

#undef LOCTEXT_NAMESPACE