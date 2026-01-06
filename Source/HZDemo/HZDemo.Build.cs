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
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"HZDemo",
			"HZDemo/Variant_Platforming",
			"HZDemo/Variant_Platforming/Animation",
			"HZDemo/Variant_Combat",
			"HZDemo/Variant_Combat/AI",
			"HZDemo/Variant_Combat/Animation",
			"HZDemo/Variant_Combat/Gameplay",
			"HZDemo/Variant_Combat/Interfaces",
			"HZDemo/Variant_Combat/UI",
			"HZDemo/Variant_SideScrolling",
			"HZDemo/Variant_SideScrolling/AI",
			"HZDemo/Variant_SideScrolling/Gameplay",
			"HZDemo/Variant_SideScrolling/Interfaces",
			"HZDemo/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
