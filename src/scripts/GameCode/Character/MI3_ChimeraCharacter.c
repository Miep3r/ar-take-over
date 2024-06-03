modded class SCR_ChimeraCharacter : ChimeraCharacter
{
	void SetAffiliatedFactionByKey(FactionKey key)
	{
		m_pFactionComponent.SetAffiliatedFactionByKey(key);
		Print("Updated player faction to: " + key);
	}
}
