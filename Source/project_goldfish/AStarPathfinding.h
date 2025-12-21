// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomPriorityQueue.h"

/**
 * AStarPathfinding:
 * A* pathfinding algorithm implementation for demonstrating search algorithms.
 * 
 * Time Complexity: O(b^d) where b is branching factor, d is depth
 * In practice with heuristic: O(|E| log |V|) where E=edges, V=vertices
 * Space Complexity: O(|V|) for storing nodes
 * 
 * Use Case: Enemy pathfinding, navigation, route planning
 */

// Node structure for A* algorithm
struct FAStarNode
{
    FVector Position;
    float GCost; // Cost from start
    float HCost; // Heuristic cost to end
    float FCost; // Total cost (G + H)
    FAStarNode* Parent;
    bool bIsWalkable;

    FAStarNode()
        : Position(FVector::ZeroVector)
        , GCost(0.0f)
        , HCost(0.0f)
        , FCost(0.0f)
        , Parent(nullptr)
        , bIsWalkable(true)
    {
    }

    FAStarNode(const FVector& InPosition)
        : Position(InPosition)
        , GCost(0.0f)
        , HCost(0.0f)
        , FCost(0.0f)
        , Parent(nullptr)
        , bIsWalkable(true)
    {
    }

    void CalculateFCost()
    {
        FCost = GCost + HCost;
    }

    bool operator==(const FAStarNode& Other) const
    {
        return Position.Equals(Other.Position, 1.0f);
    }
};

class PROJECT_GOLDFISH_API FAStarPathfinding
{
private:
    // Calculate heuristic (Manhattan distance for grid, Euclidean for free movement)
    static float CalculateHeuristic(const FVector& Start, const FVector& End, bool bUseEuclidean = true)
    {
        if (bUseEuclidean)
        {
            return FVector::Dist(Start, End);
        }
        else
        {
            // Manhattan distance
            return FMath::Abs(Start.X - End.X) + FMath::Abs(Start.Y - End.Y) + FMath::Abs(Start.Z - End.Z);
        }
    }

    // Get neighboring nodes (8-directional for 2D, or custom)
    static void GetNeighbors(FAStarNode* CurrentNode, TArray<FAStarNode*>& Neighbors, 
                            const TArray<FAStarNode*>& AllNodes, float GridSize = 100.0f)
    {
        Neighbors.Empty();

        // Define 8 directions (or 26 for 3D)
        TArray<FVector> Directions = {
            FVector(GridSize, 0, 0),      // Right
            FVector(-GridSize, 0, 0),     // Left
            FVector(0, GridSize, 0),      // Forward
            FVector(0, -GridSize, 0),     // Back
            FVector(GridSize, GridSize, 0),    // Diagonal
            FVector(-GridSize, GridSize, 0),
            FVector(GridSize, -GridSize, 0),
            FVector(-GridSize, -GridSize, 0)
        };

        for (const FVector& Direction : Directions)
        {
            FVector NeighborPos = CurrentNode->Position + Direction;

            // Find node at this position
            for (FAStarNode* Node : AllNodes)
            {
                if (Node->Position.Equals(NeighborPos, 1.0f) && Node->bIsWalkable)
                {
                    Neighbors.Add(Node);
                    break;
                }
            }
        }
    }

