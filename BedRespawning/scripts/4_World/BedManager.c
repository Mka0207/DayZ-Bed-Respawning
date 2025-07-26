/*
* Copyright (C) 2025 fwkzt.com <https://steamcommunity.com/id/mka0207/>
* Licensed under "All Rights Reserved"
*/

class BedData : BedFrameWork
{
	private string m_BedOwner = "test";
	private vector m_BedPos = "1 0 1";
	private int m_RespawnTime = 0;
	private int m_BRUsesLeft = 0;

	void BedData(string owner, vector pos, int time, int uses)
	{
		m_BedOwner = owner;
		m_BedPos = pos;
		m_RespawnTime = time;
		m_BRUsesLeft = uses;
	}
	string GetOwner()
	{
		return m_BedOwner;
	}
	vector GetPos()
	{
		return m_BedPos;
	}
	int GetRespawnTime()
	{
		return m_RespawnTime;
	}
	void SetRespawnTime(int time)
	{
		m_RespawnTime = time;
	}
	int GetUsesLeft()
	{
		return m_BRUsesLeft;
	}
	void SetUsesLeft(int uses)
	{
		m_BRUsesLeft = uses;
	}
}

class BedConfig
{
	int MaxRespawnsBeforeRemoval = 0;
	int BedRespawnTimeMinutes = 45;
	int HealthPercentage = 100;
	int EnergyPercentage = 100;
	int WaterPercentage = 100;
	int DisableAutoSpawnCall = 0;

	int EnableOpBaseItemSupport = 1;
	int EnableBBP = 1;
	int EnableMung = 1;
	int EnableZen = 1;
	int EnableBoomlay = 1;
	int EnableRagBase = 1;
}

class BedFrameWork : Managed
{
	protected static bool m_Loaded = false;
	protected static string m_Folder = "$profile:BedRespawn2\\";
	protected static string m_Config = m_Folder + "Config.json";
	protected static string m_DataFolder = m_Folder + "PlayerData\\";
	static ref BedConfig m_BedConfig = new BedConfig;

	void BedFrameWork()
	{
		if ( m_Loaded ) return;
		
		Print("[Bed-Respawn 2.0] Created BedFrameWork Instance!");

		// If the BedRespawn2 folder doesn't exist, then create it and create the Config.json file.
		if ( FileExist(m_Folder) == 0 )
		{
			Print("[Bed-Respawn 2.0] '" + m_Folder + "' does not exist, creating directory!");
            MakeDirectory(m_Folder);
			LoadConfig();
			CreateDataFolder();
		} else {
			LoadConfig();
			CreateDataFolder();
		}
		
		m_Loaded = true;
	}
	void ~BedFrameWork() {}

	void LoadConfig()
	{
		if ( FileExist(m_Config) == 0 )
		{
			Print("[Bed-Respawn 2.0] no config file found! creating...");
			JsonFileLoader<BedConfig>.JsonSaveFile(m_Config, m_BedConfig);
		} else {
			Print("[Bed-Respawn 2.0] loading prexisting config file.");
			JsonFileLoader<BedConfig>.JsonLoadFile(m_Config, m_BedConfig);
		}
	}

	void CreateDataFolder()
	{
		if ( FileExist(m_DataFolder) == 0 )
		{
			MakeDirectory(m_DataFolder);
		}
	}

	static void GetBedData( string guid, out vector bedpos, out float cooldown, out int uses )
	{
		if ( !guid ) return;

		string saved_bed = m_DataFolder + guid + ".json"; 
		if ( FileExist(saved_bed) )
		{
			BedData bed = new BedData("na","1 0 1", 0, 0);
			JsonFileLoader<BedData>.JsonLoadFile(saved_bed, bed);

			bedpos = bed.GetPos();
			cooldown = bed.GetRespawnTime();
			uses = bed.GetUsesLeft();
		}
	}

	static bool HasBedData( string guid )
	{
		if ( !guid ) return false;

		string saved_bed = m_DataFolder + guid + ".json"; 
		if ( FileExist(saved_bed) )
		{
			return true;
		}

		return false;
	}

	static int GetBedUses( string guid )
	{
		if ( !guid ) return 0;

		string saved_bed = m_DataFolder + guid + ".json"; 
		if ( FileExist(saved_bed) )
		{
			BedData bed = new BedData("na","1 0 1", 0, 0);
			JsonFileLoader<BedData>.JsonLoadFile(saved_bed, bed);

			return bed.GetUsesLeft();
		}

		return 0;
	}

