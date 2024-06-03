//------------------------------------------------------------------------------------------------
modded class SCR_AICombatComponent
{
	
	//------------------------------------------------------------------------------------------------
	protected void Event_OnDamageInstigator(BaseDamageContext damageContext)
	{
		Instigator instigator = damageContext.instigator;
		
		if (!instigator)
			return;
		
		IEntity ent = instigator.GetInstigatorEntity();
		
		Vehicle vehicle = Vehicle.Cast(ent);
		
		if (vehicle)
		{
			IEntity driver = vehicle.GetPilotCompartmentSlot().GetOccupant();
			
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(driver);
			
			if (!character)
				return;
			
			int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(character);
			
			if (playerId <= 0)
				return;
			
			MI3_PlayerController pc = MI3_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
			
			ExposePlayer(pc);
		}
		else
		{
			int id = instigator.GetInstigatorPlayerID();
			
			if (id <= 0)
				return;
			
			if (GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(GetOwner()) > 0)
			{
				// Unit is player, we dont get exposed by damaging our own...
				return;
			}
			
			MI3_PlayerController pc = MI3_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(id));
			
			ExposePlayer(pc);
		}
	}
	
	private void ExposePlayer(MI3_PlayerController pc)
	{
		// ToDo: figure this stuff out
		AIAgent aiAgent = GetAiAgent();
		
		if (!aiAgent)
			return;
		
		SCR_AIGroup aiGroup = SCR_AIGroup.Cast(aiAgent.GetParentGroup());
		
		pc.GetConcealmentController().Expose(aiGroup);
	}
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		m_DamageManager.GetOnDamage().Insert(Event_OnDamageInstigator);
	}
}
