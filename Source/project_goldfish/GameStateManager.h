// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "CustomStack.h"
#include "CustomHashMap.h"
#include "GameStateManager.generated.h"

/**
 * Game State Snapshot for undo/redo functionality
 */
USTRUCT(BlueprintType)
struct FGameStateSnapshot
{
    GENERATED_BODY()

    UPROPERTY()
    int32 PlayerHealth;

    UPROPERTY()
    int32 PlayerPoints;

    UPROPERTY()
    int32 CurrentWave;

    UPROPERTY()
    int32 WaveKills;

    UPROPERTY()
    int32 CurrentAmmo;

    UPROPERTY()
    int32 HolsteredAmmo;

    UPROPERTY()
    TArray<FVector> EnemyPositions;

    UPROPERTY()
    TArray<int32> EnemyHealthValues;

    UPROPERTY()
    float Timestamp;

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

    bool operator==(const FGameStateSnapshot& Other) const
    {
        return FMath::IsNearlyEqual(Timestamp, Other.Timestamp, 0.01f);
    }
};

/**
 * Save Game Object
 */
UCLASS()
class PROJECT_GOLDFISH_API UGameStateSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY()
    FGameStateSnapshot CurrentState;

    UPROPERTY()
    FString SaveSlotName;

    UPROPERTY()
    uint32 UserIndex;

    UGameStateSaveGame()
    {
        SaveSlotName = TEXT("GameStateSaveSlot");
        UserIndex = 0;
    }
};

/**
 * GameStateManager:
 * Manages game state with save/load functionality and undo/redo system.
 * 
 * Data Structures Used:
 * - CustomStack: For undo/redo history (LIFO operations)
 * - CustomHashMap: For quick state lookups and caching
 * 
 * Time Complexity:
 * - Save State: O(1)
 * - Load State: O(1)
 * - Undo: O(1)
 * - Redo: O(1)
 * 
 * Space Complexity: O(k) where k is the undo history size limit
 */
UCLASS(Blueprintable, BlueprintType)
class PROJECT_GOLDFISH_API UGameStateManager : public UObject
{
    GENERATED_BODY()

private:
    // Undo stack - stores previous states
    CustomStack<FGameStateSnapshot> UndoStack;
    
    // Redo stack - stores undone states
    CustomStack<FGameStateSnapshot> RedoStack;

    // Quick lookup cache for named states
    CustomHashMap<FString, FGameStateSnapshot> StateCache;

    // Current state
    FGameStateSnapshot CurrentState;

    // Maximum undo history size
    int32 MaxUndoHistory;

    // Performance metrics
    int32 TotalSaves;
    int32 TotalLoads;
    float AverageSaveTime;
    float AverageLoadTime;

public:
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
     * Capture current game state - O(1)
     */
    UFUNCTION(BlueprintCallable, Category = "Game State")
    void CaptureState(int32 PlayerHealth, int32 PlayerPoints, int32 CurrentWave,
                     int32 WaveKills, int32 CurrentAmmo, int32 HolsteredAmmo,
                     const TArray<FVector>& EnemyPositions, const TArray<int32>& EnemyHealthValues)
    {
        // Save current state to undo stack before capturing new one
        if (CurrentState.Timestamp > 0.0f)
        {
            UndoStack.Push(CurrentState);
        }

        // Create new state
        CurrentState.PlayerHealth = PlayerHealth;
        CurrentState.PlayerPoints = PlayerPoints;
        CurrentState.CurrentWave = CurrentWave;
        CurrentState.WaveKills = WaveKills;
        CurrentState.CurrentAmmo = CurrentAmmo;
        CurrentState.HolsteredAmmo = HolsteredAmmo;
        CurrentState.EnemyPositions = EnemyPositions;
        CurrentState.EnemyHealthValues = EnemyHealthValues;
        CurrentState.Timestamp = FPlatformTime::Seconds();

        // Clear redo stack when new state is captured
        RedoStack.Clear();
    }

    /**
     * Undo to previous state - O(1)
     */
    UFUNCTION(BlueprintCallable, Category = "Game State")
    bool Undo(FGameStateSnapshot& OutPreviousState)
    {
        FGameStateSnapshot PreviousState;
        if (UndoStack.Pop(PreviousState))
        {
            // Push current state to redo stack
            RedoStack.Push(CurrentState);
            
            // Restore previous state
            CurrentState = PreviousState;
            OutPreviousState = CurrentState;
            
            return true;
        }

        return false;
    }

