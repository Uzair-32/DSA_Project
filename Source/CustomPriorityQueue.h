// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * CustomPriorityQueue:
 * A min-heap based priority queue implementation.
 * * Time Complexity:
 * - Insert (Enqueue): O(log n) - Tree height traversal
 * - Remove Min (Dequeue): O(log n) - Tree height traversal
 * - Peek: O(1) - Root access
 * * Space Complexity: O(n)
 * * Use Case: 
 * 1. A* Pathfinding (Open Set management)
 * 2. Enemy Targeting (Sorting enemies by threat level)
 */
template<typename ElementType>
class PROJECT_GOLDFISH_API CustomPriorityQueue
{
private:
    struct PQElement
    {
        ElementType Element;
        float Priority; // Lower value = Higher Priority (Min-Heap)

        PQElement() : Priority(0.0f) {}
        PQElement(const ElementType& InElement, float InPriority)
            : Element(InElement), Priority(InPriority)
        {
        }

        // Overloaded operator for easy comparison between nodes
        bool operator<(const PQElement& Other) const
        {
            return Priority < Other.Priority;
        }
    };

    // The binary heap is stored as a flat array for memory efficiency.
    TArray<PQElement> Heap;

    // Helper functions to navigate the array as a binary tree.
    int32 GetParentIndex(int32 Index) const { return (Index - 1) / 2; }
    int32 GetLeftChildIndex(int32 Index) const { return 2 * Index + 1; }
    int32 GetRightChildIndex(int32 Index) const { return 2 * Index + 2; }
    bool HasParent(int32 Index) const { return GetParentIndex(Index) >= 0; }
    bool HasLeftChild(int32 Index) const { return GetLeftChildIndex(Index) < Heap.Num(); }
    bool HasRightChild(int32 Index) const { return GetRightChildIndex(Index) < Heap.Num(); }

    void Swap(int32 IndexA, int32 IndexB)
    {
        PQElement Temp = Heap[IndexA];
        Heap[IndexA] = Heap[IndexB];
        Heap[IndexB] = Temp;
    }

    // Restores heap property by bubbling a node up.
    void HeapifyUp(int32 Index)
    {
        while (HasParent(Index) && Heap[Index] < Heap[GetParentIndex(Index)])
        {
            Swap(Index, GetParentIndex(Index));
            Index = GetParentIndex(Index);
        }
    }

    // Restores heap property by sinking a node down.
    void HeapifyDown(int32 Index)
    {
        while (HasLeftChild(Index))
        {
            int32 SmallerChildIndex = GetLeftChildIndex(Index);

            // Find the smaller of the two children to swap with.
            if (HasRightChild(Index) && Heap[GetRightChildIndex(Index)] < Heap[SmallerChildIndex])
            {
                SmallerChildIndex = GetRightChildIndex(Index);
            }

            if (Heap[Index] < Heap[SmallerChildIndex])
            {
                break; // Heap property is satisfied.
            }
            else
            {
                Swap(Index, SmallerChildIndex);
                Index = SmallerChildIndex;
            }
        }
    }

public:
    CustomPriorityQueue()
    {
        Heap.Reserve(16);
    }

    // Add element and rebalance the heap.
    void Enqueue(const ElementType& Element, float Priority)
    {
        PQElement NewElement(Element, Priority);
        Heap.Add(NewElement);
        HeapifyUp(Heap.Num() - 1);
    }

    // Remove the highest priority (lowest value) element.
    bool Dequeue(ElementType& OutElement)
    {
        if (IsEmpty())
        {
            return false;
        }

        // The root (index 0) is always the minimum element.
        OutElement = Heap[0].Element;
        
        // Move the last element to the root and bubble it down.
        Heap[0] = Heap[Heap.Num() - 1];
        Heap.RemoveAt(Heap.Num() - 1);

        if (!IsEmpty())
        {
            HeapifyDown(0);
        }

        return true;
    }

    // View the highest priority element without removing it.
    bool Peek(ElementType& OutElement) const
    {
        if (IsEmpty())
        {
            return false;
        }

        OutElement = Heap[0].Element;
        return true;
    }

    bool IsEmpty() const
    {
        return Heap.Num() == 0;
    }

    int32 Size() const
    {
        return Heap.Num();
    }

    void Clear()
    {
        Heap.Empty();
    }

    // Change the priority of an existing element and rebalance.
    bool UpdatePriority(const ElementType& Element, float NewPriority)
    {
        for (int32 i = 0; i < Heap.Num(); ++i)
        {
            if (Heap[i].Element == Element)
            {
                float OldPriority = Heap[i].Priority;
                Heap[i].Priority = NewPriority;

                // Determine whether to bubble up or sink down based on the change.
                if (NewPriority < OldPriority)
                {
                    HeapifyUp(i);
                }
                else
                {
                    HeapifyDown(i);
                }

                return true;
            }
        }

        return false;
    }

    // Check if an element exists in the queue (O(n) linear scan).
    bool Contains(const ElementType& Element) const
    {
        for (const PQElement& Item : Heap)
        {
            if (Item.Element == Element)
            {
                return true;
            }
        }
        return false;
    }
};