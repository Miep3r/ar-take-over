class MI3_MuzzleComponentDto : Managed
{
	protected ref MI3_MagazineComponentDto m_Magazine;
	protected bool m_bIsChambered;
	
	void SetMagazine(MI3_MagazineComponentDto magazine)
	{
		m_Magazine = magazine;
	}
	
	MI3_MagazineComponentDto GetMagazine()
	{
		return m_Magazine;
	}
	
	void SetChambered(bool state)
	{
		m_bIsChambered = state;
	}
	
	bool IsChambered()
	{
		return m_bIsChambered;
	}
	
	bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid())
			return false;
		
		saveContext.WriteValue("mag", m_Magazine);
		saveContext.WriteValue("chambered", m_bIsChambered);
		
		return true;
	}
	
	bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid())
			return false;
		
		loadContext.ReadValue("mag", m_Magazine);
		loadContext.ReadValue("chambered", m_bIsChambered);
		
		return true;
	}
}
