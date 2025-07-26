/*
* Copyright (C) 2025 fwkzt.com <https://steamcommunity.com/id/mka0207/>
* Licensed under "All Rights Reserved"
*/

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
		m_BedFrameWork.TeleportPlayer( m_player );
	}
}

#ifdef DZ_Expansion_SpawnSelection
modded class ExpansionRespawnHandlerModule
{
	ref BedFrameWork m_BedFrameWork;

	override void OnInit()
	{
		super.OnInit();
		m_BedFrameWork = new BedFrameWork;
	}

	override void EndSpawnSelection(PlayerBase player)
	{
		super.EndSpawnSelection( player );
		m_BedFrameWork.TeleportPlayer( player );
	}
}
#endif