	static vector GetUserBedPosition( string guid )
	{
		if ( !guid ) return "0 0 0";

		string saved_bed = m_DataFolder + guid + ".json"; 
		if ( FileExist(saved_bed) )
		{
			BedData bed = new BedData("na","1 0 1", 0, 0);
			JsonFileLoader<BedData>.JsonLoadFile(saved_bed, bed);

			return bed.GetPos();
		}

		return "0 0 0";
	}

	static int GetUserRespawnTime( string guid )
	{
		string saved_bed = m_DataFolder + guid + ".json"; 
		if ( FileExist(saved_bed) )
		{
			BedData bed = new BedData("na","1 0 1", 0, 0);
			JsonFileLoader<BedData>.JsonLoadFile(saved_bed, bed);

			return bed.GetRespawnTime();
		}

		return 0;
	}

	static void WriteBedData( string guid, ref BedData bed )
	{
		if ( !guid ) return;

		if ( FileExist(m_DataFolder) )
		{
			if ( !bed )
			{
				bed = new BedData("na", "1 0 1", 0, 0);
			}
			string saved_bed = m_DataFolder + guid + ".json"; 
			JsonFileLoader<BedData>.JsonSaveFile(saved_bed, bed);
		}
	}

	static void DeleteBedData( string guid, vector pos )
	{
		if ( HasBedData(guid) )
		{
			string saved_bed = m_DataFolder + guid + ".json"; 

			if ( pos )
			{
				vector bedpos = GetUserBedPosition(guid);

				if ( bedpos.ToString(false) == pos.ToString(false) )
				{
					DeleteFile(saved_bed);
					Print("[Bed-Respawn 2.0] Cleared JSON Bed Data for " + guid + " at position: " + pos.ToString(false));
				}
			}
			else
			{
				DeleteFile(saved_bed);
				Print("[Bed-Respawn 2.0] Cleared JSON Bed Data for " + guid);
			}
		}
	}

	static void RegisterSpawnClaim( PlayerBase player, BedData bed )
	{
		PlayerIdentity pd = player.GetIdentity();
		
		if ( !pd ) return;
		if ( !bed ) return;
		
		string msg = "[Bed-Respawn 2.0] Bed has been placed by ";
		string ext_msg = " @ Location :";
		string guid = pd.GetId();
		
		Print( msg + guid + ext_msg + bed.GetPos() );

		if ( bed.GetRespawnTime() > 0 )
		{
			player.RPCSingleParam( ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>( "Bed-Respawn location set, cooldown set to "+BedFrameWork.m_BedConfig.BedRespawnTimeMinutes+" minutes." ), true, pd );
		}
		else
		{
			player.RPCSingleParam( ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>( "Bed-Respawn location set, no cool down." ), true, pd );
		}
		
		WriteBedData( guid, bed );
	}

	static void ApplyPlayerStatModifiers( PlayerBase pl )
	{
		pl.GetStatWater().Set( pl.GetStatWater().GetMax() * Math.Max( 0.1, m_BedConfig.WaterPercentage*0.01 ) );
		pl.GetStatEnergy().Set( pl.GetStatEnergy().GetMax() * Math.Max( 0.1, m_BedConfig.EnergyPercentage*0.01 ) );
		pl.SetHealth("", "Health", pl.GetMaxHealth("","Health") * Math.Max( 0.1, m_BedConfig.HealthPercentage*0.01 ) );
	}

	static void NotifyCooldownMessage(PlayerBase player)
	{
		if (!player)
		{
			return;
		}

		int respawnTime = GetUserRespawnTime(player.GetIdentity().GetId());
		
		int currentTime = CF_Date.Now(true).GetTimestamp();
		int timeRemaining = respawnTime - currentTime;

		string message;
		
		if (timeRemaining <= 0)
		{
			message = "Ready to pack or claim now!";
		}
		else 
		{
			int minutes = timeRemaining / 60;
			int seconds = timeRemaining % 60;
			
			if (minutes > 0)
			{
				message = "Can pack or claim in " + minutes.ToString() + " minute";
				if (minutes != 1)
				{
					message += "s";
				}
				
				if (seconds > 0)
				{
					message += " and " + seconds.ToString() + " second";
					if (seconds != 1)
					{
						message += "s";
					}
				}
			}
			else
			{
				message = "Can pack or claim in " + seconds.ToString() + " second";
				if (seconds != 1)
				{
					message += "s";
				}
			}
		}

		Param1<string> param = new Param1<string>(message);
		player.RPCSingleParam(ERPCs.RPC_USER_ACTION_MESSAGE, param, true, player.GetIdentity());
	}

