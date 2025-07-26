/*
* Copyright (C) 2025 fwkzt.com <https://steamcommunity.com/id/mka0207/>
* Licensed under "All Rights Reserved"
*/

modded class ItemBase
{
	string m_OwnerBRGUID;

	string GetBedOwner()
	{
		return m_OwnerBRGUID;
	}

	void SetBedOwner(string guid)
	{
		m_OwnerBRGUID = guid;
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);

		ctx.Write(m_OwnerBRGUID);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;
		
		if (!ctx.Read(m_OwnerBRGUID))
			return false;
		
		return true;
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		if ( GetGame().IsServer() )
		{
			if ( this.IsHologram() ) return;

			if ( GetBedOwner() != "" )
			{
				Print("[Bed-Respawn 2.0] Ran EEDelete for " + this.GetType() + "!");
				BedFrameWork.DeleteBedData( GetBedOwner(), this.GetPosition() );
			}
		}
	}
}