// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * SearchAlgorithms:
 * Collection of search algorithm implementations for demonstrating algorithmic knowledge.
 * 
 * Use Case: Finding items in sorted lists, player lookup, inventory search
 */

namespace SearchAlgorithms
{
    /**
     * Binary Search (Iterative)
     * Time Complexity: O(log n)
     * Space Complexity: O(1)
     * 
     * Prerequisites: Array must be sorted
     * Best for: Searching in large sorted arrays
     */
    template<typename T>
    int32 BinarySearch(const TArray<T>& SortedArray, const T& Target)
    {
        int32 Left = 0;
        int32 Right = SortedArray.Num() - 1;

        while (Left <= Right)
        {
            int32 Mid = Left + (Right - Left) / 2;

            if (SortedArray[Mid] == Target)
            {
                return Mid;
            }
            else if (SortedArray[Mid] < Target)
            {
                Left = Mid + 1;
            }
            else
            {
                Right = Mid - 1;
            }
        }

        return -1; // Not found
    }

    /**
     * Binary Search (Recursive)
     * Time Complexity: O(log n)
     * Space Complexity: O(log n) due to recursion stack
     */
    template<typename T>
    int32 BinarySearchRecursive(const TArray<T>& SortedArray, const T& Target, int32 Left, int32 Right)
    {
        if (Left > Right)
        {
            return -1;
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

    template<typename T>
    int32 BinarySearchRecursive(const TArray<T>& SortedArray, const T& Target)
    {
        return BinarySearchRecursive(SortedArray, Target, 0, SortedArray.Num() - 1);
    }

    /**
     * Linear Search (for comparison)
     * Time Complexity: O(n)
     * Space Complexity: O(1)
     * 
     * Best for: Unsorted arrays, small arrays
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
     * Jump Search
     * Time Complexity: O(√n)
     * Space Complexity: O(1)
     * 
     * Prerequisites: Array must be sorted
     * Best for: When binary search is too complex for the use case
     */
    template<typename T>
    int32 JumpSearch(const TArray<T>& SortedArray, const T& Target)
    {
        int32 n = SortedArray.Num();
        int32 Step = FMath::Sqrt(static_cast<float>(n));
        int32 Prev = 0;

        // Find the block where element may be present
        while (Prev < n && SortedArray[FMath::Min(Step, n) - 1] < Target)
        {
            Prev = Step;
            Step += FMath::Sqrt(static_cast<float>(n));

            if (Prev >= n)
            {
                return -1;
            }
        }

        // Linear search in the identified block
        while (Prev < n && SortedArray[Prev] < Target)
        {
            Prev++;

            if (Prev == FMath::Min(Step, n))
            {
                return -1;
            }
        }

        if (Prev < n && SortedArray[Prev] == Target)
        {
            return Prev;
        }

        return -1;
    }

    /**
     * Interpolation Search
     * Time Complexity: O(log log n) for uniformly distributed data, O(n) worst case
     * Space Complexity: O(1)
     * 
     * Prerequisites: Array must be sorted and uniformly distributed
     * Best for: When values are uniformly distributed
     */
    template<typename T>
    int32 InterpolationSearch(const TArray<T>& SortedArray, const T& Target)
    {
        int32 Left = 0;
        int32 Right = SortedArray.Num() - 1;

        while (Left <= Right && Target >= SortedArray[Left] && Target <= SortedArray[Right])
        {
            if (Left == Right)
            {
                if (SortedArray[Left] == Target)
                {
                    return Left;
                }
                return -1;
            }

            // Interpolation formula
            float Ratio = static_cast<float>(Target - SortedArray[Left]) / 
                         static_cast<float>(SortedArray[Right] - SortedArray[Left]);
            int32 Pos = Left + static_cast<int32>(Ratio * (Right - Left));

            // Bounds check
            Pos = FMath::Clamp(Pos, Left, Right);

            if (SortedArray[Pos] == Target)
            {
                return Pos;
            }

            if (SortedArray[Pos] < Target)
            {
                Left = Pos + 1;
            }
            else
            {
                Right = Pos - 1;
            }
        }

        return -1;
    }

    /**
     * Exponential Search
     * Time Complexity: O(log n)
     * Space Complexity: O(1)
     * 
     * Prerequisites: Array must be sorted
     * Best for: Unbounded/infinite arrays or when target is near the beginning
     */
    template<typename T>
    int32 ExponentialSearch(const TArray<T>& SortedArray, const T& Target)
    {
        int32 n = SortedArray.Num();

        if (n == 0)
        {
            return -1;
        }

        if (SortedArray[0] == Target)
        {
            return 0;
        }

        // Find range for binary search
        int32 i = 1;
        while (i < n && SortedArray[i] <= Target)
        {
            i *= 2;
        }

        // Binary search in found range
        return BinarySearchRecursive(SortedArray, Target, i / 2, FMath::Min(i, n - 1));
    }
}