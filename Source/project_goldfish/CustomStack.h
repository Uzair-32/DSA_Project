// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * CustomStack:
 * A stack (LIFO) implementation.
 * 
 * Time Complexity:
 * - Push: O(1)
 * - Pop: O(1)
 * - Peek: O(1)
 * 
 * Space Complexity: O(n)
 * 
 * Use Case: Game state history (undo/redo), action history, menu navigation
 */
template<typename ElementType>
class PROJECT_GOLDFISH_API CustomStack
{
private:
	TArray<ElementType> Data;
	int32 MaxCapacity;

public:
	CustomStack(int32 InMaxCapacity = -1)
		: MaxCapacity(InMaxCapacity)
	{
		Data.Reserve(MaxCapacity > 0 ? MaxCapacity : 32);
	}

	bool Push(const ElementType& Element)
	{
		if (MaxCapacity > 0 && Data.Num() >= MaxCapacity)
		{
			return false;
		}

		Data.Add(Element);
		return true;
	}

	bool Pop(ElementType& OutElement)
	{
		if (IsEmpty())
		{
			return false;
		}

		OutElement = Data.Last();
		Data.RemoveAt(Data.Num() - 1);
		return true;
	}

	bool Pop()
	{
		if (IsEmpty())
		{
			return false;
		}

		Data.RemoveAt(Data.Num() - 1);
		return true;
	}

	bool Peek(ElementType& OutElement) const
	{
		if (IsEmpty())
		{
			return false;
		}

		OutElement = Data.Last();
		return true;
	}

	bool IsEmpty() const
	{
		return Data.Num() == 0;
	}

	int32 Size() const
	{
		return Data.Num();
	}

	bool IsFull() const
	{
		return MaxCapacity > 0 && Data.Num() >= MaxCapacity;
	}

	void Clear()
	{
		Data.Empty();
	}

	void SetMaxCapacity(int32 NewCapacity)
	{
		MaxCapacity = NewCapacity;
        
		if (MaxCapacity > 0 && Data.Num() > MaxCapacity)
		{
			Data.SetNum(MaxCapacity);
		}
	}
};