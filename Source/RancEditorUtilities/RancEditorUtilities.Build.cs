// Copyright Rancorous Games, 2024

using UnrealBuildTool;

public class RancEditorUtilities : ModuleRules
{
    public RancEditorUtilities(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "RancUtilities", "BlueprintGraph",
                "UnrealEd", "KismetCompiler", "Kismet", "GraphEditor",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore"
            }
        );
    }
}