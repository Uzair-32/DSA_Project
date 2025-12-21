// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"

/**
 * CustomHashMap:
 * A custom hash map implementation for demonstrating data structure knowledge.
 * Uses separate chaining for collision resolution.
 * 
 * Time Complexity:
 * - Insert: O(1) average, O(n) worst case
 * - Search: O(1) average, O(n) worst case
 * - Delete: O(1) average, O(n) worst case
 * 
 * Space Complexity: O(n) where n is the number of elements
 * 
 * Use Case: Fast lookup of enemies by ID, weapon registry, collectible management
 */
template<typename KeyType, typename ValueType>
class PROJECT_GOLDFISH_API CustomHashMap
{
private:
    // Node structure for chaining
    struct HashNode
    {
        KeyType Key;
        ValueType Value;
        TSharedPtr<HashNode> Next;

        HashNode(const KeyType& InKey, const ValueType& InValue)
            : Key(InKey), Value(InValue), Next(nullptr)
        {
        }
    };

    TArray<TSharedPtr<HashNode>> Buckets;
    int32 Size;
    int32 Capacity;
    float LoadFactor;

    int32 Hash(const KeyType& Key) const
    {
        if (Capacity == 0) return 0;
        uint32 HashValue = GetTypeHash(Key);
        return HashValue % Capacity;
    }

    void Rehash()
    {
        int32 OldCapacity = Capacity;
        Capacity *= 2;
        TArray<TSharedPtr<HashNode>> OldBuckets = Buckets;
        
        Buckets.Empty();
        Buckets.SetNum(Capacity);
        Size = 0;

        for (int32 i = 0; i < OldCapacity; ++i)
        {
            TSharedPtr<HashNode> Current = OldBuckets[i];
            while (Current.IsValid())
            {
                Insert(Current->Key, Current->Value);
                Current = Current->Next;
            }
        }
    }

public:
    CustomHashMap(int32 InitialCapacity = 16)
        : Size(0), Capacity(InitialCapacity), LoadFactor(0.75f)
    {
        Buckets.SetNum(Capacity);
    }

    void Insert(const KeyType& Key, const ValueType& Value)
    {
        if ((float)Size / Capacity > LoadFactor)
        {
            Rehash();
        }

        int32 Index = Hash(Key);
        TSharedPtr<HashNode> Current = Buckets[Index];

        while (Current.IsValid())
        {
            if (Current->Key == Key)
            {
                Current->Value = Value;
                return;
            }
            Current = Current->Next;
        }

        TSharedPtr<HashNode> NewNode = MakeShared<HashNode>(Key, Value);
        NewNode->Next = Buckets[Index];
        Buckets[Index] = NewNode;
        Size++;
    }

    bool Find(const KeyType& Key, ValueType& OutValue) const
    {
        if (Capacity == 0) return false;
        
        int32 Index = Hash(Key);
        TSharedPtr<HashNode> Current = Buckets[Index];

        while (Current.IsValid())
        {
            if (Current->Key == Key)
            {
                OutValue = Current->Value;
                return true;
            }
            Current = Current->Next;
        }

        return false;
    }

    bool Contains(const KeyType& Key) const
    {
        if (Capacity == 0) return false;
        
        int32 Index = Hash(Key);
        TSharedPtr<HashNode> Current = Buckets[Index];

        while (Current.IsValid())
        {
            if (Current->Key == Key)
            {
                return true;
            }
            Current = Current->Next;
        }

        return false;
    }

    bool Remove(const KeyType& Key)
    {
        if (Capacity == 0) return false;
        
        int32 Index = Hash(Key);
        TSharedPtr<HashNode> Current = Buckets[Index];
        TSharedPtr<HashNode> Previous = nullptr;

        while (Current.IsValid())
        {
            if (Current->Key == Key)
            {
                if (Previous.IsValid())
                {
                    Previous->Next = Current->Next;
                }
                else
                {
                    Buckets[Index] = Current->Next;
                }
                Size--;
                return true;
            }
            Previous = Current;
            Current = Current->Next;
        }

        return false;
    }

    TArray<KeyType> GetKeys() const
    {
        TArray<KeyType> Keys;
        Keys.Reserve(Size);

        for (int32 i = 0; i < Capacity; ++i)
        {
            TSharedPtr<HashNode> Current = Buckets[i];
            while (Current.IsValid())
            {
                Keys.Add(Current->Key);
                Current = Current->Next;
            }
        }

        return Keys;
    }

    TArray<ValueType> GetValues() const
    {
        TArray<ValueType> Values;
        Values.Reserve(Size);

        for (int32 i = 0; i < Capacity; ++i)
        {
            TSharedPtr<HashNode> Current = Buckets[i];
            while (Current.IsValid())
            {
                Values.Add(Current->Value);
                Current = Current->Next;
            }
        }

        return Values;
    }

    void Clear()
    {
        Buckets.Empty();
        Buckets.SetNum(Capacity);
        Size = 0;
    }

    int32 GetSize() const { return Size; }
    bool IsEmpty() const { return Size == 0; }
    float GetLoadFactor() const { return Capacity > 0 ? (float)Size / Capacity : 0.0f; }
};