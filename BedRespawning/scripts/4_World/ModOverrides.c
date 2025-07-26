/*
* Copyright (C) 2025 fwkzt.com <https://steamcommunity.com/id/mka0207/>
* Licensed under "All Rights Reserved"
*/

//Base Building Plus support
#ifdef BBPItemPack
modded class BBP_Bed
{
	override void SetActions()
	{
		super.SetActions();
		if ( BedFrameWork.m_BedConfig.EnableBBP != 1 ) return;
		AddAction(ActionClaimRespawnBag);
	}
	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		if ( GetGame().IsServer() )
		{
			if ( BedFrameWork.m_BedConfig.EnableBBP == 0 ) return;
			if ( this.GetBedOwner() != "" )
			{
				Print("[Bed-Respawn 2.0] Ran EEDelete for BBP_Bed!");
				BedFrameWork.DeleteBedData( this.GetBedOwner(), this.GetPosition() );
			}
		}
	}
}
#endif

//OP_BaseItems support
#ifdef OP_BaseItems
modded class OP_SleepingBagColorbase
{
	override void SetActions()
	{
		super.SetActions();
		if ( BedFrameWork.m_BedConfig.EnableOpBaseItemSupport != 1 ) return;
		AddAction(ActionClaimRespawnBag);
	}
}
modded class TentBase
{
	override void Pack( bool update_navmesh, bool init = false )
	{		
		if ( GetGame().IsServer() )
		{
			if ( GetState() == PITCHED && this.GetBedOwner() != "" )
			{
				BedFrameWork.DeleteBedData( this.GetBedOwner(), this.GetPosition() );
			}
		}

		super.Pack(update_navmesh,init);
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		if ( GetGame().IsServer() )
		{
			if ( GetState() == PITCHED && this.GetBedOwner() != "" )
			{
				BedFrameWork.DeleteBedData( this.GetBedOwner(), this.GetPosition() );
			}
		}
	}
}
#endif

//Munghardsitems support
#ifdef Munghardsitems
modded class sleepingbag_colorbase_mung_Deployed
{
	override void SetActions()
	{
		super.SetActions();
		if ( BedFrameWork.m_BedConfig.EnableMung != 1 ) return;
		AddAction(ActionClaimRespawnBag);
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		if ( this.IsHologram() ) return;

		if ( GetGame().IsServer() )
		{
			if ( GetBedOwner() != "" )
			{
				BedFrameWork.DeleteBedData( GetBedOwner(), this.GetPosition() );
			}
		}
	}
}
#endif

#ifdef ZenSleep
modded class ZenSleepingBagDeployed_Base
{
	override void SetActions()
	{
		super.SetActions();
		if ( BedFrameWork.m_BedConfig.EnableZen != 1 ) return;
		AddAction(ActionClaimRespawnBag);
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		if ( this.IsHologram() ) return;

		if ( GetGame().IsServer() )
		{
			if ( GetBedOwner() != "" )
			{
				//Print("[Bed-Respawn 2.0] Ran EEDelete for " + this.GetType() + "!");
				BedFrameWork.DeleteBedData( GetBedOwner(), this.GetPosition() );
			}
		}
	}
};
#endif

#ifdef bl_pallet_bed
modded class bl_bed
{
	override void SetActions()
	{
		super.SetActions();
		if ( BedFrameWork.m_BedConfig.EnableBoomlay != 1 ) return;
		AddAction(ActionClaimRespawnBag);
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		if ( GetGame().IsServer() )
		{
			if ( this.GetBedOwner() != "" )
			{
				Print("[Bed-Respawn 2.0] Ran EEDelete for " + this.GetType() + "!");
				BedFrameWork.DeleteBedData( this.GetBedOwner(), this.GetPosition() );
			}
		}
	}
};
#endif

#ifdef rag_baseitems
modded class rag_baseitems_sleepingbag_base
{
	override void SetActions()
	{
		super.SetActions();
		if ( BedFrameWork.m_BedConfig.EnableRagBase != 1 ) return;
		AddAction(ActionClaimRespawnBag);
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		if ( this.IsHologram() ) return;

		if ( GetGame().IsServer() )
		{
			if ( GetBedOwner() != "" )
			{
				Print("[Bed-Respawn 2.0] Ran EEDelete for " + this.GetType() + "!");
				BedFrameWork.DeleteBedData( GetBedOwner(), this.GetPosition() );
			}
		}
	}
};
#endif