#pragma once

#include "Runtime/Core/Public/UObject/NameTypes.h"
#include "Runtime/Core/Public/Containers/UnrealString.h"

/**
 * Namespace: EnemyKeys
 *
 * Centralized collection of Blackboard key names used by enemy AI.
 * Defining keys as constants helps avoid hard-coded strings, reduces
 * runtime errors due to typos, and improves maintainability.
 */
namespace EnemyKeys
{
    /**
     * Blackboard key representing the world-space location
     * that the enemy AI should move toward or focus on.
     */
    TCHAR const* const TargetLocation = TEXT("TargetLocation");

    /**
     * Blackboard key indicating whether the player is currently
     * within the enemy's detection or attack range.
     * Typically updated by perception or distance-check services.
     */
    TCHAR const* const IsPlayerInRange = TEXT("IsPlayerInRange");
}
