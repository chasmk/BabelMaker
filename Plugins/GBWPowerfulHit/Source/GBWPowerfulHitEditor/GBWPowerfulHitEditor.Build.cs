// Copyright GanBowen 2022-2023. All Rights Reserved.

using UnrealBuildTool;

public class GBWPowerfulHitEditor : ModuleRules
{
	public GBWPowerfulHitEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Engine" });
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PrivateDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "AnimationModifiers", "AnimationBlueprintLibrary", "GBWPowerfulHit","UnrealEd"
		});

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new[]
			{
				"AnimGraph", "AnimGraphRuntime", "BlueprintGraph"
			});
		}
	}
}
