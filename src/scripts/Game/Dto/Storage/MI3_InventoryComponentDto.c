class MI3_InventoryComponentDto : Managed
{
	protected ResourceName m_PrefabResource;
	protected int m_iSlotIndex = -1;
	protected ref array<ref MI3_InventoryComponentDto> m_Storage = {};
	
	void SetPrefab(ResourceName prefab)
	{
		m_PrefabResource = prefab;
	}
	
	ResourceName GetPrefab()
	{
		return m_PrefabResource;
	}
	
	void SetSlotIndex(int slot)
	{
		m_iSlotIndex = slot;
	}
	
	int GetSlotIndex()
	{
		return m_iSlotIndex;
	}
	
	void AddStorageComponent(MI3_InventoryComponentDto component)
	{
		m_Storage.Insert(component);
	}
	
	void SetStorageComponents(array<ref MI3_InventoryComponentDto> components)
	{
		m_Storage = components;
	}
	
	array<ref MI3_InventoryComponentDto> GetStorageComponents()
	{
		return m_Storage;
	}
	
	bool HasStorage()
	{
		return m_Storage.Count() > 0;
	}
	
	// Serialization
	bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid())
			return false;
		
		saveContext.WriteValue("prefab", m_PrefabResource);
		saveContext.WriteValue("slot", m_iSlotIndex);
		
		if (m_Storage.Count() > 0)
			saveContext.WriteValue("storage", m_Storage);
		
		return true;
	}
	
	bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid())
			return false;
		
		Print("READING BASE COMPONENT - TD ENABLED: " + loadContext.IsTypeDiscriminatorEnabled());
		
		loadContext.ReadValue("prefab", m_PrefabResource);
		loadContext.ReadValue("slot", m_iSlotIndex);
		loadContext.ReadValue("storage", m_Storage);
		
		return true;
	}
}
