// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * SortingAlgorithms:
 * Collection of sorting algorithm implementations for demonstrating algorithmic knowledge.
 * * Use Case: Leaderboard sorting, enemy priority sorting, inventory organization.
 * Used internally by EnemyDirectorEnhanced for sorting threat priorities.
 */

namespace SortingAlgorithms
{
    /**
     * QuickSort Implementation
     * Time Complexity: O(n log n) average, O(n²) worst case
     * Space Complexity: O(log n) due to recursion stack
     * * Best for: General purpose sorting, good cache locality
     * Description: Picks a pivot element, partitions the array around the pivot, 
     * and recursively sorts sub-arrays.
     */
    template<typename T, typename PredicateType>
    int32 Partition(TArray<T>& Array, int32 Low, int32 High, PredicateType Predicate)
    {
        // Choose the last element as the pivot.
        T Pivot = Array[High];
        int32 i = Low - 1;

        // Iterate through elements and swap if they meet the predicate condition relative to pivot.
        for (int32 j = Low; j < High; ++j)
        {
            if (Predicate(Array[j], Pivot))
            {
                i++;
                // Swap current element with element at partition index.
                T Temp = Array[i];
                Array[i] = Array[j];
                Array[j] = Temp;
            }
        }

        // Place the pivot in the correct sorted position.
        T Temp = Array[i + 1];
        Array[i + 1] = Array[High];
        Array[High] = Temp;

        return i + 1; // Return partition index.
    }

    template<typename T, typename PredicateType>
    void QuickSortRecursive(TArray<T>& Array, int32 Low, int32 High, PredicateType Predicate)
    {
        if (Low < High)
        {
            // Partition the array and get pivot index.
            int32 PartitionIndex = Partition(Array, Low, High, Predicate);
            
            // Recursively sort elements before and after partition.
            QuickSortRecursive(Array, Low, PartitionIndex - 1, Predicate);
            QuickSortRecursive(Array, PartitionIndex + 1, High, Predicate);
        }
    }

    // Public wrapper for QuickSort allowing custom predicates (comparison logic).
    template<typename T, typename PredicateType>
    void QuickSort(TArray<T>& Array, PredicateType Predicate)
    {
        if (Array.Num() > 1)
        {
            QuickSortRecursive(Array, 0, Array.Num() - 1, Predicate);
        }
    }

    // Overload for QuickSort using standard less-than operator.
    template<typename T>
    void QuickSort(TArray<T>& Array)
    {
        QuickSort(Array, [](const T& A, const T& B) { return A < B; });
    }

    /**
     * MergeSort Implementation
     * Time Complexity: O(n log n) in all cases (Stable sort)
     * Space Complexity: O(n) for temporary arrays (Higher memory overhead than QuickSort)
     * * Best for: Linked lists, large datasets requiring stable sorting.
     */
    template<typename T, typename PredicateType>
    void Merge(TArray<T>& Array, int32 Left, int32 Mid, int32 Right, PredicateType Predicate)
    {
        int32 LeftSize = Mid - Left + 1;
        int32 RightSize = Right - Mid;

        // Create temp arrays to hold split data.
        TArray<T> LeftArray;
        TArray<T> RightArray;
        LeftArray.Reserve(LeftSize);
        RightArray.Reserve(RightSize);

        // Copy data to temp arrays.
        for (int32 i = 0; i < LeftSize; ++i)
        {
            LeftArray.Add(Array[Left + i]);
        }
        for (int32 i = 0; i < RightSize; ++i)
        {
            RightArray.Add(Array[Mid + 1 + i]);
        }

        int32 i = 0, j = 0, k = Left;

        // Merge the temp arrays back into the main array in sorted order.
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

        // Copy remaining elements of LeftArray, if any.
        while (i < LeftSize)
        {
            Array[k] = LeftArray[i];
            i++;
            k++;
        }

        // Copy remaining elements of RightArray, if any.
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
            
            // Recursively split the array halves.
            MergeSortRecursive(Array, Left, Mid, Predicate);
            MergeSortRecursive(Array, Mid + 1, Right, Predicate);
            
            // Merge the sorted halves.
            Merge(Array, Left, Mid, Right, Predicate);
        }
    }

    // Public wrapper for MergeSort allowing custom predicates.
    template<typename T, typename PredicateType>
    void MergeSort(TArray<T>& Array, PredicateType Predicate)
    {
        if (Array.Num() > 1)
        {
            MergeSortRecursive(Array, 0, Array.Num() - 1, Predicate);
        }
    }

    // Overload for MergeSort using standard less-than operator.
    template<typename T>
    void MergeSort(TArray<T>& Array)
    {
        MergeSort(Array, [](const T& A, const T& B) { return A < B; });
    }
}