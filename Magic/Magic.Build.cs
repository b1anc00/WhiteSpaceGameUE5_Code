// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Magic : ModuleRules
{
	public Magic(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Slate", "Json", "JsonUtilities", "Niagara", "AIModule", "Slate", "SlateCore"});

		PrivateDependencyModuleNames.AddRange(new string[] { "SQLiteCore", "SQLiteSupport" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
