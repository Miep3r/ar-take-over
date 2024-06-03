class MI3_PlayerData : Managed
{
	ref MI3_InventoryDto m_Inventory;
	
	bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid())
			return false;
		
		loadContext.ReadValue("inventory", m_Inventory);
		
		return true;
	}
	
	bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid())
			return false;
		
		saveContext.WriteValue("inventory", m_Inventory);
		
		return true;
	}
};
