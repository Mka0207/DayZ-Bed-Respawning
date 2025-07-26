/*
* Copyright (C) 2025 fwkzt.com <https://steamcommunity.com/id/mka0207/>
* Licensed under "All Rights Reserved"
*/

//Base Building Plus support
#ifdef BBPItemPack
modded class ActionPlaceObject
{		
	override void OnFinishProgressServer( ActionData action_data )
	{	
		super.OnFinishProgressServer(action_data);

		if ( m_CommandUID == DayZPlayerConstants.CMD_ACTIONFB_DEPLOY_2HD )
		{
			ItemBase bed = action_data.m_MainItem;
			if ( BedFrameWork.m_BedConfig.EnableBBP && bed.GetType() == "BBP_BedKit" )
			{
				//Print("ActionPlaceObject " + bed);
				Print("[ActionPlaceObject] OnFinishProgressServer - BBP FIX");

				PlayerIdentity pd = action_data.m_Player.GetIdentity();
				vector pos = action_data.m_Player.GetPosition();

				bed.SetBedOwner(pd.GetId());

				BedFrameWork.RegisterSpawnClaim( pd.GetId(), new BedData( pd.GetId(), pos, BedFrameWork.m_BedConfig.BedRespawnTimeMinutes, BedFrameWork.m_BedConfig.MaxRespawnsBeforeRemoval ) );
			}
		}
	}
}
modded class BBP_Bed
{
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
modded class BedFrameWork
{
	override static bool ClassDeleteFilter(string type)
	{
		//BedFrameWork.ClassDeleteFilter(type);
		
		return type == "OP_SleepingBagCamo" || type == "OP_SleepingBagBlue" || type == "OP_SleepingBagGrey";
	}

	static bool IsOPBaseItem(string c)
	{
		return c == "OP_SleepingBagCamo" || c == "OP_SleepingBagBlue" || c == "OP_SleepingBagGrey";
	}
}
modded class TentBase
{
	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete( player,position,orientation );
		if ( BedFrameWork.m_BedConfig.EnableOpBaseItemSupport == 0 ) return;

		if ( GetGame().IsServer() && BedFrameWork.IsOPBaseItem(this.GetType()) )
		{
			PlayerBase player_base = PlayerBase.Cast( player );
			PlayerIdentity pd = player_base.GetIdentity();
			vector pos = player_base.GetLocalProjectionPosition();

			this.SetBedOwner(pd.GetId());

			BedFrameWork.RegisterSpawnClaim( player_base, new BedData( pd.GetId(), pos, BedFrameWork.m_BedConfig.BedRespawnTimeMinutes, BedFrameWork.m_BedConfig.MaxRespawnsBeforeRemoval ) );
		}
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);

		if ( BedFrameWork.m_BedConfig.EnableOpBaseItemSupport == 0 ) return;
		if ( BedFrameWork.IsOPBaseItem(this.GetType()) )
		{
			ctx.Write(this.GetBedOwner());
		}
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
        return false;

		if ( BedFrameWork.m_BedConfig.EnableOpBaseItemSupport == 0 ) return false;

		if ( BedFrameWork.IsOPBaseItem(this.GetType()) )
		{
			if ( !ctx.Read(this.GetBedOwner()) )
				return false;
		}

		return true;
	}

	override void Pack( bool update_navmesh, bool init = false )
	{		
		if ( GetGame().IsServer() && BedFrameWork.m_BedConfig.EnableOpBaseItemSupport == 1 && BedFrameWork.IsOPBaseItem(this.GetType()) )
		{
			if ( GetState() == PITCHED && this.GetBedOwner() != "" )
			{
				Print("[Bed-Respawn 2.0] Ran Pack for Sleeping Bag!");
				BedFrameWork.DeleteBedData( this.GetBedOwner(), this.GetPosition() );
			}
		}

		super.Pack(update_navmesh,init);
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		if ( GetGame().IsServer() && BedFrameWork.IsOPBaseItem(this.GetType()) )
		{
			if ( BedFrameWork.m_BedConfig.EnableOpBaseItemSupport == 0 ) return;
			if ( GetState() == PITCHED && this.GetBedOwner() != "" )
			{
				Print("[Bed-Respawn 2.0] Ran EEDelete for Sleeping Bag!");
				BedFrameWork.DeleteBedData( this.GetBedOwner(), this.GetPosition() );
			}
		}
	}
}
#endif

