class MI3_SavegameUtility
{
	private static const string BASE_DIR = "$profile:.save/MI3/";
	private static const string FILETYPE = "json";
	
	static MI3_SavegameMetadata GetSavegames()
	{
		if (FileIO.FileExists(BASE_DIR))
		{
			MI3_SavegameMetadata metadata = new MI3_SavegameMetadata();
			
			SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
			loadContext.LoadFromFile(BASE_DIR + "Savegames.json");
			
			metadata.SerializationLoad(loadContext);
			
			return metadata;
		}
		else
		{
			bool success = FileIO.MakeDirectory(BASE_DIR);
			
			if (!success)
				return null;
			
			return new MI3_SavegameMetadata();
		}
	}
	
	static void SaveMetadata(MI3_SavegameMetadata metadata)
	{
		SCR_JsonSaveContext saveContext = new SCR_JsonSaveContext();
		
		metadata.SerializationSave(saveContext);
		
		saveContext.SaveToFile(BASE_DIR + "Savegames.json");
	}
	
	static void Init(string name)
	{
		MI3_GameMode gm = MI3_GameMode.Cast(GetGame().GetGameMode());
		
		MI3_SavegameData savegame = new MI3_SavegameData();
		
		gm.Load(savegame);
		gm.InitSavegame(name);
	}
	
	static void Load(string name)
	{
		if (!FileIO.FileExists(BASE_DIR + name + "." + FILETYPE))
		{
			Init(name);
			return;
		}
		
		MI3_GameMode gm = MI3_GameMode.Cast(GetGame().GetGameMode());
		
		SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
		loadContext.LoadFromFile(BASE_DIR + name + "." + FILETYPE);
		
		MI3_SavegameData savegame = new MI3_SavegameData();
		savegame.SerializationLoad(loadContext);
		
		gm.Load(savegame);
	}
	
	static void Save(string name)
	{
		MI3_GameMode gm = MI3_GameMode.Cast(GetGame().GetGameMode());
		
		MI3_SavegameData savegame = new MI3_SavegameData();
		gm.Save(savegame);
		
		SCR_JsonSaveContext saveContext = new SCR_JsonSaveContext();
		savegame.SerializationSave(saveContext);
		
		saveContext.SaveToFile(BASE_DIR + name + "." + FILETYPE);
	}
}
