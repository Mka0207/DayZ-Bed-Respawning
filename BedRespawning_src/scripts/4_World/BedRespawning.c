/*
 * Copyright (C) 2011-2023 FWKZT <master@fwkzt.com>
 * 
 * This file is part of Bed-Respawning.
 * 
 * Bed-Respawning can not be copied, repacked and/or modified without the express
 * permission of Scott Grissinger <https://steamcommunity.com/id/mka0207/> <g4.tyler@live.com>.
*/

//Support for our own beds.
modded class SleepingBagBase
{
	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete( player,position,orientation );

		if ( GetGame().IsServer() )
		{
			if ( SleepingBag_Deployed )
			{
				PlayerBase player_base = PlayerBase.Cast( player );
				PlayerIdentity pd = player_base.GetIdentity();
				vector pos = player_base.GetLocalProjectionPosition();

				SleepingBagBase_Deployed target = SleepingBagBase_Deployed.Cast( SleepingBag_Deployed );
				target.m_OwnerID = pd.GetId();
				target.m_Uses = BedFrameWork.m_BedConfig.MaxRespawnsBeforeRemoval;

				BedFrameWork.InsertBed( player_base, target.m_OwnerID, pos, 0, target.m_Uses );
			}
		}
	}
}

modded class SleepingBagBase_Deployed
{
	static string m_OwnerID;
	static int m_Uses;

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
}

modded class ActionPackRespawnBag
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		Object targetObject = action_data.m_Target.GetObject();
		SleepingBagBase_Deployed target = SleepingBagBase_Deployed.Cast( targetObject );

		if ( GetGame().IsServer() )
		{
			if (target && target.m_OwnerID != "" )
			{
				Print("[Bed-Respawn 2.0] Ran EEDelete for Sleeping Bag!");
				BedFrameWork.RemoveRespawnData( target.m_OwnerID );
			}
		}
		
		super.OnFinishProgressServer(action_data);
	}
}

