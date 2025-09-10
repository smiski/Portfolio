using UnrealBuildTool;

public class SnoozeTarget : TargetRules
{
	public SnoozeTarget(TargetInfo Target) : base(Target)
	{
		DefaultBuildSettings = BuildSettingsVersion.V2;
		Type = TargetType.Game;
		ExtraModuleNames.Add("Snooze");
	}
}
