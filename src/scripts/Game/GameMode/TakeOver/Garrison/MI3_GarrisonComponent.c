class MI3_GarrisonComponentClass : ScriptComponentClass
{
}

class MI3_GarrisonComponent : ScriptComponent
{
	protected MI3_Garrison m_Garrison;
	
	void MI3_GarrisonComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		m_Garrison = MI3_Garrison.Cast(ent);
		
		SetEventMask(ent, EntityEvent.INIT);
	}
	
	void ~MI3_GarrisonComponent()
	{
		m_Garrison = null;
	}
}
