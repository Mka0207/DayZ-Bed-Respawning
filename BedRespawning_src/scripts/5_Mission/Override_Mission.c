modded class MissionServer
{
	ref BedFrameWork m_BedFrameWork;

	override void OnInit()
	{
		super.OnInit();
		m_BedFrameWork = new BedFrameWork;
	}

	override void EquipCharacter(MenuDefaultCharacterData char_data)
	{
		super.EquipCharacter(char_data);
		if ( m_BedFrameWork.m_BedConfig.DisableAutoSpawnCall == 1 ) return;
		m_BedFrameWork.OnEquipCharacter( m_player );
	}
}