modded class MissionServer
{
	ref BedFrameWork m_BedFrameWork;

	override void OnInit()
	{
		super.OnInit();
		
		/*if (!BedFrameWork.m_Loaded) { 
			BedFrameWork.LoadBedData(); 
		}*/

		m_BedFrameWork = new BedFrameWork; 
	}

	override void EquipCharacter(MenuDefaultCharacterData char_data)
	{
		super.EquipCharacter(char_data);

		//Print(m_BedFrameWork.m_Folder);

		PlayerIdentity id = m_player.GetIdentity();
		BedFrameWork.FixSpawningHeight( m_player, BedFrameWork.AttemptBedSpawn( id,m_player.GetPosition() ) );
		BedFrameWork.BreakOldSpawnBed( id, BedFrameWork.AttemptBedSpawn( id,m_player.GetPosition() ) );
	}
}