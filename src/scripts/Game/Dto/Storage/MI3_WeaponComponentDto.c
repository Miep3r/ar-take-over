class MI3_WeaponComponentDto : MI3_InventoryComponentDto
{
	protected ref array<ref MI3_MuzzleComponentDto> m_aMuzzles;
	protected ref array<ref MI3_AttachmentComponentDto> m_aAttachments;
	
	void SetMuzzles(array<ref MI3_MuzzleComponentDto> muzzles)
	{
		m_aMuzzles = muzzles;
	}
	
	array<ref MI3_MuzzleComponentDto> GetMuzzles()
	{
		return m_aMuzzles;
	}
	
	void SetAttachments(array<ref MI3_AttachmentComponentDto> attachments)
	{
		m_aAttachments = attachments;
	}
	
	array<ref MI3_AttachmentComponentDto> GetAttachments()
	{
		return m_aAttachments;
	}
	
	override bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		bool success = super.SerializationSave(saveContext);
		
		if (!success)
			return false;
		
		saveContext.WriteValue("muzzles", m_aMuzzles);
		saveContext.WriteValue("attach", m_aAttachments);
		
		return true;
	}
	
	override bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		bool success = super.SerializationLoad(loadContext);
		
		if (!success)
			return false;
		
		loadContext.ReadValue("muzzles", m_aMuzzles);
		loadContext.ReadValue("attach", m_aAttachments);
		
		return true;
	}
}
