/*
 * Copyright (C) 2011-2023 FWKZT <master@fwkzt.com>
 * 
 * This file is part of Bed-Respawning.
 * 
 * Bed-Respawning can not be copied, repacked and/or modified without the express
 * permission of Scott Grissinger <https://steamcommunity.com/id/mka0207/> <g4.tyler@live.com>.
*/

//Base Building Plus support
modded class ActionPlaceObject
{		
	override void OnFinishProgressServer( ActionData action_data )
	{	
		super.OnFinishProgressServer(action_data);

		if ( m_CommandUID == DayZPlayerConstants.CMD_ACTIONFB_DEPLOY_2HD )
		{
			ItemBase bed = action_data.m_MainItem;
			if ( BedFrameWork.m_BedConfig.EnableBBP && bed.GetType() == "BBP_BedKit" )
			{
				//Print("ActionPlaceObject " + bed);
				Print("[ActionPlaceObject] OnFinishProgressServer - BBP FIX");

				PlayerIdentity pd = action_data.m_Player.GetIdentity();
				vector pos = action_data.m_Player.GetPosition();

				bed.SetBedOwner(pd.GetId());
				bed.SetBedUses(BedFrameWork.m_BedConfig.MaxRespawnsBeforeRemoval);

				BedFrameWork.InsertBed( action_data.m_Player, bed.GetBedOwner(), pos, 0, bed.GetBedUses() );
			}
		}
	}
}

//Mung and BPP support continued.
modded class ItemBase
{
	private string m_OwnerBRGUID;
	private int m_BRUses;

	string GetBedOwner()
	{
		return m_OwnerBRGUID;
	}
	void SetBedOwner(string guid)
	{
		m_OwnerBRGUID = guid;
	}

	int GetBedUses()
	{
		return m_BRUses;
	}
	void SetBedUses(int uses)
	{
		m_BRUses = uses;
	}

	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete( player,position,orientation );

		if ( GetGame().IsServer() )
		{
			if ( BedFrameWork.IsPackedMungItem(this.GetType()) )
			{
				PlayerBase player_base = PlayerBase.Cast( player );
				PlayerIdentity pd = player_base.GetIdentity();
				//vector pos = player_base.GetLocalProjectionPosition();

				m_OwnerBRGUID = pd.GetId();
				m_BRUses = BedFrameWork.m_BedConfig.MaxRespawnsBeforeRemoval;

				BedFrameWork.InsertBed( player_base, m_OwnerBRGUID, position, 0, m_BRUses );
			}
		}
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		if ( this.IsHologram() ) return;

		//Print(this.GetType());

		if ( GetGame().IsServer() )
		{
			if ( BedFrameWork.m_BedConfig.EnableBBP && this.GetType() == "BBP_Bed" || BedFrameWork.m_BedConfig.EnableMung && BedFrameWork.IsDeployedMungItem(this.GetType()) )
			{
				if ( m_OwnerBRGUID != "" )
				{
					Print("[Bed-Respawn 2.0] Ran EEDelete for BBP Bed!");
					BedFrameWork.RemoveRespawnData( m_OwnerBRGUID );
				}
			}
		}
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);

		if ( BedFrameWork.m_BedConfig.EnableBBP && this.GetType() == "BBP_Bed" || BedFrameWork.m_BedConfig.EnableMung && BedFrameWork.IsDeployedMungItem(this.GetType()) )
		{
			ctx.Write(m_OwnerBRGUID);
			ctx.Write(m_BRUses);
		}
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
        return false;

		if ( BedFrameWork.m_BedConfig.EnableBBP && this.GetType() == "BBP_Bed" || BedFrameWork.m_BedConfig.EnableMung && BedFrameWork.IsDeployedMungItem(this.GetType()) )
		{
			if ( !ctx.Read(m_OwnerBRGUID) )
				return false;

			if ( !ctx.Read(m_BRUses) )
				return false;
		}

		return true;
	}
}

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
				//vector pos = player_base.GetLocalProjectionPosition();

				SleepingBagBase_Deployed target = SleepingBagBase_Deployed.Cast( SleepingBag_Deployed );
				target.SetBedOwner(pd.GetId());
				target.SetBedUses(BedFrameWork.m_BedConfig.MaxRespawnsBeforeRemoval);


				//Print(pos);

				BedFrameWork.InsertBed( player_base, target.GetBedOwner(), position, 0, target.GetBedUses() );
			}
		}
	}
}

modded class SleepingBagBase_Deployed
{
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);

		ctx.Write(GetBedOwner());
		ctx.Write(GetBedUses());
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
        return false;

		if ( !ctx.Read(GetBedOwner()) )
			return false;

		if ( !ctx.Read(GetBedUses()) )
			return false;

		return true;
	}
	
	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		if ( GetGame().IsServer() )
		{
			if ( this.IsHologram() ) return;

			if ( GetBedOwner() != "" )
			{
				Print("[Bed-Respawn 2.0] Ran EEDelete for Sleeping Bag!");
				BedFrameWork.RemoveRespawnData( GetBedOwner() );
			}
		}
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
			if (target && target.GetBedOwner() != "" )
			{
				Print("[Bed-Respawn 2.0] Ran ActionPackRespawnBag for Sleeping Bag!");
				BedFrameWork.RemoveRespawnData( target.GetBedOwner() );
			}
		}
		
		super.OnFinishProgressServer(action_data);
	}
}

