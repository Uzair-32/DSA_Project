// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HealthInterface.generated.h"

// Unreal UInterface for health management.
UINTERFACE(MinimalAPI)
class UHealthInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IHealthInterface:
 * Interface for any actor or object that can take damage or be healed.
 * Classes implementing this interface must define how damage and healing are applied.
 */
class PROJECT_GOLDFISH_API IHealthInterface
{
	GENERATED_BODY()

public:
	/**
	 * ReceiveDamage:
	 * Apply damage to this object.
	 * @param amount - amount of health to reduce.
	 */
	virtual void ReceiveDamage(int amount);
	/**
	 * RecoverHealth:
	 * Heal this object by a given amount.
	 * @param amount - amount of health to restore.
	 */
	virtual void RecoverHealth(int amount);
public:
};
