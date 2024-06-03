class MI3_SavegameMenuControllerComponentClass : MI3_PlayerControllerComponentClass
{
}

class MI3_SavegameMenuControllerComponent : MI3_PlayerControllerComponent
{
	protected ref ScriptInvoker Event_OnSavegameCreate = new ScriptInvoker();
	
	//--------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	private void OpenSavegameMenu_C(array<string> savegames)
	{
		MI3_SaveGameMenu menu = MI3_SaveGameMenu.Cast(GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.SaveGameMenu));
		
		menu.AddServerSavegames(savegames);
		menu.m_OnSavegameCreate.Insert(CreateSavegame);
		menu.m_OnSavegameLoad.Insert(LoadSavegame);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	private void CreateSavegame_S(string name)
	{
		Print("Received savegame create request for '" + name + "'");
		MI3_GameMode gm = MI3_GameMode.Cast(GetGame().GetGameMode());
		
		gm.CreateSavegame_S(name);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	private void LoadSavegame_S(string name)
	{
		Print("Received savegame load request for '" + name + "'");
		MI3_GameMode gm = MI3_GameMode.Cast(GetGame().GetGameMode());
		
		gm.LoadSavegame_S(name);
	}
	
	//--------------------------------------------------------------
	void OpenSavegameMenu(array<string> savegames)
	{
		Rpc(OpenSavegameMenu_C, savegames);
	}
	
	void CreateSavegame(string name)
	{
		Print("Triggering savegame creation");
		Rpc(CreateSavegame_S, name);
	}
	
	void LoadSavegame(string name)
	{
		Print("Triggering savegame load");
		Rpc(LoadSavegame_S, name);
	}
	
	//--------------------------------------------------------------
	void MI3_SavegameMenuControllerComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		m_Controller.SetSavegameMenuController(this);
	}
}
