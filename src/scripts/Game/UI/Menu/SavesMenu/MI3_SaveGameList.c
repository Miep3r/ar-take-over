class MI3_SaveGameList : SCR_ScriptedWidgetComponent
{
	private ResourceName m_sButtonLayout = "{DF12B5E2A0274391}UI/layouts/UntitledGamemode/SavesMenu/SaveGameButton.layout";
	
//	protected ref SCR_ButtonTextComponent m_wAddButton;
	protected ref SCR_ModularButtonComponent m_AddButtonHandler;
	protected ref VerticalLayoutWidget m_wItemList;
	protected ref map<MI3_SaveGameButtonComponent, string> m_mNames = new map<MI3_SaveGameButtonComponent, string>();
	
	ref ScriptInvoker m_OnSavegameCreate = new ScriptInvoker();
	ref ScriptInvoker m_OnSavegameLoad = new ScriptInvoker();
	
	override event void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
		
		ButtonWidget addButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("AddButton"));
		m_AddButtonHandler = SCR_ModularButtonComponent.FindComponent(addButton);
		
		if (m_AddButtonHandler)
			m_AddButtonHandler.m_OnClicked.Insert(OnAddAction);
		
		m_wItemList = VerticalLayoutWidget.Cast(m_wRoot.FindAnyWidget("ItemList"));
	}
	
	protected void OnAddAction(SCR_ModularButtonComponent comp)
	{
		Print("Add savegame activated!");
		
		DialogUI dialog = DialogUI.Cast(GetGame().GetMenuManager().OpenDialog(ChimeraMenuPreset.CreateSaveGameDialog));
		
		dialog.m_OnConfirm.Insert(OnCreateConfirm);
	}
	
	protected void OnCreateConfirm(string value)
	{
		Print("Dialog confirmed! Value: " + value);
		
		m_OnSavegameCreate.Invoke(value);
		
//		MI3_GameMode.GetInstance().CreateSavegame_C(value);
	}
	
	void AddSavegameItem(string name)
	{
		Widget item = GetGame().GetWorkspace().CreateWidgets(m_sButtonLayout, m_wItemList);
		
		MI3_SaveGameButtonComponent button = MI3_SaveGameButtonComponent.Cast(item.FindHandler(MI3_SaveGameButtonComponent));
		button.SetTitle(name);
		button.m_OnClicked.Insert(OnSavegameLoad);
	}
	
	void OnSavegameLoad(MI3_SaveGameButtonComponent c)
	{
		Print("Savegame load activated!");
		
		m_OnSavegameLoad.Invoke(c.GetTitle());
		
//		MI3_GameMode.GetInstance().LoadSavegame_C(c.GetTitle());
	}
}
