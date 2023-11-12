//MuchStuffPack Compatibility

modded class ActionFoldItem
{
	override void OnFinishProgressServer( ActionData action_data )
	{	
		super.OnFinishProgressServer(action_data);

		ItemBase bed = action_data.m_Target.GetObject();
		if ( BedFrameWork.IsMuchStuffItem(bed.GetType()) )
		{
			if ( bed.GetBedOwner() != "" )
			{
				Print("[Bed-Respawn 2.0] Ran EEDelete for MSP Sleeping Bag!");
				BedFrameWork.RemoveRespawnData( bed.GetBedOwner() );
			}
		}
	}
}

modded class ItemBase
{
	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete( player,position,orientation );

		if ( GetGame().IsServer() )
		{
			if ( BedFrameWork.IsMuchStuffItem(this.GetType()) )
			{
				PlayerBase player_base = PlayerBase.Cast( player );
				PlayerIdentity pd = player_base.GetIdentity();
				//vector pos = player_base.GetLocalProjectionPosition();

				this.SetBedOwner(pd.GetId());
				this.SetBedUses(BedFrameWork.m_BedConfig.MaxRespawnsBeforeRemoval);

				BedFrameWork.InsertBed( player_base, this.GetBedOwner(), position, 0, this.GetBedUses() );
			}
		}
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		if ( this.IsHologram() ) return;

		if ( GetGame().IsServer() )
		{
			if ( BedFrameWork.IsMuchStuffItem(this.GetType()) )
			{
				if ( this.GetBedOwner() != "" )
				{
					Print("[Bed-Respawn 2.0] Ran EEDelete for MSP Sleeping Bag!");
					BedFrameWork.RemoveRespawnData( this.GetBedOwner() );
				}
			}
		}
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);

		if ( BedFrameWork.IsMuchStuffItem(this.GetType()) )
		{
			ctx.Write(this.GetBedOwner());
			ctx.Write(this.GetBedUses());
		}
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
        return false;

		if ( BedFrameWork.IsMuchStuffItem(this.GetType()) )
		{
			if ( !ctx.Read(this.GetBedOwner()) )
				return false;

			if ( !ctx.Read(this.GetBedUses()) )
				return false;
		}

		return true;
	}
}