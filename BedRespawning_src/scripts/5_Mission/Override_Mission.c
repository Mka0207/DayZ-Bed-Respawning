modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		
		if (!BedFrameWork.m_Loaded) { BedFrameWork.LoadBedData(); }
	}

	override void EquipCharacter()
	{
		super.EquipCharacter();

		PlayerIdentity id = m_player.GetIdentity();
		BedFrameWork.FixSpawningHeight( m_player, BedFrameWork.AttemptBedSpawn( id,m_player.GetPosition() ) );
		if ( BedFrameWork.BedClassNames.Get("DestroyBedAfterSpawn") == 1 )
		{
			BedFrameWork.BreakOldSpawnBed( id, BedFrameWork.AttemptBedSpawn( id,m_player.GetPosition() ) );
		}
	}
}