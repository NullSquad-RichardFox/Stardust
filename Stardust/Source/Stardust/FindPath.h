// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GraphAstar.h"
#include "FindPath.generated.h"


class AGameActor;
class AConnection;
class AGameFramework;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRouteUpdateEventSignature, float, Length, float, DangerLevel);

USTRUCT()
struct STARDUST_API FFindPath
{
	GENERATED_BODY()

private:
	struct FGraphNode
	{
		typedef int32 FGraphNodeRef;

		const FGraphNodeRef NodeRef;
		FGraphNodeRef ParentRef;
		float TraversalCost;
		float TotalCost;
		int32 SearchNodeIndex;
		int32 ParentNodeIndex;
		uint8 bIsOpened : 1;
		uint8 bIsClosed : 1;

		FORCEINLINE FGraphNode(const FGraphNodeRef& InNodeRef)
			:NodeRef(InNodeRef),
			ParentRef(InNodeRef),
			TraversalCost(1.0f),
			TotalCost(MAX_FLT),
			SearchNodeIndex(INDEX_NONE),
			ParentNodeIndex(INDEX_NONE),
			bIsOpened(false),
			bIsClosed(false) 
		{}

		FORCEINLINE void MarkOpened() { bIsOpened = true; }
		FORCEINLINE void MarkNotOpened() { bIsOpened = false; }
		FORCEINLINE void MarkClosed() { bIsClosed = true; }
		FORCEINLINE void MarkNotClosed() { bIsClosed = false; }
		FORCEINLINE bool IsOpened() const { return bIsOpened; }
		FORCEINLINE bool IsClosed() const { return bIsClosed; }
	};

	struct FGraph
	{
		typedef FGraphNode FSearchNode;
		typedef int32 FNodeRef;

		FSearchNode GetNodeByRef(FNodeRef NodeRef);
		int32 GetNeighbourCount(FNodeRef NodeRef) const;
		bool IsValidRef(FNodeRef NodeRef) const;
		FNodeRef GetNeighbour(const FSearchNode& NodeRef, const int32 NeighbourIndex) const;

		FGraph()
			:GameModePointer(nullptr)
		{}

		FGraph(AGameModeBase* GameMode)
			:GameModePointer(GameMode)
		{}

	private:
		AGameModeBase* GameModePointer;
	};

	struct FQuery
	{
		typedef FGraphNode FSearchNode;
		typedef int32 FNodeRef;

		float GetHeuristicScale() const;
		float GetHeuristicCost(const FSearchNode& StartNode, const FSearchNode& EndNode) const;
		float GetTraversalCost(const FSearchNode& StartNode, const FSearchNode& EndNode) const;
		bool IsTraversalAllowed(const FNodeRef NodeA, const FNodeRef NodeB) const;
		bool WantsPartialSolution() const;

		int32 GetNeighbourCount(FNodeRef NodeRef) const;
		bool ShouldIgnoreClosedNodes() const;
		bool ShouldIncludeStartNodeInPath() const;

		FQuery()
			:GameModePointer(nullptr)
		{}

		FQuery(AGameModeBase* GameMode)
			:GameModePointer(GameMode)
		{}

	private:
		float DangerLevelWeight = 100.f;
		AGameModeBase* GameModePointer;
	};
public:
	FFindPath()
		:GameModePtr(nullptr) {}

	FFindPath(AGameModeBase* GameMode)
		:GameModePtr(GameMode) {}

	FORCEINLINE void SetGameModePtr(AGameModeBase* GameMode) { GameModePtr = GameMode; }

	void AddNode(AGameActor* Node, int32 Index);
	void SetNodes(const TArray<AGameActor*>& InNodes);

	//Finds the shortest path from an array of nodes. Use AddNode function to set the nodes.
	EGraphAStarResult FindPath();

	//Find the shortest path from one actor to another
	EGraphAStarResult FindPath(AGameActor* StartActor, AGameActor* EndActor);

	void GetGameActors(TArray<AGameActor*>& OutActors);
	void GetActorConnection(TArray<AConnection*>& OutConnections);
	void GetPathIndexes(TArray<int32>& OutIndexes);


private:
	TArray<int32> FullPath;
	TArray<AGameActor*> Nodes;
	AGameModeBase* GameModePtr;
};

class STARDUST_API UPathfinder
{
private:
	typedef enum
	{
		FindingDestination,
		AddingPathMidpoints
	} InteractionType;

public:
	UPathfinder()
		:GameModePtr(nullptr) {}

	UPathfinder(AGameFramework* GameModePtr)
		:GameModePtr(GameModePtr) {}

	void ActorClicked(AGameActor* Actor);
	void SetStartingActor(AGameActor* Actor);

	void CleanUp();

	void GetRouteConnections(TArray<AConnection*>& Connections);
	void GetRouteActors(TArray<AGameActor*>& Actors);

	FRouteUpdateEventSignature OnRouteUpdate;

private:
	void CreateRoute();
	void ResetConnectionSelection();

	void ToggleActorClickeEvent(bool Toggle);

	TArray<AGameActor*> Nodes;
	AGameFramework* GameModePtr;
	FFindPath PathfinderEngine;
	InteractionType InteractionStatus;
};