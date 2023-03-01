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

	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete( player,position,orientation );
		string ent_type = GetType();
		
		if ( GetGame().IsServer() )
		{
			PlayerBase player_base = PlayerBase.Cast( player );
			vector pos = player_base.GetLocalProjectionPosition();
			PlayerIdentity pd = player_base.GetIdentity();
			string guid = pd.GetId();

			m_OwnerID = guid;
			//Print(m_OwnerID);

			ref BedData bed = new BedData(guid,pos);
			BedFrameWork.InsertBed( bed );
		}
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);

		ctx.Write(m_OwnerID);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
        return false;

		// read data loaded from game database (format and order of reading must be the same as writing!)
		if ( !ctx.Read(m_OwnerID) )
			return false;

		return true;
	}

	override void Pack( bool update_navmesh, bool init = false )
	{			
		super.Pack(update_navmesh,init);

		Print("Ran Pack for Sleeing Bag");

		Print(m_OwnerID);

		BedFrameWork.RemoveRespawnData( m_OwnerID );

		/*if ( BedFrameWork.BedClassNames.Get(this.GetType()) && BedFrameWork.BedClassNames.Get(this.GetType()) == 1 )
		{
			BedFrameWork.RemoveBedDataByVector(this.GetPosition());
		}*/
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		Print("Ran EEDelete for sleeping bag!");

		BedFrameWork.RemoveRespawnData( m_OwnerID );
	}
}

class BedData : BedFrameWork
{
	string m_BedOwner = "test";
	vector m_BedPos = "1 0 1";
	void BedData(string owner, vector pos)
	{
		m_BedOwner = owner;
		m_BedPos = pos;
	}
	string GetOwner()
	{
		return m_BedOwner;
	}
	vector GetPos()
	{
		return m_BedPos;
	}
}

class BedConfig
{
	int DestroyBedAfterSpawn = 1;
	int EnableOPBaseItems_SleepingBags = 1;
}


class BedFrameWork : Managed
{
	static autoptr ref map<string,vector> StoredBeds = new map<string,vector>;
	//static autoptr ref map<string,int> BedClassNames = new map<string,int>;

	static bool m_Loaded = false;
	
	protected static string m_Folder = "$profile:BedRespawn2\\";
	protected static string m_Config = m_Folder + "Config.json";
	protected static string m_DataFolder = m_Folder + "PlayerData\\";

	static ref BedConfig m_BedConfig = new BedConfig;

	void BedFrameWork()
	{
		m_Loaded = true;
		
		Print("Created BedFrameWork Object");

		//If the BedRespawn2 folder doesn't exist, then create it and create the Config.json file.
		if ( FileExist(m_Folder) == 0 )
		{
			Print("[Bed-Respawn 2.0] '" + m_Folder + "' does not exist, creating directory!");
            MakeDirectory(m_Folder);
            JsonFileLoader<BedConfig>.JsonSaveFile(m_Config, m_BedConfig);
        	Print(string.Format("[Bed-Respawn 2.0] saving config!"));
		}

		//If the playerdata folders dont exist, make them.
		if ( FileExist(m_DataFolder) == 0 )
		{
			MakeDirectory(m_DataFolder);
		} else {
			
		}
	}
	void ~BedFrameWork() {}

	static void InsertBed( BedData data )
	{
		string msg = "[Bed-Respawn 2.0] Bed has been placed by ";
		string name = data.GetOwner();
		string ext_msg = " @ Location :";
		
		Print( msg + name + ext_msg + data.GetPos() );
		
		if ( FileExist(m_DataFolder) )
		{
			StoredBeds.Insert( data.GetOwner(), data.GetPos() );
			string player_id = m_DataFolder + name + ".json"; 
			JsonFileLoader<BedData>.JsonSaveFile(player_id, data);
		}
		Print(data);
	}

	static vector AttemptBedSpawn( PlayerIdentity identity, vector DefaultPos, bool DestroyOldBed = true  )
	{
		if ( FileExist(m_DataFolder) )
		{
			string saved_bed = m_DataFolder + identity.GetId() + ".json"; 
			if ( FileExist(saved_bed) )
			{
				BedData bed = new BedData("na","1 0 1");
				JsonFileLoader<BedData>.JsonLoadFile(saved_bed, bed);
				StoredBeds.Insert( bed.GetOwner(), bed.GetPos() );

				DefaultPos = bed.GetPos();

				BreakOldSpawnBed(identity, DefaultPos)
			}
		}

		return DefaultPos;
	}

	static void FixSpawningHeight( PlayerBase ply, vector Location  )
	{
		ply.SetPosition( Location );
	}

	static void RemoveRespawnData( string guid )
	{
		if ( FileExist(m_DataFolder) )
		{
			string saved_bed = m_DataFolder + guid + ".json"; 
			if ( FileExist(saved_bed) )
			{
				DeleteFile(saved_bed)
			}
		}
	}

	static void BreakOldSpawnBed(PlayerIdentity identity, vector pos)
	{
		if ( m_BedConfig.DestroyBedAfterSpawn == 1 )
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
		}

		RemoveRespawnData( identity.GetId() );
	}

	static void RemoveBedDataByVector( vector pos )
	{
		foreach(string k, vector a: BedFrameWork.StoredBeds)
		{
			string BedPos = pos.ToString(false);
			string OldBedPos = a.ToString(false);

			if ( OldBedPos == BedPos )
			{
				BedFrameWork.StoredBeds.Remove(k);
				string msg = "[BedRespawning] Bed deleted ";
				string ext_msg = " @ Location :";
				
				Print( msg + ext_msg + a );
			}
		}
	}
}