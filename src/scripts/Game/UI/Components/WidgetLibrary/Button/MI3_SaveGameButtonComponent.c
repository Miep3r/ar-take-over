class MI3_SaveGameButtonComponent : SCR_ButtonBaseComponent
{
	protected ref TextWidget m_wTitle;
	
	protected string m_sTitle;
	
	override void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
		
		m_wTitle = TextWidget.Cast(m_wRoot.FindAnyWidget("Title"));
	}
	
	//----------------------------------------------------------------------
	void SetTitle(string title)
	{
		m_sTitle = title;
		m_wTitle.SetText(title);
	}
	
	string GetTitle()
	{
		return m_sTitle;
	}
}
