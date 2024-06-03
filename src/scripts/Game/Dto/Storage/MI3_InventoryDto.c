class MI3_InventoryDto : Managed
{
	protected int m_iTest = 0;
	protected ref array<ref MI3_InventoryComponentDto> m_aBaseComponents = {};
	protected ref array<ref MI3_WeaponComponentDto> m_aWeaponComponents = {};
	
	void SetBaseComponents(array<ref MI3_InventoryComponentDto> baseComponents)
	{
		m_aBaseComponents = baseComponents;
	}
	
	array<ref MI3_InventoryComponentDto> GetBaseComponents()
	{
		return m_aBaseComponents;
	}
	
	void SetWeaponComponents(array<ref MI3_WeaponComponentDto> weaponComponents)
	{
		m_aWeaponComponents = weaponComponents;
	}
	
	array<ref MI3_WeaponComponentDto> GetWeaponComponents()
	{
		return m_aWeaponComponents;
	}
	
	bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid())
			return false;
		
		loadContext.EnableTypeDiscriminator();
		loadContext.ReadValue("base", m_aBaseComponents);
		loadContext.ReadValue("weapons", m_aWeaponComponents);
		loadContext.EnableTypeDiscriminator(string.Empty);
		
		return true;
	}
	
	bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid())
			return false;
		
		saveContext.WriteValue("base", m_aBaseComponents);
		saveContext.WriteValue("weapons", m_aWeaponComponents);
		
		return true;
	}
}
