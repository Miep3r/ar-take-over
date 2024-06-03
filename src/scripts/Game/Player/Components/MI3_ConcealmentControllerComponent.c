class MI3_ConcealmentControllerComponentClass : MI3_PlayerControllerComponentClass
{
}

class MI3_ConcealmentControllerComponent : MI3_PlayerControllerComponent
{
	[RplProp(condition: RplCondition.OwnerOnly, onRplName: "OnConcealedChange")]
	protected bool m_bIsConcealed;
	[RplProp(condition: RplCondition.OwnerOnly)]
	protected bool m_bIsHunted;
	[RplProp(condition: RplCondition.OwnerOnly)]
	protected bool m_bIsHostile;
	
	private FactionKey m_sRebelFaction;
	private FactionKey m_sCivilianFaction;
	
	protected int m_iHostility;
	protected ref array<SCR_AIGroup> m_aGroupsExposedBy;
	
	//------------------------------------------------------------------
	void OnConcealedChange()
	{
		Print("CONCEALMENT CHANGED: " + m_bIsConcealed);
	}
	
	//------------------------------------------------------------------
	void UpdateConcealment()
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(m_Controller.GetControlledEntity());
		
		if (!character)
			return;
		
//		m_bIsHunted = IsHunted();
		bool bump = UpdateIsHunted();
		bool isStillUndercover = IsStillUndercover(character);
		
		if (!isStillUndercover)
		{
			if (m_bIsConcealed)
			{
				character.SetAffiliatedFactionByKey(m_sRebelFaction);
			}
		}
		else
		{
			if (!m_bIsConcealed)
			{
				// Reset hostile state when going undercover again
				m_bIsHostile = false;
				
				character.SetAffiliatedFactionByKey(m_sCivilianFaction);
			}
		}
		
		if (m_bIsConcealed != isStillUndercover)
		{
			m_bIsConcealed = isStillUndercover;
			
			Replication.BumpMe();
			
			if (character.IsInVehicle())
				UpdatePlayerVehicleFaction(character);
		}
		else
		{
			if (bump)
				Replication.BumpMe();
		}
	}
	
	bool GetIsConcealed()
	{
		return m_bIsConcealed;
	}
	
	string GetConcealedStateText()
	{
		if (m_bIsConcealed)
		{
			return "Concealed";
		}
		else
		{
			if (m_bIsHunted)
				return "Hunted";
			else
				return "Exposed";
		}
	}
	
	void OnExpose(SCR_AIGroup group)
	{
		if (m_aGroupsExposedBy.Contains(group))
			return;
		
		// Dont even dare asking why...
		m_bIsHostile = false;
		
		Print("Exposed by group " + group);
		m_aGroupsExposedBy.Insert(group);
	}
	
	void OnLost(SCR_AIGroup group)
	{
		if (!m_aGroupsExposedBy.Contains(group))
			return;
		
		Print("Lost by group " + group);
		m_aGroupsExposedBy.RemoveItem(group);
	}
	
	void Expose(SCR_AIGroup group)
	{
		m_iHostility += 1;
		m_bIsHostile = true;
		m_bIsConcealed = false;
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(m_Controller.GetControlledEntity());
		character.SetAffiliatedFactionByKey(m_sRebelFaction);
		
		Replication.BumpMe();
		
		UpdatePlayerVehicleFaction(character);
		
		// We don't want the player to get stuck being hostile, so we need to reset it after some time
		GetGame().GetCallqueue().CallLater(ResetHostility, 3000);
	}
	
	void ResetHostility()
	{
		m_bIsHostile = false;
		m_iHostility -= 1;
	}
	
	void Reset()
	{
		m_aGroupsExposedBy.Clear();
		m_bIsConcealed = true;
		m_bIsHostile = false;
		m_iHostility = 0;
		m_bIsHunted = false;
	}
	
	//------------------------------------------------------------------
	private bool IsStillUndercover(SCR_ChimeraCharacter character)
	{
		bool isStillUndercover = m_bIsConcealed;
		bool hasWeapon = HasWeapon(character);
		
		if (m_bIsConcealed)
		{
			// Check switch to exposed
			if (!character.IsInVehicle())
			{
				// Check for equipped weapons
				isStillUndercover = !hasWeapon;
			}
		}
		else
		{
			// Check switch to undercover
			bool canGoUndercover = CanGoUndercover(character);
			
			if (character.IsInVehicle())
			{
				isStillUndercover = canGoUndercover;
			}
			else
			{
				isStillUndercover = (!hasWeapon && canGoUndercover);
			}
		}
		
		return isStillUndercover;
	}
	
