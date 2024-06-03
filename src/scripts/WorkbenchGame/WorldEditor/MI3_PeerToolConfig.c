[BaseContainerProps()]
class MI3_GameModeServerCLI : DedicatedServerPluginCLI
{
	[Attribute()]
	protected string m_sAdminPassword;
	
	private ResourceName m_World = "{B6AA8FCD4A68DE26}worlds/ArlandTest/ArlandTest.ent";
	private ResourceName m_MissionHeader = "{BDA960976788F4C9}Missions/MainMission.conf";
	
	//------------------------------------------------------------------------------------------------
	//! \param[out] outParam
	//! \param[in] api
	//! \return
	override bool GetCLI(out string outParam, WorldEditorAPI api)
	{
		ResourceName worldPath = m_World;
		if (!worldPath)
			api.GetWorldPath(worldPath);

		outParam = string.Format("-server \"%1\" -MissionHeader \"%2\"", worldPath.GetPath(), m_MissionHeader.GetPath());

		if (m_sAdminPassword)
			outParam += string.Format(" -adminPassword %1", m_sAdminPassword);
		
		outParam += " -addonsDir \"C:\Users\oscho\Documents\My Games\ArmaReforgerWorkbench\addons\" -debuggerPort 42069";

		return !worldPath.IsEmpty();
	}
}
