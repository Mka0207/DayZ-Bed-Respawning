/*
* Copyright (C) 2025 fwkzt.com <https://steamcommunity.com/id/mka0207/>
* Licensed under "All Rights Reserved"
*/

modded class ItemBase
{
	string m_OwnerBRGUID;
	int m_BRUses = 0;

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
		ctx.Write(m_BRUses);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;
		
		if (!ctx.Read(m_OwnerBRGUID))
			return false;

		if (!ctx.Read(m_BRUses))
			return false;
		
		return true;
	}

	/*override void CF_OnStoreSave(CF_ModStorageMap storage)
	{
		super.CF_OnStoreSave(storage);

		auto ctx = storage[BedRespawning];
		if (!ctx) return;
		
		ctx.Write(m_OwnerBRGUID);
	}

	override bool CF_OnStoreLoad(CF_ModStorageMap storage)
	{
		if (!super.CF_OnStoreLoad(storage)) return false;

		auto ctx = storage[BedRespawning];
		if (!ctx) return true;

		if (ctx.GetVersion() >= 1)
		{
			if (!ctx.Read(m_OwnerBRGUID)) return false;
		}

		Print( "[Bed-Respawn 2.0] Loaded owner GUID: " + m_OwnerBRGUID + " for " + this.GetType() );

		return true;
	}*/

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