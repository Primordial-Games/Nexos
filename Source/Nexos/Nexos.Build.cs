// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class Nexos : ModuleRules
{
    public Nexos(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivatePCHHeaderFile = "Public/Nexos.h";
        bUseRTTI = false;
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
                new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "UMG", 
                "Networking",
                "Sockets",
                "HTTP",
                "PlayFab",
                "PlayFabCpp",
                "OnlineSubsystem",
                "OnlineSubsystemUtils",
                "OnlineSubsystemSteam",
                "AssetRegistry",
                "NavigationSystem",
                "AIModule",
                "GameplayAbilities",
                "GameplayTags",
                "GameplayTasks",
                "Gauntlet",
                }
            );

        PrivateDependencyModuleNames.AddRange(
                new string[] {
                "InputCore",
                "Slate",
                "SlateCore",
                "Json",
                "ApplicationCore",
                "ReplicationGraph",
                "PakFile",
                "RHI",
                "PlayFab",
                "PlayFabCpp",
                "PlayFabCommon",
                "VivoxCore"
                }
            );

        DynamicallyLoadedModuleNames.AddRange(
            new string[] {
                "OnlineSubsystemNull",
                "NetworkReplayStreaming",
                "NullNetworkReplayStreaming",
                "HttpNetworkReplayStreaming",
                "LocalFileNetworkReplayStreaming"
                }
            );

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
                "NetworkReplayStreaming"
                }
            );

        if (Target.bBuildDeveloperTools || (Target.Configuration != UnrealTargetConfiguration.Shipping && Target.Configuration != UnrealTargetConfiguration.Test))
        {
            PrivateDependencyModuleNames.Add("GameplayDebugger");
            PublicDefinitions.Add("WITH_GAMEPLAY_DEBUGGER=1");
        }
        else
        {
            PublicDefinitions.Add("WITH_GAMEPLAY_DEBUGGER=0");
        }

        if (Target.bGenerateProjectFiles || (Target.Type != TargetType.Game && Target.Type != TargetType.Client))
        {
            PublicDefinitions.Add("WITH_PFP_OSS_COGNITIVESERVICES=0");
        }
        else
        {
            PublicDefinitions.Add("WITH_PFP_OSS_COGNITIVESERVICES=1");

            //PublicDependencyModuleNames.Add("OnlineSubsystemPlayFabParty");
            PrivateDependencyModuleNames.Add("HTTP");
        }

        if (Target.Type == global::UnrealBuildTool.TargetType.Server)
        {
            bEnableExceptions = true;

            // Add dynamic dlls required by all dedicated servers
            RuntimeDependencies.Add("$(TargetOutputDir)/xinput1_3.dll", "$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies/Win64/DirectX/xinput1_3.dll", StagedFileType.SystemNonUFS);
            RuntimeDependencies.Add("$(TargetOutputDir)/concrt140.dll", "$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies/Win64/Microsoft.VC.CRT/concrt140.dll", StagedFileType.SystemNonUFS);
            RuntimeDependencies.Add("$(TargetOutputDir)/msvcp140.dll", "$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies/Win64/Microsoft.VC.CRT/msvcp140.dll", StagedFileType.SystemNonUFS);
            RuntimeDependencies.Add("$(TargetOutputDir)/msvcp140_1.dll", "$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies/Win64/Microsoft.VC.CRT/msvcp140_1.dll", StagedFileType.SystemNonUFS);
            RuntimeDependencies.Add("$(TargetOutputDir)/msvcp140_2.dll", "$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies/Win64/Microsoft.VC.CRT/msvcp140_2.dll", StagedFileType.SystemNonUFS);
            RuntimeDependencies.Add("$(TargetOutputDir)/vccorlib140.dll", "$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies/Win64/Microsoft.VC.CRT/vccorlib140.dll", StagedFileType.SystemNonUFS);
            RuntimeDependencies.Add("$(TargetOutputDir)/vcruntime140_1.dll", "$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies/Win64/Microsoft.VC.CRT/vcruntime140_1.dll", StagedFileType.SystemNonUFS);
            
            // Add dynamic dlls required by GSDK
            RuntimeDependencies.Add("$(TargetOutputDir)/GSDK_CPP_Windows.lib", "$(ProjectDir)/packages/com.playfab.cppgsdk.v140.0.7.200221/build/native/lib/Windows/x64/Release/dynamic/GSDK_CPP_Windows.lib", StagedFileType.SystemNonUFS);
            RuntimeDependencies.Add("$(TargetOutputDir)/libcurl.lib", "$(ProjectDir)/packages/com.playfab.cppgsdk.v140.0.7.200221/build/native/lib/Windows/x64/Release/dynamic/libcurl.lib", StagedFileType.SystemNonUFS);
            RuntimeDependencies.Add("$(TargetOutputDir)/libcurl.dll", "$(ProjectDir)/packages/com.playfab.cppgsdk.v140.0.7.200221/build/native/lib/Windows/x64/Release/dynamic/libcurl.dll", StagedFileType.SystemNonUFS);
            RuntimeDependencies.Add("$(TargetOutputDir)/libssl-1_1-x64.dll", "$(ProjectDir)/packages/com.playfab.cppgsdk.v140.0.7.200221/build/native/lib/Windows/x64/Release/dynamic/libssl-1_1-x64.dll", StagedFileType.SystemNonUFS);
            RuntimeDependencies.Add("$(TargetOutputDir)/libcrypto-1_1-x64.dll", "$(ProjectDir)/packages/com.playfab.cppgsdk.v140.0.7.200221/build/native/lib/Windows/x64/Release/dynamic/libcrypto-1_1-x64.dll", StagedFileType.SystemNonUFS);

            // Add libraries required by GSDK
            PublicAdditionalLibraries.Add("$(ProjectDir)/packages/com.playfab.cppgsdk.v140.0.7.200221/build/native/lib/Windows/x64/Release/dynamic/GSDK_CPP_Windows.lib");
            PublicAdditionalLibraries.Add("$(ProjectDir)/packages/com.playfab.cppgsdk.v140.0.7.200221/build/native/lib/Windows/x64/Release/dynamic/libcurl.lib");
            PublicIncludePaths.Add("../packages/com.playfab.cppgsdk.v140.0.7.200221/build/native/include");
        }

        var EngineDir = Path.GetFullPath(Target.RelativeEnginePath);
        PrivateIncludePaths.AddRange(
            new string[] {
                            Path.Combine(EngineDir, @"Plugins\Online\OnlineSubsystemSteam\Source\Private")
                         });
    }
}
