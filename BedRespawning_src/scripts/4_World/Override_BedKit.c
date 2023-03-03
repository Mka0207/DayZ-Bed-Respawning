/*
 * Copyright (C) 2011-2020 FWKZT <master@fwkzt.com>
 * 
 * This file is part of Bed-Respawning.
 * 
 * Bed-Respawning can not be copied, repacked and/or modified without the express
 * permission of Scott Grissinger <https://steamcommunity.com/id/mka0207/> <g4.tyler@live.com>.
*/

/*modded class PlayerBase
{
	void SetPlayerPos( PlayerBase ply, vector Location )
	{
		ply.SetPosition(Location);
		ply.RPCSingleParam( ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>( "Fixed Spawn Location!" ), true, ply.GetIdentity() );
		Print("attempted to spawn player!");
	}
}*/

//Base Building Plus support
/*modded class ActionPlaceObject
{		
	override void OnFinishProgressServer( ActionData action_data )
	{	
		Print("[ActionPlaceObject] OnFinishProgressServer - BBP FIX");
		super.OnFinishProgressServer(action_data);
		//Print(m_CommandUID);
		
		if ( m_CommandUID == DayZPlayerConstants.CMD_ACTIONFB_DEPLOY_2HD )
		{
			ItemBase bed = action_data.m_MainItem;

			if ( bed.GetType() == "Base_SingleBed_Kit" || bed.GetType() == "sleepingbag_red_mung_Deployed" || bed.GetType() == "sleepingbag_blue_mung_Deployed" || bed.GetType() == "sleepingbag_green_mung_Deployed" || bed.GetType() == "sleepingbag_yellow_mung_Deployed" ) return;
			if ( bed.GetType() == "OP_SleepingBagBluePlacing" || bed.GetType() == "OP_SleepingBagGreyPlacing" || bed.GetType() == "OP_SleepingBagCamoPlacing" ) return;
			if ( bed.GetType() == "Chub_Bed_Kit" ) return;

			if ( BedFrameWork.BedClassNames.Get( bed.GetType() ) && BedFrameWork.BedClassNames.Get( bed.GetType() ) == 1 )
			{
				vector position = action_data.m_Player.GetLocalProjectionPosition();
				PlayerIdentity pd = action_data.m_Player.GetIdentity();
				
				BedFrameWork.InsertBed( position, pd );
				BedFrameWork.SaveBedData();
			}
		}
	}
}*/

//OP_BaseItems support
modded class OP_SleepingBagColorbase //modded class TentBase extends ItemBase
{
	string m_OwnerID;
	int m_Uses;

	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete( player,position,orientation );

		if ( GetGame().IsServer() )
		{
			PlayerBase player_base = PlayerBase.Cast( player );
			vector pos = player_base.GetLocalProjectionPosition();

			PlayerIdentity pd = player_base.GetIdentity();
			m_OwnerID = pd.GetId();

			m_Uses = BedFrameWork.m_BedConfig.MaxRespawnsBeforeRemoval;

			if ( BedFrameWork.m_BedConfig.BedRespawnTimeMinutes > 0 )
			{
				player_base.RPCSingleParam( ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>( "Bed-Respawn Cooldown after Death = "+BedFrameWork.m_BedConfig.BedRespawnTimeMinutes+" Minutes." ), true, pd );
			}

			ref BedData bed = new BedData(m_OwnerID,pos,0,m_Uses);
			BedFrameWork.InsertBed( bed );
		}
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);

		ctx.Write(m_OwnerID);
		ctx.Write(m_Uses);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
        return false;

		if ( !ctx.Read(m_OwnerID) )
			return false;

		if ( !ctx.Read(m_Uses) )
			return false;

		return true;
	}

	override void Pack( bool update_navmesh, bool init = false )
	{		
		if ( GetGame().IsServer() )
		{
			if ( GetState() == PITCHED && m_OwnerID != "" )
			{
				Print("[Bed-Respawn 2.0] Ran Pack for Sleeing Bag!");
				BedFrameWork.RemoveRespawnData( m_OwnerID );
			}
		}

		super.Pack(update_navmesh,init);
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		//if ( m_IsHologram ) return;

		if ( GetGame().IsServer() )
		{
			if ( GetState() == PITCHED && m_OwnerID != "" )
			{
				Print("[Bed-Respawn 2.0] Ran EEDelete for Sleeping Bag!");
				BedFrameWork.RemoveRespawnData( m_OwnerID );
			}
		}
	}
}

