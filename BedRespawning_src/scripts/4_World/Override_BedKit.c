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
modded class ActionPlaceObject
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
}

//MunghardsItempack support
modded class ActionContinuousBase
{
	override void OnFinishProgressServer( ActionData action_data )
	{	
		Print("[ActionContinuousBase] OnFinishProgressServer - Mung FIX");
		super.OnFinishProgressServer(action_data);
		if ( m_CommandUID == DayZPlayerConstants.CMD_ACTIONFB_CRAFTING )
		{
			Object bed = action_data.m_Target.GetObject();
			//Print( bed.GetType() );

			if ( BedFrameWork.BedClassNames.Get(bed.GetType()) && BedFrameWork.BedClassNames.Get(bed.GetType()) == 1 )
			{
				vector pos = action_data.m_Player.GetLocalProjectionPosition();
				PlayerIdentity pd = action_data.m_Player.GetIdentity();
				
				BedFrameWork.InsertBed( pos, pd );
				BedFrameWork.SaveBedData();
			}
		}
	}
}

//OP_BaseItems support
modded class TentBase extends ItemBase
{
	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete( player,position,orientation );
		string ent_type = GetType();
		
		if ( GetGame().IsServer() )
		{
			if ( BedFrameWork.BedClassNames.Get(ent_type) && BedFrameWork.BedClassNames.Get(ent_type) == 1 )
			{
				PlayerBase player_base = PlayerBase.Cast( player );
				vector pos = player_base.GetLocalProjectionPosition();
				PlayerIdentity pd = player_base.GetIdentity();

				BedFrameWork.InsertBed( pos, pd );
				BedFrameWork.SaveBedData();
			}
		}
	}

	override void Pack( bool update_navmesh, bool init = false )
	{			
		super.Pack(update_navmesh,init);
		if ( BedFrameWork.BedClassNames.Get(this.GetType()) && BedFrameWork.BedClassNames.Get(this.GetType()) == 1 )
		{
			BedFrameWork.RemoveBedDataByVector(this.GetPosition());
		}
	}
}

//Base_Storage support and rest cleanup.
modded class ItemBase extends InventoryItem
{
	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete(player,position,orientation);
		
		string ent_type = GetType();

		if ( GetGame().IsServer() )
		{
			if ( BedFrameWork.BedClassNames.Get(ent_type) && BedFrameWork.BedClassNames.Get(ent_type) == 1 )
			{
				PlayerBase player_base = PlayerBase.Cast( player );
				vector pos = player_base.GetLocalProjectionPosition();
				PlayerIdentity pd = player_base.GetIdentity();

				BedFrameWork.InsertBed( pos, pd );
				BedFrameWork.SaveBedData();
			}
		}
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		//Print( this.GetType() );

		if ( this.GetType() == "Base_SingleBed" || this.GetType() == "BBP_Bed" || this.GetType() == "sleepingbag_red_mung_Deployed" || this.GetType() == "sleepingbag_blue_mung_Deployed" || this.GetType() == "sleepingbag_green_mung_Deployed" || this.GetType() == "sleepingbag_yellow_mung_Deployed" || this.GetType() == "OP_SleepingBagBlue" || this.GetType() == "OP_SleepingBagGrey" || this.GetType() == "OP_SleepingBagCamo" || this.GetType() == "Chub_Bed_UnPacked" )
		{
			BedFrameWork.RemoveBedDataByVector(this.GetPosition());
		}
	}
}

class BedFrameWork : Managed
{
	static autoptr ref map<string,vector> StoredBeds = new map<string,vector>;

	static autoptr ref map<string,int> BedClassNames = new map<string,int>;

	static bool m_Loaded = false;

	static string TextFileName = "BedRespawn/BedData";

	static string ConfigFileName = "BedRespawn/BedDataConfig";

	static ref Timer m_DelayedSpawnFix;

	void BedFrameWork()
	{
	}
	void ~BedFrameWork()
	{	
	}
	
	static void InsertBed(vector bed, PlayerIdentity pd)
	{
		RemoveBedDataByID( pd.GetId() );
		StoredBeds.Insert( pd.GetId(), bed );
		
		string msg = "[BedRespawning] Bed has been placed by ";
		string name = pd.GetName();
		string ext_msg = " @ Location :";
		
		Print( msg + name + ext_msg + bed );
	}

