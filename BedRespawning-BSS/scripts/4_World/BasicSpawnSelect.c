modded class PluginBasicSpawnSelectServer
{
	override void SERVER_TELEPORTMETOSPAWN(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		//super.SERVER_TELEPORTMETOSPAWN(type,ctx,sender,target);

		if(type == CallType.Server)
        {
            Param2<ref SpawnLocationObject, PlayerBase> data;
            if ( !ctx.Read( data ) ) return;
                
            SpawnLocationObject m_choosenSpawn = data.param1;
            PlayerBase player = data.param2;
            if(player)
            {
                vector FinalSpawnPos = m_choosenSpawn.GetSpawnRandomSpot();
                local int maxtrycounter = 0;
                array<Object> excluded_objects = new array<Object>;
		        array<Object> nearby_objects = new array<Object>;

                while(GetGame().IsBoxColliding(FinalSpawnPos, player.GetOrientation(), m_FreeSpawnSize, excluded_objects, nearby_objects) && maxtrycounter < 50)
                {
                    FinalSpawnPos = m_choosenSpawn.GetSpawnRandomSpot();
                    maxtrycounter++;
                }

				//Bed-Respawning Compat
				if ( m_choosenSpawn.GetName() == "Bed" )
				{
					BedFrameWork.OnEquipCharacter( player );
				} else {
					player.SetPosition(FinalSpawnPos);
				}
                player.SetOrientation(player.GetOrientation());
            }
            else
            {
                Error("Player was not valid! if you see this befor an crash it was not because this!");
            }
        }
	}
}