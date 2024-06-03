modded class SCR_CharacterInventoryStorageComponent
{
	//------------------------------------------------------------------------------------------------
	//!
	//! \param[in] item
	//! \param[in] storageOwner
	override void HandleOnItemAddedToInventory( IEntity item, BaseInventoryStorageComponent storageOwner )
	{
		Print("Item added to inventory: " + item.GetPrefabData().GetPrefabName());
		Print("Storage owner: " + storageOwner.GetParentSlot());
		
		int targetQuickSlot = StoreItemToQuickSlot(item);
		if (targetQuickSlot > -1 && SCR_WeaponSwitchingBaseUI.s_bOpened)
			SCR_WeaponSwitchingBaseUI.RefreshQuickSlots(targetQuickSlot);
	}
}
