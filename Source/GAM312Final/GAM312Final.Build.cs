// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GAM312Final : ModuleRules
{
	public GAM312Final(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG", "Slate", "SlateCore", "NavigationSystem"});
	}
}
