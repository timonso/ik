// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class demo_ik_ue4 : ModuleRules
{
	public demo_ik_ue4(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
