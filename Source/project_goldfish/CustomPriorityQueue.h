// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * CustomPriorityQueue:
 * A min-heap based priority queue implementation.
 * 
 * Time Complexity:
 * - Insert (Enqueue): O(log n)
 * - Remove Min (Dequeue): O(log n)
 * - Peek: O(1)
 * 
 * Space Complexity: O(n)
 * 
 * Use Case: AI target prioritization, event scheduling, pathfinding node evaluation
 */
template<typename ElementType>
class PROJECT_GOLDFISH_API CustomPriorityQueue
{
private:
    struct PQElement
    {
        ElementType Element;
        float Priority;

        PQElement() : Priority(0.0f) {}
        PQElement(const ElementType& InElement, float InPriority)
            : Element(InElement), Priority(InPriority)
        {
        }

        bool operator<(const PQElement& Other) const
        {
            return Priority < Other.Priority;
        }
    };

    TArray<PQElement> Heap;

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

    void HeapifyUp(int32 Index)
    {
        while (HasParent(Index) && Heap[Index] < Heap[GetParentIndex(Index)])
        {
            Swap(Index, GetParentIndex(Index));
            Index = GetParentIndex(Index);
        }
    }

    void HeapifyDown(int32 Index)
    {
        while (HasLeftChild(Index))
        {
            int32 SmallerChildIndex = GetLeftChildIndex(Index);

            if (HasRightChild(Index) && Heap[GetRightChildIndex(Index)] < Heap[SmallerChildIndex])
            {
                SmallerChildIndex = GetRightChildIndex(Index);
            }

            if (Heap[Index] < Heap[SmallerChildIndex])
            {
                break;
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

    void Enqueue(const ElementType& Element, float Priority)
    {
        PQElement NewElement(Element, Priority);
        Heap.Add(NewElement);
        HeapifyUp(Heap.Num() - 1);
    }

    bool Dequeue(ElementType& OutElement)
    {
        if (IsEmpty())
        {
            return false;
        }

        OutElement = Heap[0].Element;
        Heap[0] = Heap[Heap.Num() - 1];
        Heap.RemoveAt(Heap.Num() - 1);

        if (!IsEmpty())
        {
            HeapifyDown(0);
        }

        return true;
    }

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

    bool UpdatePriority(const ElementType& Element, float NewPriority)
    {
        for (int32 i = 0; i < Heap.Num(); ++i)
        {
            if (Heap[i].Element == Element)
            {
                float OldPriority = Heap[i].Priority;
                Heap[i].Priority = NewPriority;

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