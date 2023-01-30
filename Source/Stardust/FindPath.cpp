// Fill out your copyright notice in the Description page of Project Settings.


#include "FindPath.h"
#include "Stardust/GameModes/GameFramework.h"
#include "Stardust/GameObjects/Connection.h"
#include "Stardust/GameObjects/GameActor.h"



FFindPath::FGraph::FSearchNode FFindPath::FGraph::GetNodeByRef(FNodeRef NodeRef)
{
	return FSearchNode(NodeRef);
}

int32 FFindPath::FGraph::GetNeighbourCount(FNodeRef NodeRef) const
{
	if (AGameFramework* GameMode = Cast<AGameFramework>(GameModePointer))
	{
		if (GameMode->IsValidGameActorIndex(NodeRef))
		{
			return GameMode->GetGameActorRef(NodeRef)->Connections.Num();
		}
	}

	return 0;
}

bool FFindPath::FGraph::IsValidRef(FNodeRef NodeRef) const
{
	if (AGameFramework* GameMode = Cast<AGameFramework>(GameModePointer))
	{
		return GameMode->IsValidGameActorIndex(NodeRef);
	}

	return false;
}

FFindPath::FGraph::FNodeRef FFindPath::FGraph::GetNeighbour(const FSearchNode& NodeRef, const int32 NeighbourIndex) const
{
	if (AGameFramework* GameMode = Cast<AGameFramework>(GameModePointer))
	{
		if (GameMode->IsValidGameActorIndex(NodeRef.NodeRef))
		{
			AGameActor* SearchedActor = GameMode->GetGameActorRef(NodeRef.NodeRef);

			if (SearchedActor->Connections.IsValidIndex(NeighbourIndex))
			{
				if (AGameActor* OtherActor = SearchedActor->Connections[NeighbourIndex]->GetOtherActor(SearchedActor))
				{
					int32 Index;
					GameMode->FindGameActor(OtherActor, Index);
					return Index;
				}
			}
		}
	}

	return -1;
}



float FFindPath::FQuery::GetHeuristicScale() const
{
	return 1.0f;
}

float FFindPath::FQuery::GetHeuristicCost(const FSearchNode& StartNode, const FSearchNode& EndNode) const
{
	if (AGameFramework* GameMode = Cast<AGameFramework>(GameModePointer))
	{
		if (GameMode->IsValidGameActorIndex(StartNode.NodeRef) && GameMode->IsValidGameActorIndex(EndNode.NodeRef))
		{
			return GameMode->GetGameActorRef(StartNode.NodeRef)->GetDistanceTo(GameMode->GetGameActorRef(EndNode.NodeRef));
		}
	}

	return -1;
}

float FFindPath::FQuery::GetTraversalCost(const FSearchNode& StartNode, const FSearchNode& EndNode) const
{
	return StartNode.TraversalCost;
}

bool FFindPath::FQuery::IsTraversalAllowed(const FNodeRef NodeA, const FNodeRef NodeB) const
{
	if (AGameFramework* GameMode = Cast<AGameFramework>(GameModePointer))
	{
		for (auto Connection : GameMode->GetGameActorRef(NodeA)->Connections)
		{
			if (GameMode->GetGameActorRef(NodeB)->Connections.Contains(Connection))
			{
				return true;
			}
		}
	}

	return false;
}

bool FFindPath::FQuery::WantsPartialSolution() const
{
	return false;
}

int32 FFindPath::FQuery::GetNeighbourCount(FNodeRef NodeRef) const
{
	if (AGameFramework* GameMode = Cast<AGameFramework>(GameModePointer))
	{
		if (GameMode->IsValidGameActorIndex(NodeRef))
		{
			return GameMode->GetGameActorRef(NodeRef)->Connections.Num();
		}
	}

	return 0;
}

bool FFindPath::FQuery::ShouldIgnoreClosedNodes() const
{
	return false;
}

bool FFindPath::FQuery::ShouldIncludeStartNodeInPath() const
{
	return false;
}



