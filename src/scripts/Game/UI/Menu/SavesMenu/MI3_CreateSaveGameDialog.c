class MI3_CreateSaveGameDialogUI : DialogUI
{
	protected ref Widget m_wRoot;
	protected ref SCR_EditBoxComponent m_EditBox;
	
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		m_wRoot = GetRootWidget();
		
		m_EditBox = SCR_EditBoxComponent.GetEditBoxComponent("EditBox", m_wRoot);
	}
	
	override protected void OnConfirm()
	{
		m_OnConfirm.Invoke(m_EditBox.GetValue());
		Close();
	}
}
