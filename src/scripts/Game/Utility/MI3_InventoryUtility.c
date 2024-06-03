class MI3_InventoryUtility
{
	protected SCR_InventoryStorageManagerComponent m_InventoryManager;
	protected SCR_CharacterInventoryStorageComponent m_CharacterStorage;
	
	// Load
	protected IEntity LoadAttachment(MI3_AttachmentComponentDto component)
	{
		Resource aRes = Resource.Load(component.GetPrefab());
		IEntity newAttachment = GetGame().SpawnEntityPrefab(aRes);
		
		return newAttachment;
	}
	
	protected IEntity LoadWeapon(MI3_WeaponComponentDto component)
	{
		Resource res = Resource.Load(component.GetPrefab());
		if (res)
		{
			IEntity item = GetGame().SpawnEntityPrefab(res);
			
			WeaponComponent weaponComponent = WeaponComponent.Cast(item.FindComponent(WeaponComponent));
			
			// Load muzzle data
			array<BaseMuzzleComponent> muzzles = {};
			weaponComponent.GetMuzzlesList(muzzles);
			
			array<ref MI3_MuzzleComponentDto> muzzleComponents = component.GetMuzzles();
			int muzzleIndex = 0;
			foreach (MI3_MuzzleComponentDto muzzleComponent : muzzleComponents)
			{
				MI3_MagazineComponentDto magComponentDto = muzzleComponent.GetMagazine();
				
				BaseMuzzleComponent muzzle = muzzles.Get(muzzleIndex);
				
				// Delete current mag in weapon muzzle
				BaseMagazineComponent oldMagComponent = muzzle.GetMagazine();
				IEntity oldMag = oldMagComponent.GetOwner();
				
				InventoryItemComponent magInventory = InventoryItemComponent.Cast(oldMag.FindComponent(InventoryItemComponent));
				BaseInventoryStorageComponent magStorage = magInventory.GetParentSlot().GetStorage();
				
				m_InventoryManager.TryRemoveItemFromStorage(oldMag, magStorage);
				RplComponent.DeleteRplEntity(oldMag, true);
				
				if (magComponentDto)
				{
					Resource mRes = Resource.Load(magComponentDto.GetPrefab());
					
					if (!mRes)
						continue;
					
					IEntity newMag = GetGame().SpawnEntityPrefab(mRes);
					
					BaseMagazineComponent newMagComponent = BaseMagazineComponent.Cast(newMag.FindComponent(BaseMagazineComponent));
					
					// Insert new mag
					bool success = m_InventoryManager.TryInsertItemInStorage(newMag, magStorage, 0);
					
					if (!muzzleComponent.IsChambered())
					{
						int index = muzzle.GetCurrentBarrelIndex();
						muzzle.ClearChamber(index);
					}
					
					newMagComponent.SetAmmoCount(magComponentDto.GetAmmo());
				}
				
				muzzleIndex++;
			}
			
			// Load attachment data
			array<AttachmentSlotComponent> attachmentSlots = {};
			weaponComponent.GetAttachments(attachmentSlots);
			
			array<ref MI3_AttachmentComponentDto> attachmentComponents = component.GetAttachments();
			foreach (MI3_AttachmentComponentDto attachmentComponent : attachmentComponents)
			{
				AttachmentSlotComponent slot = attachmentSlots.Get(attachmentComponent.GetSlotIndex());
				
				if (!slot)
					continue;
				
				// Delete attachment if current one exists
				IEntity currentAttachment = slot.GetAttachedEntity();
				if (currentAttachment)
				{
					RplComponent.DeleteRplEntity(currentAttachment, true);
				}
				
				IEntity attachment = LoadAttachment(attachmentComponent);
				slot.SetAttachment(attachment);
			}
			
			return item;
		}
		
		return null;
	}
	
	protected IEntity LoadMagazine(MI3_MagazineComponentDto component)
	{
		Resource res = Resource.Load(component.GetPrefab());
		IEntity item = GetGame().SpawnEntityPrefab(res);
		
		MagazineComponent magComponent = MagazineComponent.Cast(item.FindComponent(MagazineComponent));
		if (magComponent)
		{
			magComponent.SetAmmoCount(component.GetAmmo());
		}
		
		return item;
	}
	
	protected IEntity LoadItem(MI3_InventoryComponentDto component)
	{
		Print("ADDING: " + component.GetPrefab());
		
		if (MI3_MagazineComponentDto.Cast(component))
		{
			MI3_MagazineComponentDto mag = MI3_MagazineComponentDto.Cast(component);
			
			return LoadMagazine(mag);
		}
		else if (MI3_WeaponComponentDto.Cast(component))
		{
			MI3_WeaponComponentDto weaponComponent = MI3_WeaponComponentDto.Cast(component);
			
			return LoadWeapon(weaponComponent);
		}
		else
		{
			Resource res = Resource.Load(component.GetPrefab());
			return GetGame().SpawnEntityPrefab(res);
		}
		
		return null;
	}
	
	protected void LoadStorage(BaseInventoryStorageComponent storage, MI3_InventoryComponentDto storageData)
	{
		Print("INVENTORY OWNER: " + Replication.FindId(m_InventoryManager));
		Print("STORAGE OWNER: " + storage.GetOwner().Type());
		foreach (MI3_InventoryComponentDto component : storageData.GetStorageComponents())
		{
			IEntity item = LoadItem(component);
			
			if (item)
				m_InventoryManager.TryInsertItemInStorage(item, storage);
			
//			int slotId = component.GetSlotIndex();
//			if (slotId >= 0)
//				m_CharacterStorage.StoreItemToQuickSlot(item, slotId, true);
		}
	}
	
	void LoadInventoryForCharacter(MI3_InventoryDto data)
	{
		if (!data)
			return;
		
		// Handle top level base
		foreach (MI3_InventoryComponentDto component : data.GetBaseComponents())
		{
			IEntity item = LoadItem(component);
			
			if (component.HasStorage())
			{
				// Handle sub storages, give storage time to initialize properly (thanks BI)
				BaseInventoryStorageComponent storage = BaseInventoryStorageComponent.Cast(item.FindComponent(BaseInventoryStorageComponent));
				GetGame().GetCallqueue().CallLater(LoadStorage, 200, false, storage, component);
			}
			
			m_InventoryManager.TryInsertItem(item, EStoragePurpose.PURPOSE_LOADOUT_PROXY);
		}
		
		// Load weapons
		foreach (MI3_WeaponComponentDto component : data.GetWeaponComponents())
		{
			IEntity item = LoadWeapon(component);
			
			if (item)
				m_InventoryManager.EquipWeapon(item);
		}
	}
	
	// Store
	protected BaseMagazineComponent GetMagazineComponentFromEntity(IEntity entity)
	{
		if (entity == null)
			return null;
		
		return BaseMagazineComponent.Cast(entity.FindComponent(BaseMagazineComponent));
	}
	
	protected BaseInventoryStorageComponent GetStorageComponentFromEntity(IEntity entity)
	{
		if (entity == null)
			return null;
		
		return BaseInventoryStorageComponent.Cast(entity.FindComponent(SCR_UniversalInventoryStorageComponent));
	}
	
	protected MI3_AttachmentComponentDto WriteAttachmentDto(IEntity attachment, int slotId)
	{
		MI3_AttachmentComponentDto component = new MI3_AttachmentComponentDto();
		
		component.SetPrefab(attachment.GetPrefabData().GetPrefabName());
		Print("ATTACHMENT NAME: " + attachment.GetPrefabData().GetPrefabName());
		component.SetSlotIndex(slotId);
		
		return component;
	}
	
	protected MI3_MagazineComponentDto WriteMagazineDto(BaseMagazineComponent magazineComponent)
	{
		IEntity item = magazineComponent.GetOwner();
		MI3_MagazineComponentDto component = new MI3_MagazineComponentDto();
		
		int ammo = magazineComponent.GetAmmoCount();
		
		component.SetAmmo(ammo);
		
		return component;
	}
	
	protected MI3_MuzzleComponentDto WriteMuzzleDto(BaseMuzzleComponent muzzle)
	{
		MI3_MuzzleComponentDto component = new MI3_MuzzleComponentDto();
		BaseMagazineComponent magazine = muzzle.GetMagazine();
		
		if (magazine)
		{
			MI3_MagazineComponentDto mag = WriteMagazineDto(magazine);
			mag.SetPrefab(magazine.GetOwner().GetPrefabData().GetPrefabName());
			component.SetMagazine(mag);
		}
		
		component.SetChambered(muzzle.IsBarrelChambered(0));
		
		return component;
	}
	
	protected MI3_WeaponComponentDto WriteWeaponDto(WeaponComponent weapon)
	{
		MI3_WeaponComponentDto component = new MI3_WeaponComponentDto();
		
		// Save muzzles
		array<BaseMuzzleComponent> muzzles = {};
		weapon.GetMuzzlesList(muzzles);
		
		array<ref MI3_MuzzleComponentDto> muzzleComponents = {};
		foreach (BaseMuzzleComponent muzzle : muzzles)
		{
			muzzleComponents.Insert(WriteMuzzleDto(muzzle));
		}
		
		component.SetMuzzles(muzzleComponents);
		
		// Save attachments
		array<AttachmentSlotComponent> attachments = {};
		weapon.GetAttachments(attachments);
		
		array<ref MI3_AttachmentComponentDto> attachmentComponents = {};
		int attachmentIndex = 0;
		foreach (AttachmentSlotComponent slot : attachments)
		{
			// Find out whether attachment should be saved or not
			if (slot.ShouldShowInInspection())
			{
				IEntity attachment = slot.GetAttachedEntity();
				
				if (attachment)
				{
					MI3_AttachmentComponentDto attachmentComponent = WriteAttachmentDto(attachment, attachmentIndex);
					attachmentComponents.Insert(attachmentComponent);
				}
			}
			attachmentIndex++;
		}
		
		component.SetAttachments(attachmentComponents);
		
		return component;
	}
	
	protected MI3_InventoryComponentDto WriteItemDto(IEntity item)
	{
		MI3_InventoryComponentDto component;
		
		// Magazines need special handling due to extra data
		if (item.FindComponent(BaseMagazineComponent))
		{
			BaseMagazineComponent magazineComponent = GetMagazineComponentFromEntity(item);
			
			component = WriteMagazineDto(magazineComponent);
		}
		else if (item.FindComponent(WeaponComponent))
		{
			WeaponComponent weaponComponent = WeaponComponent.Cast(item.FindComponent(WeaponComponent));
			
			component = WriteWeaponDto(weaponComponent);
		}
		else
		{
			component = new MI3_InventoryComponentDto();
		}
		
		// Handle item storage
		BaseInventoryStorageComponent storage = GetStorageComponentFromEntity(item);
		
		if (storage != null)
		{
			// Item has internal storage --> 1:n
			array<IEntity> items = {};
			storage.GetAll(items);
			
			array<ref MI3_InventoryComponentDto> containerItems = {};
			foreach (IEntity containerItem : items)
			{
				containerItems.Insert(WriteItemDto(containerItem));
			}
			
			component.SetStorageComponents(containerItems);
		}
		
		// Set basic attributes
		component.SetPrefab(item.GetPrefabData().GetPrefabName());
		component.SetSlotIndex(m_CharacterStorage.GetEntityIndexInQuickslots(item));
		
		return component;
	}
	
	MI3_InventoryDto SerializeCharacterInventory()
	{
		MI3_InventoryDto result = new MI3_InventoryDto();
		
		// Fetch all relevant data
		array<IEntity> items = new array<IEntity>();
		array<IEntity> weapons = new array<IEntity>();
		
		m_CharacterStorage.GetAll(items);
		m_CharacterStorage.GetWeaponStorage().GetAll(weapons);
		
		array<ref MI3_InventoryComponentDto> baseComponents = {};
		foreach (IEntity item : items)
		{
			MI3_InventoryComponentDto container = WriteItemDto(item);
			baseComponents.Insert(container);
		}
		
		array<ref MI3_WeaponComponentDto> weaponComponents = {};
		foreach (IEntity weapon : weapons)
		{
			WeaponComponent weaponComponent = WeaponComponent.Cast(weapon.FindComponent(WeaponComponent));
			MI3_WeaponComponentDto container = WriteWeaponDto(weaponComponent);
			
			container.SetPrefab(weapon.GetPrefabData().GetPrefabName());
			
			weaponComponents.Insert(container);
		}
		
		result.SetBaseComponents(baseComponents);
		result.SetWeaponComponents(weaponComponents);
		
		return result;
	}
	
	// CTOR
	void InventoryUtility(SCR_InventoryStorageManagerComponent inventoryManager)
	{
		m_InventoryManager = inventoryManager;
		m_CharacterStorage = inventoryManager.GetCharacterStorage();
	}
}
