modded class MissionServer
{
	ref BedFrameWork m_BedFrameWork;

	override void OnInit()
	{
		super.OnInit();
		m_BedFrameWork = new BedFrameWork;
	}

	override void StartingEquipSetup(PlayerBase player, bool clothesChosen)
	{
		super.StartingEquipSetup(player,clothesChosen);
		if ( m_BedFrameWork.m_BedConfig.DisableAutoSpawnCall == 1 ) return;
		BedFrameWork.OnEquipCharacter( player );
	}
}