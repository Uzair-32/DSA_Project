// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * CustomStack:
 * A stack (LIFO - Last In, First Out) implementation using Unreal's TArray.
 * * Time Complexity:
 * - Push: O(1)
 * - Pop: O(1)
 * - Peek: O(1)
 * * Space Complexity: O(n)
 * * Use Case: Menu navigation (Back button), Undo/Redo history, or depth-first search algorithms.
 */
template<typename ElementType>
class PROJECT_GOLDFISH_API CustomStack
{
private:
	// Underlying storage for the stack elements.
	TArray<ElementType> Data;
	
	// Optional limit on stack size (-1 indicates no limit).
	int32 MaxCapacity;

public:
	// Constructor: Sets capacity and pre-allocates memory to prevent frequent reallocations.
	CustomStack(int32 InMaxCapacity = -1)
		: MaxCapacity(InMaxCapacity)
	{
		Data.Reserve(MaxCapacity > 0 ? MaxCapacity : 32);
	}

	// Add an element to the top of the stack.
	bool Push(const ElementType& Element)
	{
		if (MaxCapacity > 0 && Data.Num() >= MaxCapacity)
		{
			return false; // Stack Overflow protection
		}

		Data.Add(Element);
		return true;
	}

	// Remove the top element and return it.
	bool Pop(ElementType& OutElement)
	{
		if (IsEmpty())
		{
			return false; // Stack Underflow protection
		}

		OutElement = Data.Last();
		Data.RemoveAt(Data.Num() - 1);
		return true;
	}

	// Remove the top element without returning it.
	bool Pop()
	{
		if (IsEmpty())
		{
			return false;
		}

		Data.RemoveAt(Data.Num() - 1);
		return true;
	}

	// View the top element without removing it.
	bool Peek(ElementType& OutElement) const
	{
		if (IsEmpty())
		{
			return false;
		}

		OutElement = Data.Last();
		return true;
	}

	// Check if stack has no elements.
	bool IsEmpty() const
	{
		return Data.Num() == 0;
	}

	// Get current element count.
	int32 Size() const
	{
		return Data.Num();
	}

	// Check if stack has reached its capacity limit.
	bool IsFull() const
	{
		return MaxCapacity > 0 && Data.Num() >= MaxCapacity;
	}

	// Empty the stack.
	void Clear()
	{
		Data.Empty();
	}

	// Resize the capacity limit, truncating data if necessary.
	void SetMaxCapacity(int32 NewCapacity)
	{
		MaxCapacity = NewCapacity;
        
		if (MaxCapacity > 0 && Data.Num() > MaxCapacity)
		{
			Data.SetNum(MaxCapacity);
		}
	}
};