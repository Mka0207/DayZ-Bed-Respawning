//MuchStuffPack Compatibility

modded class ActionFoldItem
{
	override void OnFinishProgressServer( ActionData action_data )
	{	
		super.OnFinishProgressServer(action_data);

		ItemBase bed = action_data.m_Target.GetObject();
		if ( BedFrameWork.IsMuchStuffItem(bed.GetType()) )
		{
			if ( bed.m_OwnerBRGUID != "" )
			{
				Print("[Bed-Respawn 2.0] Ran EEDelete for MSP Sleeping Bag!");
				BedFrameWork.RemoveRespawnData( bed.m_OwnerBRGUID );
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
				vector pos = player_base.GetLocalProjectionPosition();

				m_OwnerBRGUID = pd.GetId();
				m_BRUses = BedFrameWork.m_BedConfig.MaxRespawnsBeforeRemoval;

				BedFrameWork.InsertBed( player_base, m_OwnerBRGUID, pos, 0, m_BRUses );
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
				if ( m_OwnerBRGUID != "" )
				{
					Print("[Bed-Respawn 2.0] Ran EEDelete for MSP Sleeping Bag!");
					BedFrameWork.RemoveRespawnData( m_OwnerBRGUID );
				}
			}
		}
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);

		if ( BedFrameWork.IsMuchStuffItem(this.GetType()) )
		{
			ctx.Write(m_OwnerBRGUID);
			ctx.Write(m_BRUses);
		}
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
        return false;

		if ( BedFrameWork.IsMuchStuffItem(this.GetType()) )
		{
			if ( !ctx.Read(m_OwnerBRGUID) )
				return false;

			if ( !ctx.Read(m_BRUses) )
				return false;
		}

		return true;
	}
}