//OP_BaseItems support
modded class TentBase //modded class OP_SleepingBagColorbase
{
	string m_OwnerID;
	int m_Uses;

	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete( player,position,orientation );
		if ( BedFrameWork.m_BedConfig.EnableOpBaseItemSupport == 0 ) return;

		if ( GetGame().IsServer() && ( this.GetType() == "OP_SleepingBagCamo" || this.GetType() == "OP_SleepingBagBlue" || this.GetType() == "OP_SleepingBagGrey" ) )
		{
			PlayerBase player_base = PlayerBase.Cast( player );
			PlayerIdentity pd = player_base.GetIdentity();
			vector pos = player_base.GetLocalProjectionPosition();

			m_OwnerID = pd.GetId();
			m_Uses = BedFrameWork.m_BedConfig.MaxRespawnsBeforeRemoval;

			BedFrameWork.InsertBed( player_base, m_OwnerID, pos, 0, m_Uses );
		}
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);

		if ( BedFrameWork.m_BedConfig.EnableOpBaseItemSupport == 0 ) return;
		if ( this.GetType() == "OP_SleepingBagCamo" || this.GetType() == "OP_SleepingBagBlue" || this.GetType() == "OP_SleepingBagGrey" )
		{
			ctx.Write(m_OwnerID);
			ctx.Write(m_Uses);
		}
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
        return false;

		if ( BedFrameWork.m_BedConfig.EnableOpBaseItemSupport == 0 ) return false;

		if ( this.GetType() == "OP_SleepingBagCamo" || this.GetType() == "OP_SleepingBagBlue" || this.GetType() == "OP_SleepingBagGrey" )
		{
			if ( !ctx.Read(m_OwnerID) )
				return false;

			if ( !ctx.Read(m_Uses) )
				return false;
		}

		return true;
	}

	override void Pack( bool update_navmesh, bool init = false )
	{		
		if ( GetGame().IsServer() && BedFrameWork.m_BedConfig.EnableOpBaseItemSupport == 1 && ( this.GetType() == "OP_SleepingBagCamo" || this.GetType() == "OP_SleepingBagBlue" || this.GetType() == "OP_SleepingBagGrey" ) )
		{
			if ( GetState() == PITCHED && m_OwnerID != "" )
			{
				Print("[Bed-Respawn 2.0] Ran Pack for Sleeping Bag!");
				BedFrameWork.RemoveRespawnData( m_OwnerID );
			}
		}

		super.Pack(update_navmesh,init);
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		if ( GetGame().IsServer() && ( this.GetType() == "OP_SleepingBagCamo" || this.GetType() == "OP_SleepingBagBlue" || this.GetType() == "OP_SleepingBagGrey" ) )
		{
			if ( BedFrameWork.m_BedConfig.EnableOpBaseItemSupport == 0 ) return;
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
	int BedRespawnTimeMinutes = 45;
	int EnableOpBaseItemSupport = 1;
	int HealthPercentage = 100;
	int EnergyPercentage = 100;
	int WaterPercentage = 100;
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

	static void InsertBed( PlayerBase player_base, string guid, vector bedpos, float cooldown, int uses )
	{
		ref BedData data = new BedData(guid,bedpos,cooldown,uses);

		string msg = "[Bed-Respawn 2.0] Bed has been placed by ";
		string ext_msg = " @ Location :";
		
		Print( msg + guid + ext_msg + bedpos );

		if ( BedFrameWork.m_BedConfig.BedRespawnTimeMinutes > 0 )
		{
			PlayerIdentity pd = player_base.GetIdentity();
			player_base.RPCSingleParam( ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>( "Bed-Respawn Cooldown after Death = "+BedFrameWork.m_BedConfig.BedRespawnTimeMinutes+" Minutes." ), true, pd );
		}
		
		if ( FileExist(m_DataFolder) )
		{
			string player_id = m_DataFolder + guid + ".json"; 
			JsonFileLoader<BedData>.JsonSaveFile(player_id, data);
		}
		//Print(data);
	}

	static void ApplyModifiers( PlayerBase pl )
	{
		pl.GetStatWater().Set( pl.GetStatWater().GetMax() * Math.Max( 0.1, m_BedConfig.WaterPercentage*0.01 ) );
		pl.GetStatEnergy().Set( pl.GetStatEnergy().GetMax() * Math.Max( 0.1, m_BedConfig.EnergyPercentage*0.01 ) );
		pl.SetHealth("", "Health", pl.GetMaxHealth("","Health") * Math.Max( 0.1, m_BedConfig.HealthPercentage*0.01 ) );
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

				if ( m_BedConfig.BedRespawnTimeMinutes > 0 && bed.GetRespawnTime() > CF_Date.Now(true).GetTimestamp() )
				{
					return;
				}
				
				if ( m_BedConfig.MaxRespawnsBeforeRemoval > 0 )
				{
					bed.SetUsesLeft( Math.Clamp( bed.GetUsesLeft() - 1, 0, m_BedConfig.MaxRespawnsBeforeRemoval ) )
					JsonFileLoader<BedData>.JsonSaveFile(saved_bed, bed);

					if ( bed.GetUsesLeft() <= 0 )
					{
						BreakOldSpawnBed(identity, m_SpawnPos);
						ApplyModifiers(m_player);
						m_player.SetPosition( m_SpawnPos );
						return;
					}
				}

				if ( m_BedConfig.MaxRespawnsBeforeRemoval > 1 && m_BedConfig.BedRespawnTimeMinutes > 0 && CF_Date.Now(true).GetTimestamp() >= bed.GetRespawnTime() )
				{
					bed.SetRespawnTime( CF_Date.Now(true).GetTimestamp() + ( BedFrameWork.m_BedConfig.BedRespawnTimeMinutes * 60 ) );
					JsonFileLoader<BedData>.JsonSaveFile(saved_bed, bed);
				}

				ApplyModifiers(m_player);
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
				if ( bed.IsInherited(SleepingBagBase_Deployed) || ( bed.GetType() == "OP_SleepingBagCamo" || bed.GetType() == "OP_SleepingBagBlue" || bed.GetType() == "OP_SleepingBagGrey" ) )
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