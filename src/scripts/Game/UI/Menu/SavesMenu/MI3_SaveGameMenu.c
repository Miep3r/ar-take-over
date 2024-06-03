modded enum ChimeraMenuPreset
{
	SaveGameMenu,
	CreateSaveGameDialog
}

class MI3_SaveGameMenu : ChimeraMenuBase
{
	protected ref Widget m_wRoot;
	protected ref SCR_InputButtonComponent m_BackButton;
	protected ref MI3_SaveGameList m_SaveGameList;
	
	ref ScriptInvoker m_OnSavegameCreate = new ScriptInvoker();
	ref ScriptInvoker m_OnSavegameLoad = new ScriptInvoker();
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
	{
		Print("SaveGame menu opened!");
		
		m_wRoot = GetRootWidget();
		
		m_BackButton = SCR_InputButtonComponent.GetInputButtonComponent("Back", GetRootWidget());
		
		if (m_BackButton)
			m_BackButton.m_OnActivated.Insert(OnBack);
		
		m_SaveGameList = MI3_SaveGameList.Cast(SCR_WLibComponentBase.GetComponent(MI3_SaveGameList, "SaveGameList", m_wRoot));
		if (m_SaveGameList)
		{
			m_SaveGameList.m_OnSavegameLoad.Insert(OnSavegameLoad);
			m_SaveGameList.m_OnSavegameCreate.Insert(OnSavegameCreate);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnBack()
	{
		Print("Requesting close!");
		GameStateTransitions.RequestGameplayEndTransition();
	}
	
	void OnSavegameCreate(string name)
	{
		m_OnSavegameCreate.Invoke(name);
		Close();
	}
	
	void OnSavegameLoad(string name)
	{
		m_OnSavegameLoad.Invoke(name);
		Close();
	}
	
	//------------------------------------------------------------------------------------------------
	void AddServerSavegames(array<string> savegameNames)
	{
		foreach (string name : savegameNames)
		{
			m_SaveGameList.AddSavegameItem(name);
		}
	}
}
