// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EOS : ModuleRules
{
    public EOS(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "OnlineSubsystem",
            "OnlineSubsystemUtils", "OnlineSubsystemEOS"
        });
    }
}