//Munghardsitems support
#ifdef Munghardsitems
override static bool ClassDeleteFilter(string c)
{
	super.ClassDeleteFilter(c);

	return c == "sleepingbag_red_mung" || c == "sleepingbag_blue_mung" || c == "sleepingbag_green_mung" || c == "sleepingbag_yellow_mung";
}
static bool IsDeployedMungItem(string c)
{
	return c == "sleepingbag_red_mung_Deployed" || c == "sleepingbag_blue_mung_Deployed" || c == "sleepingbag_green_mung_Deployed" || c == "sleepingbag_yellow_mung_Deployed";
}
modded class ItemBase
{
	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete( player,position,orientation );

		if ( GetGame().IsServer() )
		{
			if ( BedFrameWork.IsPackedMungItem(this.GetType()) )
			{
				PlayerBase player_base = PlayerBase.Cast( player );
				if ( !player_base ) return;

				BedFrameWork.RegisterSpawnClaim( player_base, new BedData( pd.GetId(), position, BedFrameWork.m_BedConfig.BedRespawnTimeMinutes, BedFrameWork.m_BedConfig.MaxRespawnsBeforeRemoval ) );
			}
		}
	}
	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		if ( this.IsHologram() ) return;

		//Print(this.GetType());

		if ( GetGame().IsServer() )
		{
			if ( BedFrameWork.m_BedConfig.EnableMung && IsDeployedMungItem(this.GetType()) )
			{
				if ( GetBedOwner() != "" )
				{
					Print("[Bed-Respawn 2.0] Ran EEDelete for BBP Bed!");
					BedFrameWork.DeleteBedData( GetBedOwner(), this.GetPosition() );
				}
			}
		}
	}
}
#endif

//MuchStuffPack support
#ifdef MuchStuffPack
override static bool ClassDeleteFilter(string c)
{
	super.ClassDeleteFilter(c);
	return c == "Msp_SleepingBag_Green" || c == "Msp_SleepingBag_Blue" || c == "Msp_SleepingBag_LimeGreen" || c == "Msp_SleepingBag_Orange" || c == "Msp_SleepingBag_Purple" || c == "Msp_SleepingBag_Red";
}
static bool IsMuchStuffItem(string c)
{
	return c == "Msp_SleepingBag_Green" || c == "Msp_SleepingBag_Blue" || c == "Msp_SleepingBag_LimeGreen" || c == "Msp_SleepingBag_Orange" || c == "Msp_SleepingBag_Purple" || c == "Msp_SleepingBag_Red";
}
modded class ActionFoldItem
{
	override void OnFinishProgressServer( ActionData action_data )
	{	
		super.OnFinishProgressServer(action_data);

		ItemBase bed = action_data.m_Target.GetObject();
		if ( IsMuchStuffItem(bed.GetType()) )
		{
			if ( bed.GetBedOwner() != "" )
			{
				Print("[Bed-Respawn 2.0] Ran EEDelete for MSP Sleeping Bag!");
				BedFrameWork.DeleteBedData( bed.GetBedOwner(), this.GetPosition() );
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
			if ( IsMuchStuffItem(this.GetType()) )
			{
				PlayerBase player_base = PlayerBase.Cast( player );
				PlayerIdentity pd = player_base.GetIdentity();

				this.SetBedOwner(pd.GetId());

				BedFrameWork.RegisterSpawnClaim( player_base, new BedData( pd.GetId(), position, BedFrameWork.m_BedConfig.BedRespawnTimeMinutes, BedFrameWork.m_BedConfig.MaxRespawnsBeforeRemoval ) );
			}
		}
	}
}
#endif