	static void FixSpawningHeight( PlayerBase ply, vector Location )
	{
		ply.SetPosition( Location );
	}

	static vector AttemptBedSpawn( PlayerIdentity identity, vector DefaultPos, bool DestroyOldBed = true  )
	{
		if ( BedFrameWork.StoredBeds.Get( identity.GetId() ) )
		{
			DefaultPos = BedFrameWork.StoredBeds.Get( identity.GetId() );
		}

		return DefaultPos;
	}

	static void BreakOldSpawnBed(PlayerIdentity identity, vector pos)
	{
		int CanBedBreak = BedFrameWork.BedClassNames.Get("DestroyBedAfterSpawn");
		if ( CanBedBreak == 1 )
		{
			if ( BedFrameWork.StoredBeds.Get( identity.GetId() ) )
			{
				ref array<Object> Player_Bed = new array<Object>;
				GetGame().GetObjectsAtPosition( pos, 1.0, Player_Bed, NULL );
				
				for ( int i = 0; i < Player_Bed.Count(); i++ )
				{
					Object bed = Player_Bed.Get( i );

					//Print(bed);
					//Print(bed.GetPosition().ToString(false));
					//Print( BedFrameWork.StoredBeds.Get( identity.GetId() ) );
					if ( BedFrameWork.BedClassNames.Get( bed.GetType() ) && bed.GetPosition().ToString(false) == BedFrameWork.StoredBeds.Get( identity.GetId() ).ToString(false) )
					{
						Print("Found bed, deleting it!");
						GetGame().ObjectDelete(bed);
					}
				}
			}
		}
	}
	
	static void SaveBedData()
	{
		FileHandle file = OpenFile("$profile:"+TextFileName+".txt", FileMode.WRITE);
		if (file != 0)
		{
			int index = 0;
			foreach(string k, vector a: StoredBeds)
			{
				index++;
				string concat = index.ToString() + " " + k + " " + a.ToString(false);
				FPrintln(file, concat );
			}

			CloseFile(file);
		}
	}

	static void LoadBedData()
	{
		FileHandle file_handle = OpenFile("$profile:"+TextFileName+".txt", FileMode.READ);
		if (file_handle != 0)
		{
			string line_content;

			while ( FGets( file_handle,  line_content ) > 0 )
			{
				//Print(line_content);
				array<string> strgs = new array<string>;

				line_content.Split(" ", strgs);

				string stored_id = strgs.Get(1);
				string stored_index = strgs.Get(0);

				float stored_vect_x = strgs.Get(2).ToFloat();
				float stored_vect_y = strgs.Get(3).ToFloat();
				float stored_vect_z = strgs.Get(4).ToFloat();

				//Print(stored_vect_x);
				//Print(stored_vect_y);
				//Print(stored_vect_z);

				vector BedPos = Vector(stored_vect_x, stored_vect_y, stored_vect_z);
				StoredBeds.Insert( stored_id, BedPos );
				
				//Print( stored_index );
				//Print( stored_id );
				//Print( BedPos );
			}

			Print("[BedRespawning] Loaded Data!");

			CloseFile(file_handle);
			m_Loaded = true;
		}

		FileHandle file_handle_config = OpenFile("$profile:"+ConfigFileName+".txt", FileMode.READ);
		if (file_handle_config != 0)
		{
			string line_content_class;

			while ( FGets( file_handle_config,  line_content_class ) > 0 )
			{
				array<string> strgs_config = new array<string>;
				line_content_class.Split(" ", strgs_config);

				string stored_class = strgs_config.Get(0);
				int stored_status = strgs_config.Get(1).ToInt();

				//Print(stored_class);
				//Print(stored_status);
				BedClassNames.Insert(stored_class,stored_status);
			}

			Print("[BedRespawning] Loaded Config!");

			CloseFile(file_handle_config);
		}
		
	}

	static void RemoveBedDataByID( string guid )
	{
		foreach(string k, vector a: BedFrameWork.StoredBeds)
		{
			if ( k == guid )
			{
				BedFrameWork.StoredBeds.Remove(k);
				string msg = "[BedRespawning] Bed deleted ";
				string ext_msg = " @ Location :";
				
				Print( msg + ext_msg + a );
			}
		}
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
				BedFrameWork.SaveBedData();
			}
		}
	}
}