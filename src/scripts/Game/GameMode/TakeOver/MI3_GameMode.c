modded enum SCR_DebugMenuID
{
	DEBUGUI_MI3_MENU,
	DEBUGUI_MI3_DO_DEBUG
}

class MI3_SavegameMetadata : Managed
{
	protected ref array<string> m_aSavegames = {};
	
	void AddSavegame(string name)
	{
		m_aSavegames.Insert(name);
	}
	
	array<string> GetSavegames()
	{
		return m_aSavegames;
	}
	
	bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid())
			return false;
		
		loadContext.ReadValue("savegames", m_aSavegames);
		
		return true;
	}
	
	bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid())
			return false;
		
		saveContext.WriteValue("savegames", m_aSavegames);
		
		return true;
	}
}

[BaseContainerProps()]
class MI3_GameModeItemWhitelistConfig
{
	[Attribute(params: "et")]
	protected ref array<ResourceName> m_aItemsWhitelist;
}

class MI3_GameModeClass : SCR_BaseGameModeClass
{
}

class MI3_GameMode : SCR_BaseGameMode
{
	protected static MI3_GameMode m_Instance;
	protected static bool m_bIsDiagMenuCreated;
	
	[Attribute("US")]
	protected FactionKey m_sOccupyingFaction;
	[Attribute("USSR")]
	protected FactionKey m_sInvadingFaction;
	[Attribute("FIA")]
	protected FactionKey m_sRebelFaction;
	[Attribute("CIV")]
	protected FactionKey m_sCivilianFaction;
	[Attribute()]
	protected ref MI3_GameModeItemWhitelistConfig m_ItemWhitelistConfig;
	
	protected ref MI3_SavegameMetadata m_SavegameMetadata;
	
	protected SCR_GroupsManagerComponent m_GroupsManager;
	protected MI3_GarrisonManagerComponent m_GarrisonManager;
	
	protected int m_iCommanderId = -1;
	protected bool m_bIsInitialized = false;
	protected string m_sSavegame;
	
	//-----------------------------------------------------------------------------
	FactionKey GetRebelFactionKey()
	{
		return m_sRebelFaction;
	}
	
	FactionKey GetCivilianFactionKey()
	{
		return m_sCivilianFaction;
	}
	
	//-----------------------------------------------------------------------------
	MI3_GarrisonManagerComponent GetGarrisonManager()
	{
		return m_GarrisonManager;
	}
	
