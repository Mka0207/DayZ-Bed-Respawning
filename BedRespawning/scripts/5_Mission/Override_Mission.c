modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		
		if (!BedFrameWork.m_Loaded) { BedFrameWork.LoadBedData(); }
	}

	override PlayerBase CreateCharacter(PlayerIdentity identity, vector pos, ParamsReadContext ctx, string characterName)
	{
		super.CreateCharacter(identity,pos,ctx,characterName);

		BedFrameWork.FixSpawningHeight( m_player, BedFrameWork.AttemptBedSpawn(identity,pos,false) );

		return m_player;
	}
}