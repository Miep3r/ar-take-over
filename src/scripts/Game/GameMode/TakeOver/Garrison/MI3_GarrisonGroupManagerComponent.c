class MI3_GarrisonGroupData : Managed
{
	protected ref array<ResourceName> m_aUnits;
	
	//------------------------------------
	void SetUnits(array<ResourceName> units)
	{
		m_aUnits = units;
	}
	
	array<ResourceName> GetUnits()
	{
		return m_aUnits;
	}
	
	//------------------------------------
	bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid())
			return false;
		
		loadContext.ReadValue("units", m_aUnits);
		
		return true;
	}
	
	bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid())
			return false;
		
		saveContext.WriteValue("units", m_aUnits);
		
		return true;
	}
	
	//------------------------------------
	void MI3_GarrisonGroupData()
	{
		m_aUnits = {};
	}
	
	//------------------------------------
	static array<ref MI3_GarrisonGroupData> Create(MI3_GarrisonGroupManagerComponent gm)
	{	
		array<ref MI3_GarrisonGroupData> groupData = {};
		
		array<SCR_AIGroup> groups = {};
		gm.GetGroups(groups);
		
		foreach (SCR_AIGroup group : groups)
		{
			if (!group)
				continue;
			
			MI3_GarrisonGroupData data = new MI3_GarrisonGroupData();
			
			array<AIAgent> aiAgents = {};
			group.GetAgents(aiAgents);
			
			array<ResourceName> resourceNames = {};
			foreach (AIAgent agent : aiAgents)
			{
				resourceNames.Insert(agent.GetControlledEntity().GetPrefabData().GetPrefabName());
			}
			
			data.SetUnits(resourceNames);
			
			groupData.Insert(data);
		}
		
		Print("Group data count: " + groupData.Count());
		return groupData;
	}
}

enum MI3_EGarrisonGroupType
{
	FIRETEAM,
	MACHINEGUNS,
	ANTITANK,
	RECON,
	MEDIC
}

typedef func MI3_GarrisonGroupDeletedCallback;
void MI3_GarrisonGroupDeletedCallback(MI3_GarrisonGroupManagerComponent groupManager, SCR_AIGroup group);
typedef ScriptInvokerBase<MI3_GarrisonGroupDeletedCallback> MI3_GarrisonGroupDeletedInvoker;

class MI3_GarrisonGroupManagerComponentClass : MI3_GarrisonComponentClass
{
}

class MI3_GarrisonGroupManagerComponent : MI3_GarrisonComponent
{
	[Attribute("0", UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(MI3_EGarrisonGroupType), category: "Group Management")]
	protected ref array<MI3_EGarrisonGroupType> m_aInitialGroups;
	[Attribute(category: "Group Management")]
	protected ResourceName m_sDefendWaypointPrefab;
	[Attribute(params: "et", category: "Group Management")]
	protected ResourceName m_sAiGroupPrefab;
	
	protected MI3_GarrisonManagerComponent m_GarrisonManager;
	protected ref array<SCR_AIGroup> m_aAiGroups;
	
	protected ref MI3_GarrisonGroupDeletedInvoker Event_OnGroupDeleted = new MI3_GarrisonGroupDeletedInvoker();
	protected ref ScriptInvoker Event_OnEmpty = new ScriptInvoker();
	
	//-------------------------------------------------------------------
	MI3_GarrisonGroupDeletedInvoker GetOnGroupDeleted()
	{
		return Event_OnGroupDeleted;
	}
	
	ScriptInvoker GetOnEmpty()
	{
		return Event_OnEmpty;
	}
	
	//-------------------------------------------------------------------
	void SpawnAllGroups(array<ref MI3_GarrisonGroupData> groupData)
	{
		foreach (MI3_GarrisonGroupData group : groupData)
		{
			SCR_AIGroup aiGroup = m_Garrison.GetSpawnHandler().SpawnGroup(m_sAiGroupPrefab, group.GetUnits());
			
			if (!aiGroup)
				continue;
			
			m_aAiGroups.Insert(aiGroup);
			
			AddEventHandlersForGroup(aiGroup);
			AddDefendWaypointForGroup(aiGroup);
		}
	}
	
