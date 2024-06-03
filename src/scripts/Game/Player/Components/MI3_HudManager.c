class MI3_HudManagerComponentClass : ScriptComponentClass
{
}

class MI3_HudManagerComponent : ScriptComponent
{
	[Attribute("")]
	private ResourceName m_sHudLayout;
	[Attribute("")]
	private float m_iUndercoverTextUnhideSpeed;
	[Attribute("")]
	private float m_iUndercoverTextHideSpeed;
	
	protected MI3_PlayerController m_Controller;
	
	protected ref Widget m_wRoot;
	protected ref RichTextWidget m_wUndercoverStateText;
	
	void UpdateUndercoverState(string state)
	{
		if (state == m_wUndercoverStateText.GetText())
		{
			return;
		}
		
		Print("Updating undercover state to: " + state);
		
		m_wUndercoverStateText.SetText(state);
		
		AnimateWidget.StopAnimation(m_wUndercoverStateText, WidgetAnimationOpacity);
		
		AnimateWidget.Opacity(m_wUndercoverStateText, 1, m_iUndercoverTextUnhideSpeed);
		
		GetGame().GetCallqueue().CallLater(HideUndercoverState, 3000);
	}
	
	void HideUndercoverState()
	{
		AnimateWidget.Opacity(m_wUndercoverStateText, 0, m_iUndercoverTextHideSpeed);
	}
	
	void Hide()
	{
		AnimateWidget.Opacity(m_wRoot, 0, 10);
	}
	
	void Show()
	{
		AnimateWidget.Opacity(m_wRoot, 1, 10);
	}
	
	void InitHud()
	{
		m_wRoot = GetGame().GetWorkspace().CreateWidgets(m_sHudLayout);
		m_wUndercoverStateText = RichTextWidget.Cast(m_wRoot.FindAnyWidget("UndercoverState"));
	}
	
	void MI3_HudManagerComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		m_Controller = MI3_PlayerController.Cast(ent);
		m_Controller.SetHudManager(this);
		
		InitHud();
		
		m_wUndercoverStateText.SetText("Concealed");
		m_wUndercoverStateText.SetOpacity(0);
	}
}
