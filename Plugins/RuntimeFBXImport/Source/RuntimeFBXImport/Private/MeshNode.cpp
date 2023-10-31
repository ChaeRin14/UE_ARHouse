// Copyright 2019-2023, Athian Games. All Rights Reserved. 


#include "MeshNode.h"

void UMeshNode::AddChild(UMeshNode* Child)
{
	Children.Add(Child);
	Child->Parent = this;
}