//	private bool IsHunted()
//	{
//		return (m_aGroupsExposedBy.Count() > 0);
//	}
	
	private bool UpdateIsHunted()
	{
		bool isHunted = (m_aGroupsExposedBy.Count() > 0);
		
		bool changed = (isHunted != m_bIsHunted);
		
		m_bIsHunted = isHunted;
		
		return changed;
	}
	
	private bool HasIllegalItem(SCR_ChimeraCharacter character)
	{
		SCR_CharacterInventoryStorageComponent storage = SCR_CharacterInventoryStorageComponent.Cast(character.FindComponent(SCR_CharacterInventoryStorageComponent));
		
		
		
		return true;
	}
	
	private bool HasWeapon(SCR_ChimeraCharacter character)
	{
		BaseWeaponManagerComponent weaponManager = character.GetWeaponManager();
		
		array<WeaponSlotComponent> slots = {};
		weaponManager.GetWeaponsSlots(slots);
		
		bool hasWeapon = false;
		
		foreach (WeaponSlotComponent slot : slots)
		{
			IEntity weapon = slot.GetWeaponEntity();
			
			if (weapon)
			{
				hasWeapon = true;
				break;
			}
		}
		
		return hasWeapon;
	}
	
	private bool CanGoUndercover(SCR_ChimeraCharacter character)
	{
		return (!m_bIsHunted && (m_iHostility <= 0));
	}
	
	private Vehicle GetVehicle(SCR_ChimeraCharacter character)
	{
		if (!character.IsInVehicle())
			return null;
		
		CompartmentAccessComponent access = character.GetCompartmentAccessComponent();
		if (!access)
			return null;

		BaseCompartmentSlot compartmentSlot = BaseCompartmentSlot.Cast(access.GetCompartment());
		if (!compartmentSlot)
			return null;
		
		Vehicle vehicle = Vehicle.Cast(compartmentSlot.GetOwner());
		
		return vehicle;
	}
	
	private void SetVehicleFaction(Vehicle vehicle, FactionKey factionKey)
	{
		SCR_VehicleFactionAffiliationComponent factionAffiliationComponent = SCR_VehicleFactionAffiliationComponent.Cast(vehicle.FindComponent(SCR_VehicleFactionAffiliationComponent));
		
		if (!factionAffiliationComponent)
			return;
		
		Print("Old vehicle faction: " + factionAffiliationComponent.GetAffiliatedFaction().GetFactionKey());
		factionAffiliationComponent.SetAffiliatedFactionByKey(factionKey);
		Print("New vehicle faction: " + factionAffiliationComponent.GetAffiliatedFaction().GetFactionKey());
	}
	
	private void UpdatePlayerVehicleFaction(SCR_ChimeraCharacter player)
	{
		Vehicle vehicle = GetVehicle(player);
		
		if (vehicle)
			SetVehicleFaction(vehicle, player.GetFactionKey());
	}
	
	//------------------------------------------------------------------
	override void EOnUpdate(float timeSlice)
	{
		UpdateConcealment();
	}
	
	//------------------------------------------------------------------
	void MI3_ConcealmentControllerComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		m_Controller.SetConcealmentController(this);
		
		m_aGroupsExposedBy	= {};
		
		MI3_GameMode gm		= MI3_GameMode.Cast(GetGame().GetGameMode());
		m_sRebelFaction		= gm.GetRebelFactionKey();
		m_sCivilianFaction	= gm.GetCivilianFactionKey();
		
		m_bIsConcealed	= true;
		m_bIsHunted		= false;
		m_bIsHostile	= false;
		
		Print("Concealment Controller Constructed");
	}
}
