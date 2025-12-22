
#pragma once

#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"

/**
 * CustomHashMap:
 * A custom hash map implementation for demonstrating data structure knowledge.
 * Uses separate chaining (linked lists) to handle hash collisions.
 * * Time Complexity:
 * - Insert: O(1) average, O(n) worst case (if many collisions occur)
 * - Search: O(1) average, O(n) worst case
 * - Delete: O(1) average, O(n) worst case
 * * Space Complexity: O(n) where n is the number of elements
 * * Use Case: Fast lookup of enemies by ID in the EnemyDirectorEnhanced.
 */
template<typename KeyType, typename ValueType>
class PROJECT_GOLDFISH_API CustomHashMap
{
private:
    // Node structure for chaining mechanism.
    // If two keys hash to the same bucket, they form a linked list using 'Next'.
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

    // The main array of buckets. Each bucket holds a pointer to the head of a linked list (HashNode).
    TArray<TSharedPtr<HashNode>> Buckets;
    int32 Size;
    int32 Capacity;
    float LoadFactor; // Threshold to trigger a resize (Rehash).

    // Generates a bucket index from a Key.
    int32 Hash(const KeyType& Key) const
    {
        if (Capacity == 0) return 0;
        // Use Unreal's built-in hashing for generic types.
        uint32 HashValue = GetTypeHash(Key);
        // Modulo operator maps the hash to a valid array index.
        return HashValue % Capacity;
    }

    // Increases the bucket array size and redistributes elements to maintain O(1) access time.
    void Rehash()
    {
        int32 OldCapacity = Capacity;
        Capacity *= 2; // Double the capacity.
        TArray<TSharedPtr<HashNode>> OldBuckets = Buckets;
        
        // Reset current storage.
        Buckets.Empty();
        Buckets.SetNum(Capacity);
        Size = 0;

        // Iterate through old buckets and re-insert every node into the new, larger array.
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
    // Constructor: Sets initial size and load factor threshold.
    CustomHashMap(int32 InitialCapacity = 16)
        : Size(0), Capacity(InitialCapacity), LoadFactor(0.75f)
    {
        Buckets.SetNum(Capacity);
    }

    // Insert or Update a key-value pair.
    void Insert(const KeyType& Key, const ValueType& Value)
    {
        // Check if we need to expand the map to prevent high collision rates.
        if ((float)Size / Capacity > LoadFactor)
        {
            Rehash();
        }

        int32 Index = Hash(Key);
        TSharedPtr<HashNode> Current = Buckets[Index];

        // Check if key already exists in this bucket chain; update if found.
        while (Current.IsValid())
        {
            if (Current->Key == Key)
            {
                Current->Value = Value;
                return;
            }
            Current = Current->Next;
        }

        // Key not found: Create new node and insert at head of the bucket (Separate Chaining).
        TSharedPtr<HashNode> NewNode = MakeShared<HashNode>(Key, Value);
        NewNode->Next = Buckets[Index];
        Buckets[Index] = NewNode;
        Size++;
    }

    // Retrieve a value by Key. Returns true if found.
    bool Find(const KeyType& Key, ValueType& OutValue) const
    {
        if (Capacity == 0) return false;
        
        int32 Index = Hash(Key);
        TSharedPtr<HashNode> Current = Buckets[Index];

        // Traverse the linked list in this bucket.
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

    // Check if a Key exists.
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

    // Delete a key-value pair.
    bool Remove(const KeyType& Key)
    {
        if (Capacity == 0) return false;
        
        int32 Index = Hash(Key);
        TSharedPtr<HashNode> Current = Buckets[Index];
        TSharedPtr<HashNode> Previous = nullptr;

        // Traverse list to find the node to remove.
        while (Current.IsValid())
        {
            if (Current->Key == Key)
            {
                // Unlink the node.
                if (Previous.IsValid())
                {
                    Previous->Next = Current->Next;
                }
                else
                {
                    // If it's the head of the list, update the bucket pointer.
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

    // Utility: Return all keys.
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

    // Utility: Return all values.
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

    // Reset the map.
    void Clear()
    {
        Buckets.Empty();
        Buckets.SetNum(Capacity);
        Size = 0;
    }

    // Getters for map state.
    int32 GetSize() const { return Size; }
    bool IsEmpty() const { return Size == 0; }
    float GetLoadFactor() const { return Capacity > 0 ? (float)Size / Capacity : 0.0f; }
};