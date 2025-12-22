// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * SearchAlgorithms:
 * Collection of search algorithm implementations for demonstrating algorithmic knowledge.
 * * Use Case: Finding items in sorted lists, player lookup, inventory search.
 */

namespace SearchAlgorithms
{
    /**
     * Binary Search (Iterative)
     * Time Complexity: O(log n)
     * Space Complexity: O(1)
     * * Prerequisites: Array must be sorted.
     * Best for: Searching in large sorted arrays where memory overhead of recursion is a concern.
     */
    template<typename T>
    int32 BinarySearch(const TArray<T>& SortedArray, const T& Target)
    {
        int32 Left = 0;
        int32 Right = SortedArray.Num() - 1;

        while (Left <= Right)
        {
            // Calculate mid to avoid overflow.
            int32 Mid = Left + (Right - Left) / 2;

            if (SortedArray[Mid] == Target)
            {
                return Mid;
            }
            else if (SortedArray[Mid] < Target)
            {
                Left = Mid + 1; // Target is in the right half.
            }
            else
            {
                Right = Mid - 1; // Target is in the left half.
            }
        }

        return -1; // Not found
    }

    /**
     * Binary Search (Recursive)
     * Time Complexity: O(log n)
     * Space Complexity: O(log n) due to recursion stack.
     */
    template<typename T>
    int32 BinarySearchRecursive(const TArray<T>& SortedArray, const T& Target, int32 Left, int32 Right)
    {
        if (Left > Right)
        {
            return -1; // Base case: Search space exhausted.
        }

        int32 Mid = Left + (Right - Left) / 2;

        if (SortedArray[Mid] == Target)
        {
            return Mid;
        }
        else if (SortedArray[Mid] < Target)
        {
            return BinarySearchRecursive(SortedArray, Target, Mid + 1, Right);
        }
        else
        {
            return BinarySearchRecursive(SortedArray, Target, Left, Mid - 1);
        }
    }

    // Public wrapper for Recursive Binary Search.
    template<typename T>
    int32 BinarySearchRecursive(const TArray<T>& SortedArray, const T& Target)
    {
        return BinarySearchRecursive(SortedArray, Target, 0, SortedArray.Num() - 1);
    }

    /**
     * Linear Search (for comparison)
     * Time Complexity: O(n)
     * Space Complexity: O(1)
     * * Best for: Unsorted arrays, small arrays where overhead of sorting is too high.
     */
    template<typename T>
    int32 LinearSearch(const TArray<T>& Array, const T& Target)
    {
        for (int32 i = 0; i < Array.Num(); ++i)
        {
            if (Array[i] == Target)
            {
                return i;
            }
        }

        return -1;
    }

    

    /**
     * Exponential Search
     * Time Complexity: O(log n)
     * Space Complexity: O(1)
     * * Prerequisites: Array must be sorted.
     * Best for: Unbounded/infinite arrays or when target is near the beginning.
     * Description: Finds a range where the element is present by repeated doubling, then performs Binary Search.
     */
    template<typename T>
    int32 ExponentialSearch(const TArray<T>& SortedArray, const T& Target)
    {
        int32 n = SortedArray.Num();

        if (n == 0)
        {
            return -1;
        }

        // Check first element.
        if (SortedArray[0] == Target)
        {
            return 0;
        }

        // Find range for binary search by exponentially increasing index.
        int32 i = 1;
        while (i < n && SortedArray[i] <= Target)
        {
            i *= 2;
        }

        // Binary search in found range [i/2, min(i, n-1)].
        return BinarySearchRecursive(SortedArray, Target, i / 2, FMath::Min(i, n - 1));
    }
}