class BedData : BedFrameWork
{
	string m_BedOwner = "test";
	vector m_BedPos = "1 0 1";
	int m_RespawnTime = 0;
	int m_UsesLeft = 0;

	void BedData(string owner, vector pos, int time, int uses)
	{
		m_BedOwner = owner;
		m_BedPos = pos;
		m_RespawnTime = time;
		m_UsesLeft = uses;
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
		return m_UsesLeft;
	}
	void SetUsesLeft(int uses)
	{
		m_UsesLeft = uses;
	}
}

class BedConfig
{
	int MaxRespawnsBeforeRemoval = 0;
	int EnableOPBaseItems_SleepingBags = 1;
	int BedRespawnTimeMinutes = 45;
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

		//If the BedRespawn2 folder doesn't exist, then create it and create the Config.json file.
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

	static void InsertBed( BedData data )
	{
		string msg = "[Bed-Respawn 2.0] Bed has been placed by ";
		string name = data.GetOwner();
		string ext_msg = " @ Location :";
		
		Print( msg + name + ext_msg + data.GetPos() );
		
		if ( FileExist(m_DataFolder) )
		{
			string player_id = m_DataFolder + name + ".json"; 
			JsonFileLoader<BedData>.JsonSaveFile(player_id, data);
		}
		Print(data);
	}

	static void OnEquipCharacter( PlayerBase m_player )
	{
		private vector m_SpawnPos = m_player.GetPosition();
		private PlayerIdentity identity = m_player.GetIdentity();

		if ( FileExist(m_DataFolder) )
		{
			string saved_bed = m_DataFolder + identity.GetId() + ".json"; 
			if ( FileExist(saved_bed) )
			{
				BedData bed = new BedData("na","1 0 1", 0, 0);
				JsonFileLoader<BedData>.JsonLoadFile(saved_bed, bed);
				m_SpawnPos = bed.GetPos();
				
				if ( m_BedConfig.MaxRespawnsBeforeRemoval > 0 )
				{
					bed.SetUsesLeft( Math.Clamp( bed.GetUsesLeft() - 1, 0, m_BedConfig.MaxRespawnsBeforeRemoval ) )
					if ( bed.GetUsesLeft() <= 0 )
					{
						BreakOldSpawnBed(identity, m_SpawnPos);
					}
				}

				if ( m_BedConfig.BedRespawnTimeMinutes > 0 && CF_Date.Now(true).GetTimestamp() >= bed.GetRespawnTime() )
				{
					
					bed.SetRespawnTime( CF_Date.Now(true).GetTimestamp() + ( BedFrameWork.m_BedConfig.BedRespawnTimeMinutes * 60 ) );
					JsonFileLoader<BedData>.JsonSaveFile(saved_bed, bed);
				}

				m_player.SetPosition( m_SpawnPos );
			}
		}
	}

	static void RemoveRespawnData( string guid )
	{
		if ( FileExist(m_DataFolder) )
		{
			string saved_bed = m_DataFolder + guid + ".json"; 
			if ( FileExist(saved_bed) )
			{
				Print("[Bed-Respawn 2.0] Cleared JSON Bed Data for " + guid);
				DeleteFile(saved_bed);
			}
		}
	}

	static void BreakOldSpawnBed(PlayerIdentity identity, vector pos)
	{
		ref array<Object> Player_Bed = new array<Object>;
		GetGame().GetObjectsAtPosition( pos, 1.0, Player_Bed, NULL );
		for ( int i = 0; i < Player_Bed.Count(); i++ )
		{
			Object bed = Player_Bed.Get( i );
			//Print(bed.IsItemBase());
			//Print(bed.IsItemTent());
			if ( bed.IsItemBase() )
			{
				//Print("Object is ItemBase");
				if ( bed.IsInherited(OP_SleepingBagColorbase) )
				{
					//Print("Object is inherited from OP_SleepingBagColorbase")
					if ( bed.GetPosition().ToString(false) == pos.ToString(false) )
					{
						//Print("Found bed, deleting it!");
						GetGame().ObjectDelete(bed);
					}
				}
			}
		}
		RemoveRespawnData( identity.GetId() );
	}
}