void FFindPath::AddNode(AGameActor* Node, int32 Index)
{
	Nodes.EmplaceAt(Index, Node);
}

void FFindPath::SetNodes(const TArray<AGameActor*>& InNodes)
{
	Nodes.Empty();
	Nodes.Append(InNodes);
}

EGraphAStarResult FFindPath::FindPath()
{
	FGraph Graph(GameModePtr);
	FQuery Query(GameModePtr);

	FGraphAStar<FGraph, FGraphAStarDefaultPolicy, FGraphNode> Pathfinding(Graph);

	if (Nodes.Num() > 0)
	{
		if (AGameFramework* GameMode = Cast<AGameFramework>(GameModePtr))
		{
			int32 Index;
			if (GameMode->FindGameActor(Nodes[0], Index))
			{
				FullPath.Add(Index);
			}

			for (int32 i = 1; i < Nodes.Num(); i++)
			{
				TArray<int32> Path;

				int32 StartIndex;
				int32 EndIndex;

				if (GameMode->FindGameActor(Nodes[i - 1], StartIndex) && GameMode->FindGameActor(Nodes[i], EndIndex))
				{
					EGraphAStarResult Result = Pathfinding.FindPath(Graph.GetNodeByRef(StartIndex), Graph.GetNodeByRef(EndIndex), Query, Path);
					if (Result != EGraphAStarResult::SearchSuccess)
					{
						return Result;
					}

					FullPath.Append(Path);
				}
				else
				{
					return EGraphAStarResult::SearchFail;
				}
			}

			return EGraphAStarResult::SearchSuccess;
		}
	}

	return EGraphAStarResult::SearchFail;
}

EGraphAStarResult FFindPath::FindPath(AGameActor* StartActor, AGameActor* EndActor)
{
	FGraph Graph(GameModePtr);
	FQuery Query(GameModePtr);

	FGraphAStar<FGraph, FGraphAStarDefaultPolicy, FGraphNode> Pathfinding(Graph);

	if (AGameFramework* GameMode = Cast<AGameFramework>(GameModePtr))
	{
		TArray<int32> Path;

		int32 StartIndex;
		int32 EndIndex;

		if (GameMode->FindGameActor(StartActor, StartIndex) && GameMode->FindGameActor(EndActor, EndIndex))
		{
			EGraphAStarResult Result = Pathfinding.FindPath(Graph.GetNodeByRef(StartIndex), Graph.GetNodeByRef(EndIndex), Query, Path);
			FullPath.Add(StartIndex);
			FullPath.Append(Path);
			return Result;
		}
	}

	return EGraphAStarResult::SearchFail;
}

void FFindPath::GetGameActors(TArray<AGameActor*>& OutActors)
{
	if (AGameFramework* GameMode = Cast<AGameFramework>(GameModePtr))
	{
		for (int32 i = 0; i < FullPath.Num(); i++)
		{
			if (GameMode->IsValidGameActorIndex(FullPath[i]))
			{
				OutActors.Add(GameMode->GetGameActorRef(FullPath[i]));
			}
		}
	}
}

void FFindPath::GetActorConnection(TArray<AConnection*>& OutConnections)
{
	if (FullPath.Num() == 0)
		return;

	if (AGameFramework* GameMode = Cast<AGameFramework>(GameModePtr))
	{
		if (GameMode->IsValidGameActorIndex(FullPath[0]))
		{
			for (int32 i = 1; i < FullPath.Num(); i++)
			{
				if (GameMode->IsValidGameActorIndex(FullPath[i]))
				{
					int32 ConnectionIndex;
					if (GameMode->GetGameActorRef(FullPath[i])->IsConnectedTo(GameMode->GetGameActorRef(FullPath[i - 1]), ConnectionIndex))
					{
						OutConnections.Add(GameMode->GetActorConnectionRef(ConnectionIndex));
					}
				}
			}
		}
	}
}
