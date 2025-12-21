// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * SortingAlgorithms:
 * Collection of sorting algorithm implementations for demonstrating algorithmic knowledge.
 * 
 * Use Case: Leaderboard sorting, enemy priority sorting, inventory organization
 */

namespace SortingAlgorithms
{
    /**
     * QuickSort Implementation
     * Time Complexity: O(n log n) average, O(n²) worst case
     * Space Complexity: O(log n) due to recursion stack
     * 
     * Best for: General purpose sorting, good cache locality
     */
    template<typename T, typename PredicateType>
    int32 Partition(TArray<T>& Array, int32 Low, int32 High, PredicateType Predicate)
    {
        T Pivot = Array[High];
        int32 i = Low - 1;

        for (int32 j = Low; j < High; ++j)
        {
            if (Predicate(Array[j], Pivot))
            {
                i++;
                T Temp = Array[i];
                Array[i] = Array[j];
                Array[j] = Temp;
            }
        }

        T Temp = Array[i + 1];
        Array[i + 1] = Array[High];
        Array[High] = Temp;

        return i + 1;
    }

    template<typename T, typename PredicateType>
    void QuickSortRecursive(TArray<T>& Array, int32 Low, int32 High, PredicateType Predicate)
    {
        if (Low < High)
        {
            int32 PartitionIndex = Partition(Array, Low, High, Predicate);
            QuickSortRecursive(Array, Low, PartitionIndex - 1, Predicate);
            QuickSortRecursive(Array, PartitionIndex + 1, High, Predicate);
        }
    }

    template<typename T, typename PredicateType>
    void QuickSort(TArray<T>& Array, PredicateType Predicate)
    {
        if (Array.Num() > 1)
        {
            QuickSortRecursive(Array, 0, Array.Num() - 1, Predicate);
        }
    }

    template<typename T>
    void QuickSort(TArray<T>& Array)
    {
        QuickSort(Array, [](const T& A, const T& B) { return A < B; });
    }

    /**
     * MergeSort Implementation
     * Time Complexity: O(n log n) in all cases
     * Space Complexity: O(n) for temporary arrays
     */
    template<typename T, typename PredicateType>
    void Merge(TArray<T>& Array, int32 Left, int32 Mid, int32 Right, PredicateType Predicate)
    {
        int32 LeftSize = Mid - Left + 1;
        int32 RightSize = Right - Mid;

        TArray<T> LeftArray;
        TArray<T> RightArray;
        LeftArray.Reserve(LeftSize);
        RightArray.Reserve(RightSize);

        for (int32 i = 0; i < LeftSize; ++i)
        {
            LeftArray.Add(Array[Left + i]);
        }
        for (int32 i = 0; i < RightSize; ++i)
        {
            RightArray.Add(Array[Mid + 1 + i]);
        }

        int32 i = 0, j = 0, k = Left;

        while (i < LeftSize && j < RightSize)
        {
            if (Predicate(LeftArray[i], RightArray[j]))
            {
                Array[k] = LeftArray[i];
                i++;
            }
            else
            {
                Array[k] = RightArray[j];
                j++;
            }
            k++;
        }

        while (i < LeftSize)
        {
            Array[k] = LeftArray[i];
            i++;
            k++;
        }

        while (j < RightSize)
        {
            Array[k] = RightArray[j];
            j++;
            k++;
        }
    }

    template<typename T, typename PredicateType>
    void MergeSortRecursive(TArray<T>& Array, int32 Left, int32 Right, PredicateType Predicate)
    {
        if (Left < Right)
        {
            int32 Mid = Left + (Right - Left) / 2;
            MergeSortRecursive(Array, Left, Mid, Predicate);
            MergeSortRecursive(Array, Mid + 1, Right, Predicate);
            Merge(Array, Left, Mid, Right, Predicate);
        }
    }

    template<typename T, typename PredicateType>
    void MergeSort(TArray<T>& Array, PredicateType Predicate)
    {
        if (Array.Num() > 1)
        {
            MergeSortRecursive(Array, 0, Array.Num() - 1, Predicate);
        }
    }

    template<typename T>
    void MergeSort(TArray<T>& Array)
    {
        MergeSort(Array, [](const T& A, const T& B) { return A < B; });
    }
}