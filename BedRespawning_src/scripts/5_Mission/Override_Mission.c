modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		
		if (!BedFrameWork.m_Loaded) { BedFrameWork.LoadBedData(); }
	}

	override void EquipCharacter(MenuDefaultCharacterData char_data)
	{
		super.EquipCharacter(char_data);

		PlayerIdentity id = m_player.GetIdentity();
		BedFrameWork.FixSpawningHeight( m_player, BedFrameWork.AttemptBedSpawn( id,m_player.GetPosition() ) );
		BedFrameWork.BreakOldSpawnBed( id, BedFrameWork.AttemptBedSpawn( id,m_player.GetPosition() ) );
	}
}