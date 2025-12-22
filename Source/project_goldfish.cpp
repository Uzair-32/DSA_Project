// Copyright Epic Games, Inc. All Rights Reserved.

#include "project_goldfish.h"
#include "Modules/ModuleManager.h"
/**
 * Implements the primary game module for this project.
 * 
 * FDefaultGameModuleImpl provides default initialization for the game module.
 * The IMPLEMENT_PRIMARY_GAME_MODULE macro registers this module with the engine.
 * 
 * Parameters:
 * - FDefaultGameModuleImpl: Base implementation of the game module
 * - project_goldfish: Class name of this module
 * - "project_goldfish": Name string of the module used by the engine
 */
IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, project_goldfish, "project_goldfish" );
 