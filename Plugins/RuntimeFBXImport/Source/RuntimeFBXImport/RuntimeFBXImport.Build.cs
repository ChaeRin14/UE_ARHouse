// Some copyright should be here...

using System.IO;
using UnrealBuildTool;

public class RuntimeFBXImport : ModuleRules
{
	private string ThirdPartyPath
	{
		get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty/")); }
	}
	
	private void CopyToBinaries(string filePath, ReadOnlyTargetRules target)
	{
		string binariesDir = Path.Combine(target.ProjectFile!.Directory.FullName, "Binaries", target.Platform.ToString());
        
		string filename = Path.GetFileName(filePath);
		string destPath = Path.Combine(binariesDir, filename);

		if (!Directory.Exists(binariesDir))
		{
			Directory.CreateDirectory(binariesDir);
		}

		if (!File.Exists(destPath))
		{
			File.Copy(filePath, destPath, true);
		}
        
		RuntimeDependencies.Add(destPath);
		PublicDelayLoadDLLs.Add(Path.Combine(destPath));

	}
	public RuntimeFBXImport(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", "ProceduralMeshComponent",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore", "InputCore", "UMG", "AppFramework"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
	
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
		
		
		string fbxsdkDir =  Path.Combine(ThirdPartyPath, "FBX");
		PublicSystemIncludePaths.AddRange(
			new string[] {
				Path.Combine(fbxsdkDir , "include"),
				Path.Combine(fbxsdkDir , "include/fbxsdk"),
			}
		);
		
		

		if (target.Platform == UnrealTargetPlatform.Win64 )
		{
			string fbxdllDir = Path.Combine(fbxsdkDir, target.Platform.ToString(), "binaries");
			string fBxLibPath =Path.Combine(fbxsdkDir, target.Platform.ToString(), "lib");
			string fBxDllPath = Path.Combine(fbxdllDir , "libfbxsdk.dll");

            PublicAdditionalLibraries.Add(Path.Combine(fBxLibPath, "libfbxsdk.lib"));

            PublicDefinitions.Add("FBXSDK_SHARED");

           CopyToBinaries(fBxDllPath, target);

        }
	}
}