    /**
     * Redo to next state - O(1)
     */
    UFUNCTION(BlueprintCallable, Category = "Game State")
    bool Redo(FGameStateSnapshot& OutNextState)
    {
        FGameStateSnapshot NextState;
        if (RedoStack.Pop(NextState))
        {
            // Push current state to undo stack
            UndoStack.Push(CurrentState);
            
            // Restore next state
            CurrentState = NextState;
            OutNextState = CurrentState;
            
            return true;
        }

        return false;
    }

    /**
     * Save current state to disk - O(1) + disk I/O
     */
    UFUNCTION(BlueprintCallable, Category = "Game State")
    bool SaveGameState(const FString& SlotName = TEXT("QuickSave"))
    {
        double StartTime = FPlatformTime::Seconds();

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
                // Cache the state
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
     * Load state from disk - O(1) + disk I/O
     */
    UFUNCTION(BlueprintCallable, Category = "Game State")
    bool LoadGameState(const FString& SlotName, FGameStateSnapshot& OutLoadedState)
    {
        double StartTime = FPlatformTime::Seconds();

        // Check cache first - O(1)
        FGameStateSnapshot CachedState;
        if (StateCache.Find(SlotName, CachedState))
        {
            CurrentState = CachedState;
            OutLoadedState = CurrentState;
            
            UE_LOG(LogTemp, Log, TEXT("Game state loaded from cache for slot '%s'"), *SlotName);
            return true;
        }

        // Load from disk if not in cache
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

                // Update metrics
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

    /**
     * Delete save file
     */
    UFUNCTION(BlueprintCallable, Category = "Game State")
    bool DeleteSaveGame(const FString& SlotName)
    {
        if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
        {
            bool bSuccess = UGameplayStatics::DeleteGameInSlot(SlotName, 0);
            
            if (bSuccess)
            {
                StateCache.Remove(SlotName);
            }
            
            return bSuccess;
        }

        return false;
    }

    /**
     * Get available save slots
     */
    UFUNCTION(BlueprintCallable, Category = "Game State")
    TArray<FString> GetAvailableSaveSlots() const
    {
        return StateCache.GetKeys();
    }

    /**
     * Get current state
     */
    UFUNCTION(BlueprintPure, Category = "Game State")
    FGameStateSnapshot GetCurrentState() const
    {
        return CurrentState;
    }

    /**
     * Check if undo is available
     */
    UFUNCTION(BlueprintPure, Category = "Game State")
    bool CanUndo() const
    {
        return !UndoStack.IsEmpty();
    }

    /**
     * Check if redo is available
     */
    UFUNCTION(BlueprintPure, Category = "Game State")
    bool CanRedo() const
    {
        return !RedoStack.IsEmpty();
    }

    /**
     * Get undo stack size
     */
    UFUNCTION(BlueprintPure, Category = "Game State")
    int32 GetUndoStackSize() const
    {
        return UndoStack.Size();
    }

    /**
     * Get redo stack size
     */
    UFUNCTION(BlueprintPure, Category = "Game State")
    int32 GetRedoStackSize() const
    {
        return RedoStack.Size();
    }

    /**
     * Clear all history
     */
    UFUNCTION(BlueprintCallable, Category = "Game State")
    void ClearHistory()
    {
        UndoStack.Clear();
        RedoStack.Clear();
    }

    /**
     * Get performance metrics
     */
    UFUNCTION(BlueprintPure, Category = "Game State")
    void GetPerformanceMetrics(int32& OutTotalSaves, int32& OutTotalLoads,
                               float& OutAvgSaveTime, float& OutAvgLoadTime) const
    {
        OutTotalSaves = TotalSaves;
        OutTotalLoads = TotalLoads;
        OutAvgSaveTime = AverageSaveTime;
        OutAvgLoadTime = AverageLoadTime;
    }

    /**
     * Get cache statistics
     */
    UFUNCTION(BlueprintPure, Category = "Game State")
    void GetCacheStats(int32& OutCachedStates, float& OutCacheLoadFactor) const
    {
        OutCachedStates = StateCache.GetSize();
        OutCacheLoadFactor = StateCache.GetLoadFactor();
    }
};