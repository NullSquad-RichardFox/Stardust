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
			AGameActor* StartActor = GameMode->GetGameActorRef(StartNode.NodeRef);
			AGameActor* EndActor = GameMode->GetGameActorRef(EndNode.NodeRef);

			int32 ConnectionIndex;
			if (StartActor->IsConnectedTo(EndActor, ConnectionIndex))
			{
				AConnection* Connection = GameMode->GetActorConnectionRef(ConnectionIndex);

				float Distance = Connection->GetLength();
				float DangerLevel = Connection->GetDangerLevel();

				return Distance + DangerLevel * DangerLevelWeight;
			}
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
	FullPath.Empty();

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
	FullPath.Empty();

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

void FFindPath::GetPathIndexes(TArray<int32>& OutIndexes)
{
	OutIndexes = FullPath;
}




void UPathfinder::ActorClicked(AGameActor* Actor)
{
	switch (InteractionStatus)
	{
	case FindingDestination:
		Nodes.Add(Actor);
		CreateRoute();
		InteractionStatus = AddingPathMidpoints;
		break;

	case AddingPathMidpoints:
		int32 Index;
		if (!Nodes.Find(Actor, Index))
		{
			Nodes.EmplaceAt(Nodes.Num() - 1, Actor);
			CreateRoute();
		}
		else if (Index != 0)
		{
			Nodes.Remove(Actor);

			if (Nodes.Num() == 1)
			{
				InteractionStatus = FindingDestination;
				ResetConnectionSelection();
				OnRouteUpdate.Broadcast(0.f, 0.f);
			}
			else
			{
				CreateRoute();
			}
		}
		break;
	}
}

void UPathfinder::SetStartingActor(AGameActor* Actor, UTradeRoutePicker* TradeRouteUI)
{
	if (!GameModePtr) return;

	Nodes.Empty();
	Nodes.Add(Actor);

	ToggleActorClickeEvent(TradeRouteUI);

	PathfinderEngine.SetGameModePtr(GameModePtr);
	InteractionStatus = FindingDestination;
}

void UPathfinder::CleanUp()
{
	ToggleActorClickeEvent(nullptr);
	ResetConnectionSelection();
}

void UPathfinder::GetRouteConnections(TArray<AConnection*>& Connections)
{
	PathfinderEngine.GetActorConnection(Connections);
}

void UPathfinder::GetRouteActors(TArray<AGameActor*>& Actors)
{
	PathfinderEngine.GetGameActors(Actors);
}

void UPathfinder::ToggleActorClickeEvent(UTradeRoutePicker* TradeRouteUI)
{
	if (!GameModePtr) return;

	for (int32 i = 0; i < GameModePtr->GetGameActorNum(); i++)
	{
		GameModePtr->GetGameActorRef(i)->SetFindRouteActive(TradeRouteUI);
	}
}

void UPathfinder::CreateRoute()
{
	ResetConnectionSelection();

	PathfinderEngine.SetNodes(Nodes);
	PathfinderEngine.FindPath();

	TArray<AConnection*> Connections;
	float PathLength = 0.f;
	PathfinderEngine.GetActorConnection(Connections);
	for (AConnection* Connection : Connections)
	{
		Connection->Select();
		PathLength += Connection->GetLength();
	}

	OnRouteUpdate.Broadcast(PathLength, 1.f);
}

void UPathfinder::ResetConnectionSelection()
{
	TArray<AConnection*> Connections;
	PathfinderEngine.GetActorConnection(Connections);
	for (AConnection* Connection : Connections)
		Connection->Deselect();
}