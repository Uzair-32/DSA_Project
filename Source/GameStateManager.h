// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "CustomStack.h"
#include "CustomHashMap.h"
#include "GameStateManager.generated.h"

/**
 * FGameStateSnapshot:
 * Represents a complete snapshot of the game state at a given time.
 * Used for undo/redo functionality, quick saves, and state restoration.
 */
USTRUCT(BlueprintType)
struct FGameStateSnapshot
{
    GENERATED_BODY()

    // Player's current health
    UPROPERTY()
    int32 PlayerHealth;

    // Player's current points
    UPROPERTY()
    int32 PlayerPoints;

    // Current wave index in the game
    UPROPERTY()
    int32 CurrentWave;

    // Number of kills in the current wave
    UPROPERTY()
    int32 WaveKills;

    // Ammo in the currently equipped weapon
    UPROPERTY()
    int32 CurrentAmmo;

    // Ammo in reserve / holstered
    UPROPERTY()
    int32 HolsteredAmmo;

    // Positions of all active enemies in the level
    UPROPERTY()
    TArray<FVector> EnemyPositions;

    // Health values of all active enemies
    UPROPERTY()
    TArray<int32> EnemyHealthValues;

    // Timestamp used to identify this snapshot
    UPROPERTY()
    float Timestamp;

    // Default constructor initializing safe defaults
    FGameStateSnapshot()
        : PlayerHealth(100)
        , PlayerPoints(0)
        , CurrentWave(0)
        , WaveKills(0)
        , CurrentAmmo(0)
        , HolsteredAmmo(0)
        , Timestamp(0.0f)
    {
    }

    // Equality operator to compare snapshots based on timestamp
    bool operator==(const FGameStateSnapshot& Other) const
    {
        return FMath::IsNearlyEqual(Timestamp, Other.Timestamp, 0.01f);
    }
};

/**
 * UGameStateSaveGame:
 * Unreal SaveGame object for persisting a single game state snapshot to disk.
 */
UCLASS()
class PROJECT_GOLDFISH_API UGameStateSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    // Snapshot to be saved
    UPROPERTY()
    FGameStateSnapshot CurrentState;

    // Save slot name
    UPROPERTY()
    FString SaveSlotName;

    // Player index (default = 0)
    UPROPERTY()
    uint32 UserIndex;

    // Constructor with default slot name
    UGameStateSaveGame()
    {
        SaveSlotName = TEXT("GameStateSaveSlot");
        UserIndex = 0;
    }
};

/**
 * UGameStateManager:
 * 
 * Handles:
 * - Undo/Redo history using CustomStack (O(1) push/pop)
 * - Quick save/load caching using CustomHashMap (O(1) lookup)
 * - Save/load to disk via UE5 USaveGame
 * - Performance metrics tracking for saves and loads
 */
UCLASS(Blueprintable, BlueprintType)
class PROJECT_GOLDFISH_API UGameStateManager : public UObject
{
    GENERATED_BODY()

private:
    // Undo stack storing previous states
    CustomStack<FGameStateSnapshot> UndoStack;
    
    // Redo stack storing undone states
    CustomStack<FGameStateSnapshot> RedoStack;

    // Quick lookup cache for saved states
    CustomHashMap<FString, FGameStateSnapshot> StateCache;

    // Current active game state
    FGameStateSnapshot CurrentState;

    // Maximum number of undo states stored
    int32 MaxUndoHistory;

    // Performance metrics
    int32 TotalSaves;
    int32 TotalLoads;
    float AverageSaveTime;
    float AverageLoadTime;

public:
    // Constructor: initializes undo stack and metrics
    UGameStateManager()
        : MaxUndoHistory(50)
        , TotalSaves(0)
        , TotalLoads(0)
        , AverageSaveTime(0.0f)
        , AverageLoadTime(0.0f)
    {
        UndoStack.SetMaxCapacity(MaxUndoHistory);
    }

