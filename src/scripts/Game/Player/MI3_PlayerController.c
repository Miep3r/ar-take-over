class MI3_PlayerControllerClass : SCR_PlayerControllerClass
{
};

class MI3_PlayerController : SCR_PlayerController
{
	protected const float UPDATE_INTERVAL = 1;
	
	protected ref array<MI3_PlayerControllerComponent> m_aComponents = new array<MI3_PlayerControllerComponent>();
	protected MI3_ConcealmentControllerComponent m_ConcealmentController;
	protected MI3_SavegameMenuControllerComponent m_SavegameMenuController;
	
	protected MI3_HudManagerComponent m_HudManager;
	protected SCR_InventoryStorageManagerComponent m_StorageManager;
	
	protected int m_iLastUpdate = 0;
	protected float m_fTimePassed = 0;
	
	//-------------------------------------------
	void RegisterComponent(MI3_PlayerControllerComponent component)
	{
		m_aComponents.Insert(component);
	}
	
	void SetConcealmentController(MI3_ConcealmentControllerComponent concealmentController)
	{
		m_ConcealmentController = concealmentController;
	}
	
	MI3_ConcealmentControllerComponent GetConcealmentController()
	{
		return m_ConcealmentController;
	}
	
	void SetSavegameMenuController(MI3_SavegameMenuControllerComponent component)
	{
		m_SavegameMenuController = component;
	}
	
	MI3_SavegameMenuControllerComponent GetSavegameMenuController()
	{
		return m_SavegameMenuController;
	}
	
	void SetHudManager(MI3_HudManagerComponent hudManager)
	{
		m_HudManager = hudManager;
	}
	
	//------------------------------------------- Inventory
	void AddInventoryStateListener()
	{
		m_StorageManager.m_OnInventoryOpenInvoker.Insert(OnInventoryToggle);
	}
	
	void OnInventoryToggle(bool open)
	{
		Print("Inventory toggled: " + open);
//		m_ConcealmentController.UpdateConcealment();
	}
	
	//------------------------------------------- Overrides
	override void OnControlledEntityChanged(IEntity from, IEntity to)
	{
		super.OnControlledEntityChanged(from, to);
		
		if (m_StorageManager)
		{
			m_StorageManager.m_OnInventoryOpenInvoker.Remove(OnInventoryToggle);
		}
		
		SCR_InventoryStorageManagerComponent storageManager = SCR_InventoryStorageManagerComponent.Cast(to.FindComponent(SCR_InventoryStorageManagerComponent));
		
		if (storageManager)
		{
			m_StorageManager = storageManager;
			AddInventoryStateListener();
		}
		
		// Respawn signal to concealment controller
		m_ConcealmentController.Reset();
	}
	
	private void ServerUpdate(float timeSlice)
	{
		foreach (MI3_PlayerControllerComponent component : m_aComponents)
		{
			component.EOnUpdate(timeSlice);
		}
	}
	
	private void ClientUpdate(float timeSlice)
	{
		if (m_ConcealmentController)
		{
			string text = m_ConcealmentController.GetConcealedStateText();
			
			if (m_HudManager != null)
				m_HudManager.UpdateUndercoverState(text);
		}
	}
	
	override event protected void OnUpdate(float timeSlice)
	{
		super.OnUpdate(timeSlice);
		
		m_fTimePassed += timeSlice;
		
		if (m_fTimePassed >= UPDATE_INTERVAL)
		{
			m_iLastUpdate = 0;
			m_fTimePassed = 0;
			
			if (Replication.IsServer())
				ServerUpdate(timeSlice);
			
			ClientUpdate(timeSlice);
		}
	}
	
	void MI3_PlayerController(IEntitySource src, IEntity parent)
	{
	}
};