	void SpawnAllGroupsInitial()
	{
		foreach (MI3_EGarrisonGroupType type : m_aInitialGroups)
		{
			SpawnGroup(type);
		}
	}
	
	bool SpawnGroup(MI3_EGarrisonGroupType type)
	{
		MI3_GarrisonManagerComponent gm = MI3_GarrisonManagerComponent.GetInstance();
		
		if (!gm)
			return false;
		
		MI3_GarrisonFactionConfig config = gm.GetFactionConfigByKey(m_Garrison.GetFactionKey());
		
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
		
		SCR_AIGroup aiGroup = m_Garrison.GetSpawnHandler().SpawnGroup(m_sAiGroupPrefab, prefabsToSpawn);
		
		if (!aiGroup)
			return false;
		
		m_aAiGroups.Insert(aiGroup);
		
		AddEventHandlersForGroup(aiGroup);
		AddDefendWaypointForGroup(aiGroup);
		
		return true;
	}
	
	void DespawnAll()
	{
		Print("Despawning units!");
		
		MI3_GameMode gm = MI3_GameMode.Cast(GetGame().GetGameMode());
		gm.OnGarrisonGroupsDelete(m_aAiGroups);
		
		foreach (SCR_AIGroup group : m_aAiGroups)
		{
			delete group;
		}
		
		m_aAiGroups.Clear();
	}
	
	array<MI3_EGarrisonGroupType> GetInitialGroupTypes()
	{
		return m_aInitialGroups;
	}
	
	//------------------------------------------------------------------------
	protected void OnGroupEmpty(SCR_AIGroup group)
	{
		m_aAiGroups.RemoveItem(group);
		
		Event_OnGroupDeleted.Invoke(this, group);
		
		delete group;
		
		if (m_aAiGroups.Count() <= 0)
		{
			Event_OnEmpty.Invoke(this);
		}
	}
	
	//------------------------------------------------------------------------
	private void DeleteGroupUnits(SCR_AIGroup group)
	{
		array<SCR_ChimeraCharacter> members = group.GetAIMembers();
		foreach (SCR_ChimeraCharacter character : members)
		{
			delete character;
		}
	}
	
	private void AddDefendWaypointForGroup(SCR_AIGroup group)
	{
		vector mat[4];
		group.GetTransform(mat);
		
		EntitySpawnParams params = new EntitySpawnParams();
		params.Transform = mat;
		
		IEntity wpEnt = GetGame().SpawnEntityPrefab(Resource.Load(m_sDefendWaypointPrefab), GetOwner().GetWorld(), params);
		
		SCR_DefendWaypoint wp = SCR_DefendWaypoint.Cast(wpEnt);
		
		group.AddWaypoint(wp);
		Print("Added defend waypoint!", LogLevel.DEBUG);
	}
	
	private void AddEventHandlersForGroup(SCR_AIGroup group)
	{
		group.GetOnEmpty().Insert(OnGroupEmpty);
		
		SCR_AIGroupUtilityComponent groupUtility = SCR_AIGroupUtilityComponent.Cast(group.FindComponent(SCR_AIGroupUtilityComponent));
		
		if (groupUtility)
		{
			MI3_GameMode gm = MI3_GameMode.Cast(GetGame().GetGameMode());
			
			groupUtility.m_Perception.GetOnEnemyDetected().Insert(gm.OnEnemyDetectedGlobal);
			groupUtility.m_Perception.GetOnEnemyForgot().Insert(gm.OnEnemyForgotGlobal);
		}
	}
	
	//------------------------------------------------------------------------
	int GetUnitCount()
	{
		int count = 0;
		
		foreach (SCR_AIGroup aiGroup : m_aAiGroups)
		{
			if (aiGroup)
				count += aiGroup.GetAgentsCount();
		}
		
		return count;
	}
	
	void GetGroups(out array<SCR_AIGroup> out_groups)
	{
		out_groups.InsertAll(m_aAiGroups);
	}
	
	//-------------------------------------------
	void MI3_GarrisonGroupManagerComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		m_aAiGroups = {};
		
		Print("Garrison: " + m_Garrison.GetGarrisonName());
	}
}
