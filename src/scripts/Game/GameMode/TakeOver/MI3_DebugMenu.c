class MI3_GarrisonSubMenu
{
	
	void MI3_GarrisonSubMenu(MI3_Garrison garrison)
	{
	}
}

class MI3_DebugMenu
{
	static bool m_bShowGarrisonDebug = false;

	static void Render(MI3_GarrisonManagerComponent garrisonManager)
	{
		if (!DiagMenu.GetBool(SCR_DebugMenuID.DEBUGUI_MI3_DO_DEBUG))
			return;
		
		// Menu begin
		DbgUI.Begin("MI3 Debug Menu");
		
		DbgUI.Check("Garrison debug", m_bShowGarrisonDebug);
		
		if (m_bShowGarrisonDebug && garrisonManager)
		{
			RenderGarrisons(garrisonManager);
		}
		
		DbgUI.End();
		// Menu end
	}
	
	private static void RenderGarrisons(MI3_GarrisonManagerComponent garrisonManager)
	{
		array<MI3_Garrison> garrisons = {};
		garrisonManager.GetGarrisons(garrisons);
		
		DbgUI.Begin("Garrisons");
		
		foreach (MI3_Garrison garrison : garrisons)
		{
			RenderGarrison(garrison);
		}
		
		DbgUI.End();
	}
	
	private static void RenderGarrison(MI3_Garrison garrison)
	{
		DbgUI.Spacer(10);
		DbgUI.Text("----------------------");
		DbgUI.Text("Garrison " + garrison.GetGarrisonName() + " (" + garrison.GetFactionKey() + ")");
		DbgUI.Text("Unit count: " + garrison.GetUnitCount());
	}
}
