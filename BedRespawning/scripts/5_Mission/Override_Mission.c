modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		
		if (!BedFrameWork.m_Loaded) { BedFrameWork.LoadBedData(); }
	}
}