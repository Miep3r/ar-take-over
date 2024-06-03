void MI3_AIGroupPerceptionOnEnemyLost(SCR_AIGroup group, SCR_AITargetInfo target);
typedef func MI3_AIGroupPerceptionOnEnemyLost;

void MI3_AIGroupPerceptionOnEnemyForgot(SCR_AIGroup group, SCR_AITargetInfo target);
typedef func MI3_AIGroupPerceptionOnEnemyForgot;

modded class SCR_AIGroupPerception : Managed
{
	protected ref ScriptInvokerBase<MI3_AIGroupPerceptionOnEnemyLost> Event_OnEnemyLost;
	protected ref ScriptInvokerBase<MI3_AIGroupPerceptionOnEnemyForgot> Event_OnEnemyForgot;
	
	ScriptInvokerBase<MI3_AIGroupPerceptionOnEnemyLost> GetOnEnemyLost()
	{
		if (!Event_OnEnemyLost)
			Event_OnEnemyLost = new ScriptInvokerBase<MI3_AIGroupPerceptionOnEnemyLost>();
		
		return Event_OnEnemyLost;
	}
	
	ScriptInvokerBase<MI3_AIGroupPerceptionOnEnemyForgot> GetOnEnemyForgot()
	{
		if (!Event_OnEnemyForgot)
			Event_OnEnemyForgot = new ScriptInvokerBase<MI3_AIGroupPerceptionOnEnemyForgot>();
		
		return Event_OnEnemyForgot;
	}
	
	//---------------------------------------------------------------------------------------------------
	// Adds or updates target from BaseTarget
	override SCR_AITargetInfo AddOrUpdateTarget(notnull BaseTarget target, out bool outNewTarget)
	{
		IEntity enemy = target.GetTargetEntity();
		
		if (!enemy)
		{
			outNewTarget = false;
			return null;
		}
			
		int id = m_aTargetEntities.Find(enemy);
		if (id > -1)
		{
			SCR_AITargetInfo oldTargetInfo = m_aTargets[id];
			EAITargetInfoCategory oldCategory = m_aTargets[id].m_eCategory;
			
			// Ignore if destroyed or disarmed
			if (oldCategory == EAITargetInfoCategory.DESTROYED || oldCategory == EAITargetInfoCategory.DISARMED)
			{
				outNewTarget = false;
				return oldTargetInfo;
			}
			
			if (oldCategory == EAITargetInfoCategory.DETECTED && target.GetTargetCategory() == ETargetCategory.ENEMY)
			{
				// Moved from DETECTED (gunshot) to IDENTIFIED (enemy)
				Event_OnEnemyDetected.Invoke(m_Group, oldTargetInfo);
			}
				
			// This target was already found
			// Which newTimestamp to use? Depends on target category
			float newTimestamp;
			ETargetCategory targetCategory = target.GetTargetCategory();
			if (targetCategory == ETargetCategory.DETECTED)
				newTimestamp = target.GetTimeLastDetected();
			else
				newTimestamp = target.GetTimeLastDetected();
			
			if (oldTargetInfo.m_fTimestamp < newTimestamp)
			{
				// New information is newer
				// Is new data more relevant?
				if ((targetCategory == ETargetCategory.ENEMY) || // If enemy, always update
					((targetCategory == ETargetCategory.DETECTED) && (oldCategory != EAITargetInfoCategory.IDENTIFIED)) )
				{
					oldTargetInfo.UpdateFromBaseTarget(target);
				}
			}
			
			outNewTarget = false;
			return oldTargetInfo;
		}		
		
		// new enemy found
		
		// Ignore if disarmed
		if (target.IsDisarmed())
		{
			outNewTarget = false;
			return null;
		}
			
		SCR_AITargetInfo targetInfo = new SCR_AITargetInfo();
		targetInfo.InitFromBaseTarget(target);
				
		m_aTargetEntities.Insert(enemy);
		m_aTargets.Insert(targetInfo);
		
		if (Event_OnEnemyDetected)
		{
			Event_OnEnemyDetected.Invoke(m_Group, targetInfo);
		}
		
		outNewTarget = true;
		return targetInfo;
	}
	
	//---------------------------------------------------------------------------------------------------
	override protected void MaintainTargets()
	{
		float timeNow;
		PerceptionManager pm = GetGame().GetPerceptionManager();
		if (pm)
			timeNow = pm.GetTime();
		
		for (int i = m_aTargets.Count()-1; i >= 0; i--)
		{
			SCR_AITargetInfo tgtInfo = m_aTargets[i];
			EAITargetInfoCategory category = tgtInfo.m_eCategory;
			if (timeNow - tgtInfo.m_fTimestamp > TARGET_FORGET_THRESHOLD_S)			// Time to forget?
			{
				RemoveTarget(i);
				
				if (Event_OnEnemyForgot)
					Event_OnEnemyForgot.Invoke(m_Group, tgtInfo);
			}
			else if (category == EAITargetInfoCategory.DISARMED)					// Disarmed?
			{
				if (tgtInfo.m_DamageManager && tgtInfo.m_DamageManager.IsDestroyed())	// Destroyed?
					tgtInfo.m_eCategory = EAITargetInfoCategory.DESTROYED;
				if (tgtInfo.m_Perceivable && !tgtInfo.m_Perceivable.IsDisarmed())		// Not disarmed any more?
					tgtInfo.m_eCategory = EAITargetInfoCategory.DETECTED;				// Back to detected
			}
			else if (category != EAITargetInfoCategory.DESTROYED)					// Not destroyed?
			{					
				if (tgtInfo.m_DamageManager && tgtInfo.m_DamageManager.IsDestroyed())	// Destroyed?
					tgtInfo.m_eCategory = EAITargetInfoCategory.DESTROYED;
				else if (!tgtInfo.m_Entity)												// Deleted? Treat as destroyed.
					tgtInfo.m_eCategory = EAITargetInfoCategory.DESTROYED;
				else if (timeNow - tgtInfo.m_fTimestamp > TARGET_LOST_THRESHOLD_S)		// Lost?
					tgtInfo.m_eCategory = EAITargetInfoCategory.LOST;
				else if (tgtInfo.m_Perceivable && tgtInfo.m_Perceivable.IsDisarmed())	// Disarmed?
					tgtInfo.m_eCategory = EAITargetInfoCategory.DISARMED;
			}
		}
	}
}
