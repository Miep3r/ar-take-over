[BaseContainerProps(), BaseContainerCustomTitleField("m_sFactionKey")]
class MI3_GarrisonFactionConfig
{
	[Attribute()]
	protected FactionKey m_sFactionKey;
	[Attribute(params: "et")]
	protected ResourceName m_sSquadLeader;
	[Attribute(params: "et")]
	protected ResourceName m_sRifleman;
	[Attribute(params: "et")]
	protected ResourceName m_sGrenadier;
	[Attribute(params: "et")]
	protected ResourceName m_sMedic;
	[Attribute(params: "et")]
	protected ResourceName m_sMachineGunner;
	[Attribute(params: "et")]
	protected ResourceName m_sLightMachineGunner;
	[Attribute(params: "et")]
	protected ResourceName m_sAntiTank;
	[Attribute(params: "et")]
	protected ResourceName m_sSharpShooter;
	
	//----------------------------------------------------
	FactionKey GetFactionKey()
	{
		return m_sFactionKey;
	}
	
	//----------------------------------------------------
	ResourceName GetSquadLeader()
	{
		return m_sSquadLeader;
	}
	
	//----------------------------------------------------
	ResourceName GetRifleman()
	{
		return m_sRifleman;
	}
	
	//----------------------------------------------------
	ResourceName GetGrenadier()
	{
		return m_sGrenadier;
	}
	
	//----------------------------------------------------
	ResourceName GetMedic()
	{
		return m_sMedic;
	}
	
	//----------------------------------------------------
	ResourceName GetMachineGunner()
	{
		return m_sMachineGunner;
	}
	
	//----------------------------------------------------
	ResourceName GetLightMachineGunner()
	{
		return m_sLightMachineGunner;
	}
	
	//----------------------------------------------------
	ResourceName GetAntiTank()
	{
		return m_sAntiTank;
	}
	
	//----------------------------------------------------
	ResourceName GetSharpShooter()
	{
		return m_sSharpShooter;
	}
};

class MI3_GarrisonManagerComponentClass : SCR_BaseGameModeComponentClass
{
};

class MI3_GarrisonManagerComponent : SCR_BaseGameModeComponent
{
	protected static MI3_GarrisonManagerComponent m_Instance;
	
	[Attribute(category: "Faction Configuration")]
	protected ref array<ref MI3_GarrisonFactionConfig> m_aFactionConfigs;
	[Attribute("5", desc: "Rate of update for all garrisons (seconds)")]
	protected int m_iUpdateInterval;
	[Attribute("250", desc: "Garrison discovery distance (meters)")]
	protected int m_iDiscoveryDistance;
	
	protected ref array<MI3_Garrison> m_aGarrisons;
	protected ref map<string, MI3_Garrison> m_mGarrisons;
	
	protected int m_iDiscoveryDistanceSqr;
	protected float m_fTimePassed = 0;
	
	//-------------------------------------------------------------
	MI3_GarrisonFactionConfig GetFactionConfigByKey(FactionKey faction)
	{
		foreach (MI3_GarrisonFactionConfig c : m_aFactionConfigs)
		{
			if (c.GetFactionKey() == faction)
				return c;
		}
		
		return null;
	}
	
	//-------------------------------------------------------------
	int RegisterGarrison(MI3_Garrison garrison)
	{
		int id = m_aGarrisons.Insert(garrison);
		m_mGarrisons.Set(garrison.GetGarrisonName(), garrison);
		
		Print("Registered garrison " + id);
		
		return id;
	}
	
	//-------------------------------------------------------------
	void GetGarrisons(out array<MI3_Garrison> outGarrisons)
	{
		outGarrisons.InsertAll(m_aGarrisons);
	}
	