	static bool HasCooldown(PlayerBase m_player, vector pos)
	{
		if (!m_player)
			return false;

		string guid = m_player.GetIdentity().GetId();
		vector bedPos = GetUserBedPosition(guid);

		int respawnTime = GetUserRespawnTime(guid);
		int currentTime = CF_Date.Now(true).GetTimestamp();

		Print("Respawn time: " + respawnTime);
		Print("Current time: " + currentTime);

		// If current time is before respawn time, cooldown is active
		if ( respawnTime != 0 && currentTime < respawnTime && bedPos.ToString(false) == pos.ToString(false) )
			return true;

		return false;
	}

	static BedData LoadBedData(string playerId)
	{
		string filePath = m_DataFolder + playerId + ".json";
		BedData bed = new BedData("na", "1 0 1", 0, 0); // Default values
		
		if (FileExist(filePath))
		{
			JsonFileLoader<BedData>.JsonLoadFile(filePath, bed);
		}
		else
		{
			Print("[BedData] No bed data found for player: " + playerId);
		}
		
		return bed;
	}

	static void TeleportPlayer( PlayerBase m_player )
	{
		if ( m_BedConfig.DisableAutoSpawnCall == 1 ) return;
		
		PlayerIdentity identity = m_player.GetIdentity();
		string guid = identity.GetId();
		if ( HasBedData(guid) )
		{
			BedData bed = LoadBedData(guid);
			vector m_SpawnPos = bed.GetPos();

			// If the player's respawn time cool down hasn't expired, do not set spawn position
			if ( m_BedConfig.BedRespawnTimeMinutes > 0 && bed.GetRespawnTime() > CF_Date.Now(true).GetTimestamp() )
			{
				return;
			}
			
			// Handle Bed uses left
			if ( m_BedConfig.MaxRespawnsBeforeRemoval > 0 )
			{
				bed.SetUsesLeft( Math.Clamp( bed.GetUsesLeft() - 1, 0, m_BedConfig.MaxRespawnsBeforeRemoval ) );
				WriteBedData( guid, bed );

				// Delete the bed if uses left are empty
				if ( bed.GetUsesLeft() <= 0 )
				{
					DeleteBedItem(guid, m_SpawnPos);
					ApplyPlayerStatModifiers(m_player);
					m_player.SetPosition( m_SpawnPos );
					return;
				}
			}

			// If the bed respawn time is set, update the respawn time
			if ( m_BedConfig.BedRespawnTimeMinutes > 0 && CF_Date.Now(true).GetTimestamp() >= bed.GetRespawnTime() )
			{
				if ( m_BedConfig.MaxRespawnsBeforeRemoval == 1 ) return;
				
				bed.SetRespawnTime( CF_Date.Now(true).GetTimestamp() + ( BedFrameWork.m_BedConfig.BedRespawnTimeMinutes * 60 ) );
				WriteBedData( guid, bed );
			}

			ApplyPlayerStatModifiers(m_player);
			m_player.SetPosition( m_SpawnPos );
		}
	}

	static bool ClassDeleteFilter(string type)
	{
		return false; // This is a placeholder for any specific class deletion logic you might want to implement.
	}

	static void DeleteBedItem(string guid, vector pos)
	{
		ref array<Object> Player_Bed = new array<Object>;
		GetGame().GetObjectsAtPosition(pos, 1.0, Player_Bed, NULL);
		for (int i = 0; i < Player_Bed.Count(); i++)
		{
			Object bed = Player_Bed.Get(i);
			if (bed.IsItemBase())
			{
				// Check if GetBedOwner exists and is not empty
				if ( bed.GetPosition().ToString(false) == pos.ToString(false) )
				{
					GetGame().ObjectDelete(bed);
				}
			}
		}
		DeleteBedData(guid, pos);
	}
}