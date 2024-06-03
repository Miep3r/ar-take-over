class MI3_GarrisonDataUtility
{
	
	static MI3_GarrisonData GetInitialDataForGarrison(MI3_Garrison garrison)
	{
		MI3_GarrisonData garrisonData = new MI3_GarrisonData();
		
		garrisonData.SetName(garrison.GetGarrisonName());
		
		array<ref MI3_GarrisonGroupData> groupsData = CreateInitialGroupData(garrison.GetFactionKey(), garrison.GetGroupManager().GetInitialGroupTypes());
		
		garrisonData.SetGroups(groupsData);
		
		return garrisonData;
	}
	
	private static array<ref MI3_GarrisonGroupData> CreateInitialGroupData(FactionKey factionKey, array<MI3_EGarrisonGroupType> groupTypes)
	{
		MI3_GarrisonManagerComponent gm = MI3_GarrisonManagerComponent.GetInstance();
		
		if (!gm)
			return null;
		
		MI3_GarrisonFactionConfig factionConfig = gm.GetFactionConfigByKey(factionKey);
		
		array<ref MI3_GarrisonGroupData> groups = {};
		foreach (MI3_EGarrisonGroupType type : groupTypes)
		{
			MI3_GarrisonGroupData gd = GetInitialGroupPrefabs(factionConfig, type);
			
			groups.Insert(gd);
		}
		
		return groups;
	}
	
	private static MI3_GarrisonGroupData GetInitialGroupPrefabs(MI3_GarrisonFactionConfig config, MI3_EGarrisonGroupType type)
	{	
		array<ResourceName> prefabsToSpawn = {config.GetSquadLeader()};
		switch (type)
		{
			case MI3_EGarrisonGroupType.FIRETEAM:
				prefabsToSpawn.Insert(config.GetRifleman());
				prefabsToSpawn.Insert(config.GetGrenadier());
				prefabsToSpawn.Insert(config.GetMachineGunner());
				prefabsToSpawn.Insert(config.GetAntiTank());
				prefabsToSpawn.Insert(config.GetMedic());
				break;
			case MI3_EGarrisonGroupType.MACHINEGUNS:
				prefabsToSpawn.Insert(config.GetMachineGunner());
				prefabsToSpawn.Insert(config.GetMachineGunner());
				break;
			case MI3_EGarrisonGroupType.ANTITANK:
				prefabsToSpawn.Insert(config.GetAntiTank());
				break;
			case MI3_EGarrisonGroupType.RECON:
				prefabsToSpawn.Insert(config.GetSharpShooter());
				break;
			case MI3_EGarrisonGroupType.MEDIC:
				prefabsToSpawn.Insert(config.GetMedic());
				prefabsToSpawn.Insert(config.GetMedic());
				break;
		}
		
		MI3_GarrisonGroupData groupData = new MI3_GarrisonGroupData();
		groupData.SetUnits(prefabsToSpawn);
		
		return groupData;
	}
}