	//-------------------------------------------------------------
	void LoadAllGarrisons(array<ref MI3_GarrisonData> garrisonsData)
	{
		if (garrisonsData && !garrisonsData.IsEmpty())
		{
			foreach (MI3_GarrisonData garrisonData : garrisonsData)
			{
				MI3_Garrison garrison = m_mGarrisons.Get(garrisonData.GetName());
				
				if (garrison)
					garrison.Load(garrisonData);
				
				if (garrison.IsDiscovered())
					CreateGarrisonMarker(garrison);
			}
		}
		else
		{
			foreach (MI3_Garrison garrison : m_aGarrisons)
			{
				MI3_GarrisonData gd = MI3_GarrisonDataUtility.GetInitialDataForGarrison(garrison);
				garrison.Load(gd);
			}
		}
	}
	
	void SaveAllGarrisons(out array<ref MI3_GarrisonData> out_garrisons)
	{
		if (!m_aGarrisons)
			return;
		
		foreach (MI3_Garrison garrison : m_aGarrisons)
		{
			if (!garrison)
				return;
			
			MI3_GarrisonData garrisonData = MI3_GarrisonData.Create(garrison);
			
			out_garrisons.Insert(garrisonData);
		}
	}
	
	//-------------------------------------------------------------
	void CreateGarrisonMarker(MI3_Garrison garrison)
	{
		if (!garrison.IsDiscovered())
			return;
		
		SCR_MapMarkerManagerComponent markerMgr = SCR_MapMarkerManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_MapMarkerManagerComponent));
		
		if (!markerMgr)
			return;
		
		vector pos = garrison.GetOrigin();
		
		markerMgr.InsertStaticMarkerByType(SCR_EMapMarkerType.SIMPLE, pos[0], pos[2], false, 1);
		
		/*
		SCR_MapMarkerBase marker = new SCR_MapMarkerBase();
		marker.SetType(SCR_EMapMarkerType.PLACED_MILITARY);
		marker.SetWorldPos(pos[0], pos[2]);
		marker.SetMarkerConfigID(0);
		markerMgr.InsertStaticMarker();
		*/
	}
	
	//-------------------------------------------------------------
	void CheckGarrisonDiscovery()
	{
		Print("Checking garrison discovery", LogLevel.SPAM);
		
		array<int> playerIds = {};
		GetGame().GetPlayerManager().GetAllPlayers(playerIds);
		
		array<IEntity> plrEntities = {};
		foreach (int playerId : playerIds)
		{
			IEntity ent = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
			
			if (ent)
				plrEntities.Insert(ent);
		}
		
		foreach (MI3_Garrison garrison : m_aGarrisons)
		{
			if (garrison.IsDiscovered())
				continue;
			
			UpdateGarrisonDiscovered(garrison, plrEntities);
		}
	}
	
	void UpdateGarrisonDiscovered(MI3_Garrison garrison, array<IEntity> plrEntities)
	{
		foreach (IEntity ent : plrEntities)
		{
			if (garrison.DistanceSquareTo(ent) <= m_iDiscoveryDistanceSqr)
			{
				garrison.Discover();
				return;
			}
		}
	}
	
	//-------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		m_iDiscoveryDistanceSqr = m_iDiscoveryDistance * m_iDiscoveryDistance;
	}
	
	override void EOnFixedFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);
		
		m_fTimePassed += timeSlice;
		if (m_fTimePassed > m_iUpdateInterval)
		{
			m_fTimePassed = 0;
			
			CheckGarrisonDiscovery();
		}
	}
	
	//-------------------------------------------------------------
	void MI3_GarrisonManagerComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		m_Instance = this;
		
		if (Replication.IsServer())
			SetEventMask(ent, EntityEvent.INIT | EntityEvent.FIXEDFRAME);
		
		m_aGarrisons = {};
		m_mGarrisons = new map<string, MI3_Garrison>();
	}
	
	void ~MI3_GarrisonManagerComponent()
	{
//		SaveAllGarrisons();
		
		m_aGarrisons.Clear();
		m_aGarrisons = null;
	}
	
	//---------------------------------------------
	static MI3_GarrisonManagerComponent GetInstance()
	{
		return m_Instance;
	}
};
