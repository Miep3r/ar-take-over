class MI3_PlayerControllerComponentClass : ScriptComponentClass
{
}

class MI3_PlayerControllerComponent : ScriptComponent
{
	protected MI3_PlayerController m_Controller;
	
	void EOnUpdate(float timeSlice)
	{
	}
	
	void MI3_PlayerControllerComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		m_Controller = MI3_PlayerController.Cast(ent);
		m_Controller.RegisterComponent(this);
		
		Print("PlayerController Component constructed!");
	}
}
