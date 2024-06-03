class MI3_GarrisonUnitSpawnRequest
{
	ResourceName PrefabResourceName;
	vector Offset[4];
}

class MI3_GarrisonSpawnHandlerComponentClass : MI3_GarrisonComponentClass
{
}

class MI3_GarrisonSpawnHandlerComponent : MI3_GarrisonComponent
{
	[Attribute("10")]
	protected float m_fMaximumSpawnRadius;
	
	protected ref array<ResourceName> m_aUnitsToSpawn;
	
	SCR_AIGroup SpawnGroup(ResourceName groupResourceName, array<ResourceName> resourceNames)
	{
		SCR_AIGroup group = SpawnGroupPrefab(groupResourceName);
		
		RplComponent rplComponent = RplComponent.Cast(group.FindComponent(RplComponent));
		if (rplComponent)
			rplComponent.EnableStreaming(true);
		
		AIPathfindingComponent pathFindindingComponent = AIPathfindingComponent.Cast(group.FindComponent(AIPathfindingComponent));
		
		foreach (ResourceName name : resourceNames)
		{
			if (name == ResourceName.Empty)
				continue;
			
			SCR_ChimeraCharacter unit = SpawnUnit(pathFindindingComponent, name);
			
			group.AddAgentFromControlledEntity(unit);
		}
		
		return group;
	}
	
	//---------------------------------------------------
	private SCR_ChimeraCharacter SpawnUnit(AIPathfindingComponent pathFindindingComponent, ResourceName resourceName)
	{
		vector mat[4];
		m_Garrison.GetTransform(mat);
		
		vector pos = mat[3];
		pos[0] = pos[0] + Math.RandomFloat(-m_fMaximumSpawnRadius, m_fMaximumSpawnRadius);
		pos[2] = pos[2] + Math.RandomFloat(-m_fMaximumSpawnRadius, m_fMaximumSpawnRadius);
		pos[1] = GetGame().GetWorld().GetSurfaceY(pos[0], pos[2]);
		
		pathFindindingComponent.GetClosestPositionOnNavmesh(pos, "10 10 10", pos);
		
		mat[3] = pos;
		
		EntitySpawnParams params = new EntitySpawnParams();
		params.Transform = mat;
		
		Resource resource = Resource.Load(resourceName);
		if (!resource)
			return null;
		
		IEntity entity = GetGame().SpawnEntityPrefab(resource, GetOwner().GetWorld(), params);
		
		AIControlComponent control = AIControlComponent.Cast(entity.FindComponent(AIControlComponent));
		
		if (!control)
			return null;
		
		control.ActivateAI();
		
		return SCR_ChimeraCharacter.Cast(entity);
	} 
	
	//---------------------------------------------------
	private SCR_AIGroup SpawnGroupPrefab(ResourceName groupResourceName)
	{	
		Resource resource = Resource.Load(groupResourceName);
		IEntity entity = GetGame().SpawnEntityPrefab(resource);
		
		if (!entity)
			return null;
		
		return SCR_AIGroup.Cast(entity);
	}
}