    // Reconstruct path from end node to start
    static void ReconstructPath(FAStarNode* EndNode, TArray<FVector>& OutPath)
    {
        OutPath.Empty();
        FAStarNode* CurrentNode = EndNode;

        while (CurrentNode != nullptr)
        {
            OutPath.Insert(CurrentNode->Position, 0);
            CurrentNode = CurrentNode->Parent;
        }
    }

public:
    /**
     * Find path using A* algorithm
     * Returns true if path found, false otherwise
     * OutPath contains the path from start to end
     */
    static bool FindPath(const FVector& StartPos, const FVector& EndPos, 
                        const TArray<FAStarNode*>& AllNodes,
                        TArray<FVector>& OutPath, float GridSize = 100.0f)
    {
        // Find start and end nodes
        FAStarNode* StartNode = nullptr;
        FAStarNode* EndNode = nullptr;

        for (FAStarNode* Node : AllNodes)
        {
            if (Node->Position.Equals(StartPos, 1.0f))
            {
                StartNode = Node;
            }
            if (Node->Position.Equals(EndPos, 1.0f))
            {
                EndNode = Node;
            }

            // Reset node data
            Node->GCost = 0.0f;
            Node->HCost = 0.0f;
            Node->FCost = 0.0f;
            Node->Parent = nullptr;
        }

        if (StartNode == nullptr || EndNode == nullptr)
        {
            return false;
        }

        // Open list (nodes to be evaluated) - using custom priority queue
        CustomPriorityQueue<FAStarNode*> OpenList;
        
        // Closed list (nodes already evaluated)
        TArray<FAStarNode*> ClosedList;

        // Add start node to open list
        StartNode->HCost = CalculateHeuristic(StartNode->Position, EndNode->Position);
        StartNode->CalculateFCost();
        OpenList.Enqueue(StartNode, StartNode->FCost);

        while (!OpenList.IsEmpty())
        {
            // Get node with lowest F cost
            FAStarNode* CurrentNode;
            OpenList.Dequeue(CurrentNode);

            // Add to closed list
            ClosedList.Add(CurrentNode);

            // Check if we reached the end
            if (*CurrentNode == *EndNode)
            {
                ReconstructPath(CurrentNode, OutPath);
                return true;
            }

            // Get neighbors
            TArray<FAStarNode*> Neighbors;
            GetNeighbors(CurrentNode, Neighbors, AllNodes, GridSize);

            for (FAStarNode* Neighbor : Neighbors)
            {
                // Skip if in closed list
                if (ClosedList.Contains(Neighbor))
                {
                    continue;
                }

                // Calculate tentative G cost
                float TentativeGCost = CurrentNode->GCost + 
                    FVector::Dist(CurrentNode->Position, Neighbor->Position);

                // Check if this path is better
                bool bInOpenList = OpenList.Contains(Neighbor);
                
                if (!bInOpenList || TentativeGCost < Neighbor->GCost)
                {
                    // Update neighbor
                    Neighbor->Parent = CurrentNode;
                    Neighbor->GCost = TentativeGCost;
                    Neighbor->HCost = CalculateHeuristic(Neighbor->Position, EndNode->Position);
                    Neighbor->CalculateFCost();

                    if (!bInOpenList)
                    {
                        OpenList.Enqueue(Neighbor, Neighbor->FCost);
                    }
                    else
                    {
                        // Update priority
                        OpenList.UpdatePriority(Neighbor, Neighbor->FCost);
                    }
                }
            }
        }

        // No path found
        return false;
    }

    /**
     * Simple pathfinding using existing nav mesh but with A* concepts
     * This version works with UE's navigation system
     */
    static bool FindPathSimple(const FVector& StartPos, const FVector& EndPos,
                               UWorld* World, TArray<FVector>& OutPath, int32 MaxSteps = 100)
    {
        if (World == nullptr)
        {
            return false;
        }

        OutPath.Empty();
        OutPath.Add(StartPos);

        FVector CurrentPos = StartPos;
        int32 Steps = 0;

        while (!CurrentPos.Equals(EndPos, 50.0f) && Steps < MaxSteps)
        {
            // Move towards target
            FVector Direction = (EndPos - CurrentPos).GetSafeNormal();
            FVector NextPos = CurrentPos + Direction * 100.0f;

            // Simple collision check
            FHitResult HitResult;
            bool bHit = World->LineTraceSingleByChannel(
                HitResult,
                CurrentPos,
                NextPos,
                ECC_Visibility
            );

            if (!bHit)
            {
                CurrentPos = NextPos;
                OutPath.Add(CurrentPos);
            }
            else
            {
                // Try to go around obstacle
                FVector RightDir = FVector::CrossProduct(Direction, FVector::UpVector);
                FVector AlternatePos = CurrentPos + RightDir * 100.0f;
                
                CurrentPos = AlternatePos;
                OutPath.Add(CurrentPos);
            }

            Steps++;
        }

        OutPath.Add(EndPos);
        return Steps < MaxSteps;
    }
};