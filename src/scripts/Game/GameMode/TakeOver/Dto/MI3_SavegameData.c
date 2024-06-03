class MI3_SavegameData : Managed
{
	ref array<ref MI3_GarrisonData> m_GarrisonsData;
	ref map<string, ref MI3_PlayerData> m_PlayersData = new map<string, ref MI3_PlayerData>();
	
	bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid())
			return false;
		
		loadContext.ReadValue("garrisons", m_GarrisonsData);
		loadContext.ReadValue("players", m_PlayersData);
		
		return true;
	}
	
	bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid())
			return false;
		
		saveContext.WriteValue("garrisons", m_GarrisonsData);
		saveContext.WriteValue("players", m_PlayersData);
		
		return true;
	}
};
