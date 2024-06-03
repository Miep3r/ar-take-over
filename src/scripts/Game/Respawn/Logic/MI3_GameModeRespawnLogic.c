[BaseContainerProps(category: "Respawn")]
class MI3_GameModeSpawnLogic : SCR_SpawnLogic
{
	protected ref set<int> m_DisconnectingPlayers = new set<int>();
	
	//------------------------------------------------------------------------------------------------
	override void OnPlayerRegistered_S(int playerId)
	{
		super.OnPlayerRegistered_S(playerId);
		
		int indexOf = m_DisconnectingPlayers.Find(playerId);
		if (indexOf != -1)
		{
			m_DisconnectingPlayers.Remove(indexOf);
		}
		
		Print("Player registered: " + playerId);
		
		// ToDo: Add savegame menu
		
//		GetGame().GetCallqueue().CallLater(DoInitialSpawn, 0, false, playerId);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPlayerDisconnected_S(int playerId, KickCauseCode cause, int timeout)
	{
		super.OnPlayerDisconnected_S(playerId, cause, timeout);
		m_DisconnectingPlayers.Insert(playerId);
	}
	
	//------------------------------------------------------------------------------------------------
	//								Non network stuff
	//------------------------------------------------------------------------------------------------
	void DoInitialSpawn(int playerId)
	{
		// Probe reconnection component first
		IEntity returnedEntity;
		if (ResolveReconnection(playerId, returnedEntity))
		{
			// User was reconnected, their entity was returned
			return;
		}
		
		// Spawn player the usual way, if no entity has been given yet
		PlayerController playerController = GetGame().GetPlayerManager().GetPlayerController(playerId);
		IEntity controlledEntity = playerController.GetControlledEntity();
		if (controlledEntity)
			return;
		
		Spawn(playerId);
	}
	
	override void OnPlayerEntityLost_S(int playerId)
	{
		super.OnPlayerEntityLost_S(playerId);
		Spawn(playerId);
	}

	//------------------------------------------------------------------------------------------------
	override void OnPlayerSpawnFailed_S(int playerId)
	{
		super.OnPlayerSpawnFailed_S(playerId);

		int delay = Math.RandomFloat(900, 1100);
		GetGame().GetCallqueue().CallLater(Spawn, delay, false, playerId);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void Spawn(int playerId)
	{
		// Player is disconnecting (and disappearance of controlled entity started this feedback loop).
		// Simply ignore such requests as it would create unwanted entities.
		int indexOf = m_DisconnectingPlayers.Find(playerId);
		if (indexOf != -1)
		{
			m_DisconnectingPlayers.Remove(indexOf);
			return;
		}
		
		array<Faction> factions = {};
		GetGame().GetFactionManager().GetFactionsList(factions);		
		
		Faction targetFaction = factions.GetRandomElement();
		
		// Request both
		if (!GetPlayerFactionComponent_S(playerId).RequestFaction(targetFaction))
		{
			// Try again later
		}

		SCR_BasePlayerLoadout targetLoadout = GetGame().GetLoadoutManager().GetRandomFactionLoadout(targetFaction);		
		
		if (!GetPlayerLoadoutComponent_S(playerId).RequestLoadout(targetLoadout))
		{
			// Try again later
		}

		Faction faction = GetPlayerFactionComponent_S(playerId).GetAffiliatedFaction();
		if (!faction)
		{
			OnPlayerSpawnFailed_S(playerId);
			return;
		}

		SCR_BasePlayerLoadout loadout = GetPlayerLoadoutComponent_S(playerId).GetLoadout();
		if (!loadout)
		{
			OnPlayerSpawnFailed_S(playerId);
			return;
		}

		SCR_SpawnPoint point = SCR_SpawnPoint.GetRandomSpawnPointForFaction(faction.GetFactionKey());
		if (!point)
		{
			OnPlayerSpawnFailed_S(playerId);
			return;
		}

		SCR_SpawnPointSpawnData data = new SCR_SpawnPointSpawnData(loadout.GetLoadoutResource(), point.GetRplId());
		if (GetPlayerRespawnComponent_S(playerId).CanSpawn(data))
			DoSpawn(playerId, data);
		else
			OnPlayerSpawnFailed_S(playerId);
	}
	
	protected void DoSpawn(int playerId, SCR_SpawnData data)
	{
		if (!GetPlayerRespawnComponent_S(playerId).RequestSpawn(data))
		{
			// Try again later
		}
	}
}
