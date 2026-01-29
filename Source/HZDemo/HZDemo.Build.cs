// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HZDemo : ModuleRules
{
	public HZDemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		OptimizeCode = CodeOptimization.Never;
        
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"MassEntity",
			"MassSpawner",
			"MassCommon",
			"MassNavigation",
			"MassMovement",
			"MassAIBehavior",
			"MassRepresentation",
			"MassSignals",
			"StateTreeModule",
			"SmartObjectsModule",
			"MassSmartObjects",
			"AnimToTexture",
			"Niagara"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"HZDemo",
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
