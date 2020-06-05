modded class ItemBase extends InventoryItem
{
	override void OnPlacementComplete( Man player )
	{
		string ent_type = GetType();

		super.OnPlacementComplete(player);

		Print( ent_type );

		if ( BedFrameWork.BedClassNames.Get(ent_type) )
		{
			PlayerBase player_base = PlayerBase.Cast( player );
			vector position = player_base.GetLocalProjectionPosition();

			BedFrameWork.InsertBed( position, player );
		}
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		//Print( this.GetType() );

		if ( BedFrameWork.BedClassNames.Get( this.GetType() ) || this.GetType() == "sleepingbag_red_mung_Deployed" || this.GetType() == "sleepingbag_blue_mung_Deployed" || this.GetType() == "sleepingbag_green_mung_Deployed" || this.GetType() == "sleepingbag_yellow_mung_Deployed" )
		{
			//Print("Deleted Bed!!!!!!!!!!!!");

			foreach(string k, vector a: BedFrameWork.StoredBeds)
			{
				string BedPos = this.GetPosition().ToString(false);
				string OldBedPos = a.ToString(false);
				if ( OldBedPos == BedPos )
				{
					BedFrameWork.StoredBeds.Remove(k);
					string msg = "[BedRespawning] Bed deleted ";
					string ext_msg = " @ Location :";
					
					Print( msg + ext_msg + a );
				}
			}
			BedFrameWork.SaveBedData();
		}
	}
}

class BedFrameWork
{
	static ref map<string,vector> StoredBeds = new map<string,vector>;

	static ref map<string,int> BedClassNames = new map<string,int>;

	static bool m_Loaded = false;

	static string TextFileName = "BedRespawn/BedData";

	static string ConfigFileName = "BedRespawn/BedDataConfig";
	
	static void InsertBed(vector bed, Man player)
	{
		PlayerBase pb = PlayerBase.Cast( player );
		PlayerIdentity pd = pb.GetIdentity();

		if ( StoredBeds.Get( pd.GetId() ) )
		{
			RemoveBedData( pd.GetId() );
			StoredBeds.Insert( pd.GetId(), bed );
		}
		else
		{
			StoredBeds.Insert( pd.GetId(), bed );
		}
		
		string msg = "[BedRespawning] Bed has been placed by ";
		string name = pb.GetIdentity().GetName();
		string ext_msg = " @ Location :";
		
		Print( msg + name + ext_msg + bed );
		
		SaveBedData();
	}

	static vector AttemptBedSpawn( PlayerIdentity identity, vector DefaultPos )
	{
		foreach(string k, vector a: StoredBeds)
		{
			//Print("StoredBeds[" + k + "] = " + a);
			
			if ( k == identity.GetId() )
			{
				DefaultPos = a;
				break;
			}
		}

		return DefaultPos;
	}
	
	static int GetPlayerBedID(PlayerIdentity player)
	{
		int plybed_id = -1;
		
		FileHandle file_handle = OpenFile("$profile:BedData.txt", FileMode.READ);
		if (file_handle != 0)
		{
			string line_content;
			
			while ( FGets( file_handle,  line_content ) > 0 )
			{
				array<string> strgs = new array<string>;
				line_content.Split(" ", strgs);

				string stored_id = strgs.Get(1);
				int stored_index = strgs.Get(0).ToInt();
				
				if ( player.GetId() == stored_id )
				{
					plybed_id = stored_index
					break;
				}
			}

			CloseFile(file_handle);
		}
		
		return plybed_id;
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

	static void RemoveBedData( string guid )
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
		
		SaveBedData();
	}
}

modded class ActionDeployObject: ActionContinuousBase
{
	override void OnFinishProgressServer( ActionData action_data )
	{	
		Print(this);
		Print("finished serer!");
		super.OnFinishProgressServer(action_data);
	}
}
