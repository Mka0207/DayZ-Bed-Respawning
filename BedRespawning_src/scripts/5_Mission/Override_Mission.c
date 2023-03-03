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
		BedFrameWork.OnEquipCharacter( m_player );
	}
}