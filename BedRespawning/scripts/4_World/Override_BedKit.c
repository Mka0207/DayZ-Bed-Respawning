class BedFrameWork
{
	static ref map<string,vector> StoredBeds = new map<string,vector>;

	static bool m_Loaded = false;
	
	static void InsertBed(Object bed, Man player)
	{
		PlayerBase pb = PlayerBase.Cast( player );
		PlayerIdentity pd = pb.GetIdentity();

		if ( StoredBeds.Get( pd.GetId() ) )
		{
			RemoveBedData( pd.GetId() );
			StoredBeds.Insert( pd.GetId(), bed.GetPosition() );
		}
		else
		{
			StoredBeds.Insert( pd.GetId(), bed.GetPosition() );
		}
		
		string msg = "[BedRespawning] Bed has been placed by ";
		string name = pb.GetIdentity().GetName();
		vector bedpos = bed.GetPosition();
		string ext_msg = " @ Location :";
		
		Print( msg + name + ext_msg + bedpos );
		
		FileHandle file = OpenFile("$profile:BedData.txt", FileMode.WRITE);
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

	static void LoadBedData()
	{
		FileHandle file_handle = OpenFile("$profile:BedData.txt", FileMode.READ);
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
	}
}

modded class Base_SingleBed_Kit extends ItemBase
{
	override void OnPlacementComplete( Man player )
	{
		//super.OnPlacementComplete( player );
		
		PlayerBase pb = PlayerBase.Cast( player );
		if ( GetGame().IsServer() )
		{
			PlayerBase player_base = PlayerBase.Cast( player );
			vector position = player_base.GetLocalProjectionPosition();
			vector orientation = player_base.GetLocalProjectionOrientation();
				
			Base_SingleBed_Kit1 = GetGame().CreateObject("Base_SingleBed", pb.GetLocalProjectionPosition(), false );
			Base_SingleBed_Kit1.SetPosition( position );
			Base_SingleBed_Kit1.SetOrientation( orientation );

			BedFrameWork.InsertBed( Base_SingleBed_Kit1, player );
		}	
		
		SetIsDeploySound( true );
	}
}

modded class ActionDismantleBase_SingleBed: ActionContinuousBase
{
	void RemoveFromBedFrameWork(ActionData action_data)
	{
		PlayerIdentity pd = action_data.m_Player.GetIdentity();
		foreach(string k, vector a: BedFrameWork.StoredBeds)
		{
			if ( k == pd.GetId() )
			{
				BedFrameWork.StoredBeds.Remove(k);
				string msg = "[BedRespawning] Bed deconstructed by ";
				string name = action_data.m_Player.GetIdentity().GetName();
				vector bedpos = action_data.m_Target.GetObject().GetPosition();
				string ext_msg = " @ Location :";
				
				Print( msg + name + ext_msg + bedpos );
			}
		}
	}
	override void OnFinishProgressServer( ActionData action_data ) 
	{
		super.OnFinishProgressServer(action_data);
		RemoveFromBedFrameWork(action_data);
	}
}

