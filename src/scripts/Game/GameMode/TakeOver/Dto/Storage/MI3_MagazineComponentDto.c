class MI3_MagazineComponentDto : MI3_InventoryComponentDto
{
	protected int m_iAmmo;
	
	void SetAmmo(int ammo)
	{
		m_iAmmo = ammo;
	}
	
	int GetAmmo()
	{
		return m_iAmmo;
	}
	
	override bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		bool success = super.SerializationSave(saveContext);
		
		if (!success)
			return false;
		
		saveContext.WriteValue("ammo", m_iAmmo);
		
		return true;
	}
	
	override bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		bool success = super.SerializationLoad(loadContext);
		
		if (!success)
			return false;
		
		loadContext.ReadValue("ammo", m_iAmmo);
		
		return true;
	}
}