	//-----------------------------------------------------------------------------
	void SetCommanderId(int id)
	{
		Print("Setting new commander: " + id);
		m_iCommanderId = id;
		
		#ifdef MI3_USE_SAVEGAMES
		if (!m_bIsInitialized)
		{
			// Handle savegame loading
			MI3_PlayerController pc = MI3_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(id));
			
			if (!pc)
				return;
			
			MI3_SavegameMenuControllerComponent smc = pc.GetSavegameMenuController();
			smc.OpenSavegameMenu(m_SavegameMetadata.GetSavegames());
		}
		#endif
	}
	
	int GetCommanderId()
	{
		return m_iCommanderId;
	}
	
	//-----------------------------------------------------------------------------
	void OnGarrisonGroupsDelete(array<SCR_AIGroup> groups)
	{
		array<int> allPlayerIds = {};
		GetGame().GetPlayerManager().GetAllPlayers(allPlayerIds);
		
		foreach (int id : allPlayerIds)
		{
			MI3_PlayerController pc = MI3_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(id));
		
			foreach (SCR_AIGroup group : groups)
			{
				pc.GetConcealmentController().OnLost(group);
			}	
		}
	}
	
	void OnEnemyDetectedGlobal(SCR_AIGroup group, SCR_AITargetInfo targetInfo)
	{
		Print("Target detected! Group: " + group.GetID(), LogLevel.DEBUG);
		SCR_ChimeraCharacter unit = SCR_ChimeraCharacter.Cast(targetInfo.m_Entity);
		
		if (unit)
		{
			PlayerManager pm = GetGame().GetPlayerManager();
			
			int id = pm.GetPlayerIdFromControlledEntity(unit);
			
			if (id <= 0)
				return;
			
			MI3_PlayerController pc = MI3_PlayerController.Cast(pm.GetPlayerController(id));
			
			MI3_ConcealmentControllerComponent cc = MI3_ConcealmentControllerComponent.Cast(pc.FindComponent(MI3_ConcealmentControllerComponent));
			cc.OnExpose(group);
		}
	}
	
	void OnEnemyForgotGlobal(SCR_AIGroup group, SCR_AITargetInfo targetInfo)
	{
		Print("Target forgot! Group: " + group.GetID(), LogLevel.DEBUG);
		SCR_ChimeraCharacter unit = SCR_ChimeraCharacter.Cast(targetInfo.m_Entity);
		
		if (unit)
		{
			PlayerManager pm = GetGame().GetPlayerManager();
			
			int id = pm.GetPlayerIdFromControlledEntity(unit);
			
			if (id <= 0)
				return;
			
			MI3_PlayerController pc = MI3_PlayerController.Cast(pm.GetPlayerController(id));
			
			MI3_ConcealmentControllerComponent cc = MI3_ConcealmentControllerComponent.Cast(pc.FindComponent(MI3_ConcealmentControllerComponent));
			cc.OnLost(group);
		}
	}
	
	void OnGroupDeleted(MI3_GarrisonGroupManagerComponent groupManager, SCR_AIGroup group)
	{
		array<int> playerIds = {};
		GetGame().GetPlayerManager().GetAllPlayers(playerIds);
		
		foreach (int playerId : playerIds)
		{
			MI3_PlayerController controller = MI3_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
			
			if (!controller)
				continue;
			
			controller.GetConcealmentController().OnLost(group);
		}
	}
	
	//-----------------------------------------------------------------------------
	void Save(notnull MI3_SavegameData savegame)
	{
		savegame.m_GarrisonsData = {};
		m_GarrisonManager.SaveAllGarrisons(savegame.m_GarrisonsData);
	}
	
	void Load(notnull MI3_SavegameData savegame)
	{
		m_GarrisonManager.LoadAllGarrisons(savegame.m_GarrisonsData);
	}
	
	//-----------------------------------------------------------------------------
	void DoInitialSpawn(int playerId)
	{
		MI3_GameModeSpawnLogic spawnLogic = MI3_GameModeSpawnLogic.Cast(GetRespawnSystemComponent().GetSpawnLogic());
		spawnLogic.DoInitialSpawn(playerId);
	}
	
	void SetSavegame(string name)
	{
		m_sSavegame = name;
	}
	
	void InitSavegame(string name)
	{
		SetSavegame(name);
		m_bIsInitialized = true;
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void LoadSavegame_S(string name)
	{
		Print("Loading savegame '" + name + "'");
		
		InitSavegame(name);
		
		MI3_SavegameUtility.Load(name);
		
		DoInitialSpawn(GetCommanderId());
	}
	
	void SaveSavegame_S(string name)
	{
		MI3_SavegameUtility.Save(name);
	}
	
	void CreateSavegame_S(string name)
	{
		Print("Creating savegame '" + name + "'");
		m_SavegameMetadata.AddSavegame(name);
		
		MI3_SavegameUtility.Init(name);
		
		DoInitialSpawn(GetCommanderId());
	}
	
	//-----------------------------------------------------------------------------
	protected override void OnPlayerRegistered(int playerId)
	{
		super.OnPlayerRegistered(playerId);
		
		if (!IsMaster())
			return;
		
		if (m_iCommanderId == -1)
		{
			// First player to connect becomes default commander
			SetCommanderId(playerId);
		}
	}
	
	override void OnPlayerSpawnFinalize_S(SCR_SpawnRequestComponent requestComponent, SCR_SpawnHandlerComponent handlerComponent, SCR_SpawnData data, IEntity entity)
	{
		super.OnPlayerSpawnFinalize_S(requestComponent, handlerComponent, data, entity);
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(entity);
		
		character.SetAffiliatedFactionByKey(m_sCivilianFaction);
	}
	
	override protected void OnGameModeStart()
	{
		super.OnGameModeStart();
		
		Print("Mieps GameMode started!");
	}
	
	override void OnGameEnd()
	{
		super.OnGameEnd();
		
		if (!Replication.IsServer())
			return;
		
		SCR_JsonSaveContext saveContext = new SCR_JsonSaveContext();
		m_SavegameMetadata.SerializationSave(saveContext);
		
		saveContext.SaveToFile("./Savegames.json");
		
		if (m_bIsInitialized)
		{
			MI3_SavegameUtility.Save(m_sSavegame);
		}
	}
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		m_GroupsManager = SCR_GroupsManagerComponent.Cast(owner.FindComponent(SCR_GroupsManagerComponent));
		m_GarrisonManager = MI3_GarrisonManagerComponent.Cast(owner.FindComponent(MI3_GarrisonManagerComponent));
		
		m_SavegameMetadata = MI3_SavegameUtility.GetSavegames();
		
		GetGame().GetCallqueue().CallLater(RenderDebugMenu, 0, true);
		
		#ifndef MI3_USE_SAVEGAMES
		GetGame().GetCallqueue().CallLater(LoadSavegame_S, 100, false, "DefaultSavegame");
		#endif
	}
	
	//-----------------------------------------------------------------------------
	private void RenderDebugMenu()
	{
		MI3_DebugMenu.Render(m_GarrisonManager);
	}
	
	//-----------------------------------------------------------------------------
	void MI3_GameMode(IEntitySource src, IEntity parent)
	{
		m_Instance = this;
		
		if (!m_bIsDiagMenuCreated)
		{
			m_bIsDiagMenuCreated = true;
			DiagMenu.RegisterMenu(SCR_DebugMenuID.DEBUGUI_MI3_MENU, "MI3", "");
			DiagMenu.RegisterBool(SCR_DebugMenuID.DEBUGUI_MI3_DO_DEBUG, "", "Debug Menu", "MI3");
		}
	}
	
	void ~MI3_GameMode()
	{
		MI3_SavegameUtility.SaveMetadata(m_SavegameMetadata);
		
		if (m_bIsDiagMenuCreated)
		{
			DiagMenu.Unregister(SCR_DebugMenuID.DEBUGUI_MI3_MENU);
			DiagMenu.Unregister(SCR_DebugMenuID.DEBUGUI_MI3_DO_DEBUG);
			m_bIsDiagMenuCreated = false;
		}
	}
	
	//-----------------------------------------------------------------------------
	static MI3_GameMode GetInstance()
	{
		return m_Instance;
	}
};
