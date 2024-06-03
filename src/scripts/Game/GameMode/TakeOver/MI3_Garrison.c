class MI3_GarrisonData : Managed
{
	protected string m_sGarrison;
	protected bool m_bIsDiscovered;
	protected ref array<ref MI3_GarrisonGroupData> m_aGroups;
	
	//------------------------------------
	void SetName(string name)
	{
		m_sGarrison = name;
	}
	
	string GetName()
	{
		return m_sGarrison;
	}
	
	//------------------------------------
	void SetDiscovered(bool state)
	{
		m_bIsDiscovered = state;
	}
	
	bool IsDiscovered()
	{
		return m_bIsDiscovered;
	}
	
	//------------------------------------
	void SetGroups(array<ref MI3_GarrisonGroupData> groups)
	{
		m_aGroups = groups;
	}
	
	array<ref MI3_GarrisonGroupData> GetGroups()
	{
		return m_aGroups;
	}
	
	//------------------------------------
	bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid())
			return false;
		
		loadContext.ReadValue("name", m_sGarrison);
		loadContext.ReadValue("dscvrd", m_bIsDiscovered);
		loadContext.ReadValue("groups", m_aGroups);
		
		return true;
	}
	
	bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid())
			return false;
		
		saveContext.WriteValue("name", m_sGarrison);
		saveContext.WriteValue("dscvrd", m_bIsDiscovered);
		saveContext.WriteValue("groups", m_aGroups);
		
		return true;
	}
	
	//------------------------------------
	static MI3_GarrisonData Create(MI3_Garrison garrison)
	{
		MI3_GarrisonData data = new MI3_GarrisonData();
		
		data.SetName(garrison.GetGarrisonName());
		data.SetDiscovered(garrison.IsDiscovered());
		
		MI3_GarrisonGroupManagerComponent gm = garrison.GetGroupManager();
		
		array<ref MI3_GarrisonGroupData> groupsData = MI3_GarrisonGroupData.Create(gm);
		
		data.SetGroups(groupsData);
		
		return data;
	}
}

class MI3_GarrisonClass : GenericEntityClass
{
}

class MI3_Garrison : GenericEntity
{
	[Attribute(category: "Garrison Properties")]
	protected string m_sName;
	[Attribute(category: "Garrison Properties")]
	protected FactionKey m_sFactionKey;
	
	protected bool m_bIsDiscovered;
	
	protected ref MI3_GarrisonData m_GarrisonData;
	protected ref array<MI3_GarrisonComponent> m_aComponents;
	
	protected MI3_GarrisonSpawnHandlerComponent m_SpawnHandler;
	protected MI3_GarrisonGroupManagerComponent m_GroupManager;
	
	protected int m_iId = -1;
	protected bool m_bIsSpawned = false;
	
	protected ref ScriptInvoker Event_OnDiscovered	= new ScriptInvoker();
	protected ref ScriptInvoker Event_OnAttack		= new ScriptInvoker();
	protected ref ScriptInvoker Event_OnCapture		= new ScriptInvoker();
	protected ref ScriptInvoker Event_OnCaptured	= new ScriptInvoker();
	
	//-----------------------------------------------------
	ScriptInvoker GetOnDiscovered()
	{
		return Event_OnDiscovered;
	}
	
	ScriptInvoker GetOnAttack()
	{
		return Event_OnAttack;
	}
	
	ScriptInvoker GetOnCapture()
	{
		return Event_OnCapture;
	}
	
	ScriptInvoker GetOnCaptured()
	{
		return Event_OnCaptured;
	}
	
	//-----------------------------------------------------
	void Discover()
	{
		Print("Garrison discovered: " + m_sName);
		
		m_bIsDiscovered = true;
		
		Event_OnDiscovered.Invoke(this);
	}
	
	bool IsDiscovered()
	{
		return m_bIsDiscovered;
	}
	
	//-----------------------------------------------------
	void SetFactionKey(FactionKey faction)
	{
		m_sFactionKey = faction;
	}
	
	FactionKey GetFactionKey()
	{
		return m_sFactionKey;
	}
	
	//-----------------------------------------------------
	int GetGarrisonId()
	{
		return m_iId;
	}
	
	//-----------------------------------------------------
	string GetGarrisonName()
	{
		return m_sName;
	}
	
	//-----------------------------------------------------
	MI3_GarrisonSpawnHandlerComponent GetSpawnHandler()
	{
		return m_SpawnHandler;
	}
	
	//-----------------------------------------------------
	MI3_GarrisonGroupManagerComponent GetGroupManager()
	{
		return m_GroupManager;
	}
	
	//-----------------------------------------------------
	void Load(notnull MI3_GarrisonData garrisonData)
	{
		if (IsSpawned())
			return;
		
		m_bIsSpawned = true;
		
		m_bIsDiscovered = garrisonData.IsDiscovered();
		array<ref MI3_GarrisonGroupData> groups = garrisonData.GetGroups();
		
		m_GroupManager.SpawnAllGroups(groups);
	}
	
	void Unload()
	{
		if (!IsSpawned())
			return;
		
		m_bIsSpawned = false;
		
		m_GroupManager.DespawnAll();
	}
	
	//-----------------------------------------------------
	bool IsSpawned()
	{
		return m_bIsSpawned;
	}
	
	//-----------------------------------------------------
	int GetUnitCount()
	{
		return m_GroupManager.GetUnitCount();
	}
	
	//-----------------------------------------------------
	float DistanceSquareTo(IEntity ent)
	{
		vector entPos = ent.GetOrigin();
		
		return vector.DistanceSqXZ(entPos, GetOrigin());
	}
	
	//-----------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		MI3_GameMode gamemode = MI3_GameMode.Cast(GetGame().GetGameMode());
		
		if (!gamemode)
			return;
		
		MI3_GarrisonManagerComponent gm = gamemode.GetGarrisonManager();
		
		if (gm)
			m_iId = gm.RegisterGarrison(this);
		
		array<Managed> components = {};
		
		owner.FindComponents(MI3_GarrisonComponent, components);
		
		m_aComponents = {};
		foreach (Managed m : components)
		{
			MI3_GarrisonComponent component = MI3_GarrisonComponent.Cast(m);
			m_aComponents.Insert(component);
		}
		
		m_SpawnHandler = MI3_GarrisonSpawnHandlerComponent.Cast(owner.FindComponent(MI3_GarrisonSpawnHandlerComponent));
		m_GroupManager = MI3_GarrisonGroupManagerComponent.Cast(owner.FindComponent(MI3_GarrisonGroupManagerComponent));
		
		m_GroupManager.GetOnGroupDeleted().Insert(gamemode.OnGroupDeleted);
	}
	
	//-----------------------------------------------------
	void MI3_Garrison(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);
	}
}