    /**
     * Capture the current game state.
     * Saves the previous state to the undo stack and clears the redo stack.
     * Time Complexity: O(1)
     */
    UFUNCTION(BlueprintCallable, Category = "Game State")
    void CaptureState(int32 PlayerHealth, int32 PlayerPoints, int32 CurrentWave,
                     int32 WaveKills, int32 CurrentAmmo, int32 HolsteredAmmo,
                     const TArray<FVector>& EnemyPositions, const TArray<int32>& EnemyHealthValues)
    {
        // Save previous state if it exists
        if (CurrentState.Timestamp > 0.0f)
        {
            UndoStack.Push(CurrentState);
        }

        // Update current state with new values
        CurrentState.PlayerHealth = PlayerHealth;
        CurrentState.PlayerPoints = PlayerPoints;
        CurrentState.CurrentWave = CurrentWave;
        CurrentState.WaveKills = WaveKills;
        CurrentState.CurrentAmmo = CurrentAmmo;
        CurrentState.HolsteredAmmo = HolsteredAmmo;
        CurrentState.EnemyPositions = EnemyPositions;
        CurrentState.EnemyHealthValues = EnemyHealthValues;
        CurrentState.Timestamp = FPlatformTime::Seconds();

        // Clear redo history on new state capture
        RedoStack.Clear();
    }

    /**
     * Undo to previous state.
     * Pops from UndoStack and pushes current state to RedoStack.
     * Time Complexity: O(1)
     */
    UFUNCTION(BlueprintCallable, Category = "Game State")
    bool Undo(FGameStateSnapshot& OutPreviousState)
    {
        FGameStateSnapshot PreviousState;
        if (UndoStack.Pop(PreviousState))
        {
            RedoStack.Push(CurrentState); // Save current state to redo stack
            CurrentState = PreviousState; // Restore previous state
            OutPreviousState = CurrentState;
            return true;
        }
        return false;
    }

    /**
     * Redo to next state.
     * Pops from RedoStack and pushes current state to UndoStack.
     * Time Complexity: O(1)
     */
    UFUNCTION(BlueprintCallable, Category = "Game State")
    bool Redo(FGameStateSnapshot& OutNextState)
    {
        FGameStateSnapshot NextState;
        if (RedoStack.Pop(NextState))
        {
            UndoStack.Push(CurrentState); // Save current state to undo stack
            CurrentState = NextState;     // Restore next state
            OutNextState = CurrentState;
            return true;
        }
        return false;
    }

    /**
     * Save the current game state to disk.
     * Also caches the saved state in memory.
     * Time Complexity: O(1) + disk I/O
     */
    UFUNCTION(BlueprintCallable, Category = "Game State")
    bool SaveGameState(const FString& SlotName = TEXT("QuickSave"))
    {
        double StartTime = FPlatformTime::Seconds();

        // Create a new SaveGame object
        UGameStateSaveGame* SaveGameInstance = Cast<UGameStateSaveGame>(
            UGameplayStatics::CreateSaveGameObject(UGameStateSaveGame::StaticClass())
        );

        if (SaveGameInstance)
        {
            SaveGameInstance->CurrentState = CurrentState;
            SaveGameInstance->SaveSlotName = SlotName;

            bool bSuccess = UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName, 0);

            if (bSuccess)
            {
                // Cache the state for fast future loads
                StateCache.Insert(SlotName, CurrentState);

                // Update metrics
                double EndTime = FPlatformTime::Seconds();
                float SaveTime = static_cast<float>(EndTime - StartTime);
                AverageSaveTime = (AverageSaveTime * TotalSaves + SaveTime) / (TotalSaves + 1);
                TotalSaves++;

                UE_LOG(LogTemp, Log, TEXT("Game saved to slot '%s' in %.4f seconds"), *SlotName, SaveTime);
            }

            return bSuccess;
        }