//OP_BaseItems support
#ifdef OP_BaseItems
modded class TentBase
{
	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete( player,position,orientation );
		if ( BedFrameWork.m_BedConfig.EnableOpBaseItemSupport == 0 ) return;

		if ( GetGame().IsServer() && BedFrameWork.IsOPBaseItem(this.GetType()) )
		{
			PlayerBase player_base = PlayerBase.Cast( player );
			PlayerIdentity pd = player_base.GetIdentity();
			vector pos = player_base.GetLocalProjectionPosition();

			this.SetBedOwner(pd.GetId());
			this.SetBedUses(BedFrameWork.m_BedConfig.MaxRespawnsBeforeRemoval);

			BedFrameWork.InsertBed( player_base, this.GetBedOwner(), pos, 0, this.GetBedUses() );
		}
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);

		if ( BedFrameWork.m_BedConfig.EnableOpBaseItemSupport == 0 ) return;
		if ( BedFrameWork.IsOPBaseItem(this.GetType()) )
		{
			ctx.Write(this.GetBedOwner());
			ctx.Write(this.GetBedUses());
		}
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
        return false;

		if ( BedFrameWork.m_BedConfig.EnableOpBaseItemSupport == 0 ) return false;

		if ( BedFrameWork.IsOPBaseItem(this.GetType()) )
		{
			if ( !ctx.Read(this.GetBedOwner()) )
				return false;

			if ( !ctx.Read(this.GetBedUses()) )
				return false;
		}

		return true;
	}

	override void Pack( bool update_navmesh, bool init = false )
	{		
		if ( GetGame().IsServer() && BedFrameWork.m_BedConfig.EnableOpBaseItemSupport == 1 && BedFrameWork.IsOPBaseItem(this.GetType()) )
		{
			if ( GetState() == PITCHED && this.GetBedOwner() != "" )
			{
				Print("[Bed-Respawn 2.0] Ran Pack for Sleeping Bag!");
				BedFrameWork.RemoveRespawnData( this.GetBedOwner() );
			}
		}

		super.Pack(update_navmesh,init);
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		if ( GetGame().IsServer() && BedFrameWork.IsOPBaseItem(this.GetType()) )
		{
			if ( BedFrameWork.m_BedConfig.EnableOpBaseItemSupport == 0 ) return;
			if ( GetState() == PITCHED && this.GetBedOwner() != "" )
			{
				Print("[Bed-Respawn 2.0] Ran EEDelete for Sleeping Bag!");
				BedFrameWork.RemoveRespawnData( this.GetBedOwner() );
			}
		}
	}
}
#endif

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
	int EnableOpBaseItemSupport = 1;
	int HealthPercentage = 100;
	int EnergyPercentage = 100;
	int WaterPercentage = 100;
	int DisableAutoSpawnCall = 0;
	int EnableBBP = 1;
	int EnableMung = 1;
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
					bed.SetUsesLeft( Math.Clamp( bed.GetUsesLeft() - 1, 0, m_BedConfig.MaxRespawnsBeforeRemoval ) );
					JsonFileLoader<BedData>.JsonSaveFile(saved_bed, bed);

					if ( bed.GetUsesLeft() <= 0 )
					{
						BreakOldSpawnBed(identity, m_SpawnPos);
						ApplyModifiers(m_player);
						m_player.SetPosition( m_SpawnPos );
						return;
					}
				}

				if ( m_BedConfig.BedRespawnTimeMinutes > 0 && CF_Date.Now(true).GetTimestamp() >= bed.GetRespawnTime() )
				{
					if ( m_BedConfig.MaxRespawnsBeforeRemoval == 1 ) return;
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
				if ( bed.IsInherited(SleepingBagBase_Deployed) || BedFrameWork.IsDeployedMungItem(bed.GetType()) || BedFrameWork.IsOPBaseItem(bed.GetType()) || bed.GetType() == "BBP_Bed" )
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

	static bool IsOPBaseItem(string c)
	{
		return c == "OP_SleepingBagCamo" || c == "OP_SleepingBagBlue" || c == "OP_SleepingBagGrey";
	}

	static bool IsDeployedMungItem(string c)
	{
		return c == "sleepingbag_red_mung_Deployed" || c == "sleepingbag_blue_mung_Deployed" || c == "sleepingbag_green_mung_Deployed" || c == "sleepingbag_yellow_mung_Deployed";
	}

	static bool IsPackedMungItem(string c)
	{
		return c == "sleepingbag_red_mung" || c == "sleepingbag_blue_mung" || c == "sleepingbag_green_mung" || c == "sleepingbag_yellow_mung";
	}

	static bool IsMuchStuffItem(string c)
	{
		return c == "Msp_SleepingBag_Green" || c == "Msp_SleepingBag_Blue" || c == "Msp_SleepingBag_LimeGreen" || c == "Msp_SleepingBag_Orange" || c == "Msp_SleepingBag_Purple" || c == "Msp_SleepingBag_Red";
	}
}