        return false;
    }

    /**
     * Load a saved game state from cache or disk.
     * Updates CurrentState and returns the loaded snapshot.
     * Time Complexity: O(1) + disk I/O
     */
    UFUNCTION(BlueprintCallable, Category = "Game State")
    bool LoadGameState(const FString& SlotName, FGameStateSnapshot& OutLoadedState)
    {
        double StartTime = FPlatformTime::Seconds();

        // Attempt to load from cache first
        FGameStateSnapshot CachedState;
        if (StateCache.Find(SlotName, CachedState))
        {
            CurrentState = CachedState;
            OutLoadedState = CurrentState;
            UE_LOG(LogTemp, Log, TEXT("Game state loaded from cache for slot '%s'"), *SlotName);
            return true;
        }

        // Fallback to loading from disk
        if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
        {
            UGameStateSaveGame* LoadedGame = Cast<UGameStateSaveGame>(
                UGameplayStatics::LoadGameFromSlot(SlotName, 0)
            );

            if (LoadedGame)
            {
                CurrentState = LoadedGame->CurrentState;
                OutLoadedState = CurrentState;

                // Update cache
                StateCache.Insert(SlotName, CurrentState);

                // Update performance metrics
                double EndTime = FPlatformTime::Seconds();
                float LoadTime = static_cast<float>(EndTime - StartTime);
                AverageLoadTime = (AverageLoadTime * TotalLoads + LoadTime) / (TotalLoads + 1);
                TotalLoads++;

                UE_LOG(LogTemp, Log, TEXT("Game loaded from slot '%s' in %.4f seconds"), *SlotName, LoadTime);
                return true;
            }
        }

        return false;
    }

    /** Delete a save slot from disk and cache */
    UFUNCTION(BlueprintCallable, Category = "Game State")
    bool DeleteSaveGame(const FString& SlotName)
    {
        if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
        {
            bool bSuccess = UGameplayStatics::DeleteGameInSlot(SlotName, 0);
            
            if (bSuccess)
            {
                StateCache.Remove(SlotName); // Remove from cache
            }
            
            return bSuccess;
        }
        return false;
    }

    /** Get all currently cached save slots */
    UFUNCTION(BlueprintCallable, Category = "Game State")
    TArray<FString> GetAvailableSaveSlots() const
    {
        return StateCache.GetKeys();
    }

    /** Returns a copy of the current game state */
    UFUNCTION(BlueprintPure, Category = "Game State")
    FGameStateSnapshot GetCurrentState() const
    {
        return CurrentState;
    }

    /** Checks whether undo is available */
    UFUNCTION(BlueprintPure, Category = "Game State")
    bool CanUndo() const
    {
        return !UndoStack.IsEmpty();
    }

    /** Checks whether redo is available */
    UFUNCTION(BlueprintPure, Category = "Game State")
    bool CanRedo() const
    {
        return !RedoStack.IsEmpty();
    }

    /** Returns the number of states in the undo stack */
    UFUNCTION(BlueprintPure, Category = "Game State")
    int32 GetUndoStackSize() const
    {
        return UndoStack.Size();
    }

    /** Returns the number of states in the redo stack */
    UFUNCTION(BlueprintPure, Category = "Game State")
    int32 GetRedoStackSize() const
    {
        return RedoStack.Size();
    }

    /** Clears all undo and redo history */
    UFUNCTION(BlueprintCallable, Category = "Game State")
    void ClearHistory()
    {
        UndoStack.Clear();
        RedoStack.Clear();
    }

    /** Retrieves performance metrics for debugging or UI display */
    UFUNCTION(BlueprintPure, Category = "Game State")
    void GetPerformanceMetrics(int32& OutTotalSaves, int32& OutTotalLoads,
                               float& OutAvgSaveTime, float& OutAvgLoadTime) const
    {
        OutTotalSaves = TotalSaves;
        OutTotalLoads = TotalLoads;
        OutAvgSaveTime = AverageSaveTime;
        OutAvgLoadTime = AverageLoadTime;
    }

    /** Returns cache statistics for monitoring memory and performance */
    UFUNCTION(BlueprintPure, Category = "Game State")
    void GetCacheStats(int32& OutCachedStates, float& OutCacheLoadFactor) const
    {
        OutCachedStates = StateCache.GetSize();
        OutCacheLoadFactor = StateCache.GetLoadFactor();
    }
};
