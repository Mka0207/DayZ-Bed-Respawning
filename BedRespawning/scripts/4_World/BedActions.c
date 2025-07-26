/*
* Copyright (C) 2025 fwkzt.com <https://steamcommunity.com/id/mka0207/>
* Licensed under "All Rights Reserved"
*/

// Pack Bag Action
class ActionPackRespawnBag: ActionContinuousBase
{
	void ActionPackRespawnBag()
	{
		m_CallbackClass = ActionBuildOvenCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
	}
	
	override void CreateConditionComponents()  
	{	
		
		m_ConditionTarget = new CCTNonRuined( UAMaxDistances.DEFAULT );
		m_ConditionItem = new CCINotPresent;
	}
		
	override string GetText()
	{
		return "Pack Sleeping Bag";
	}
	
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override bool HasProgress()
	{
		return true;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		Object targetObject = target.GetObject();
		SleepingBagBase_Deployed m_sleepingbag = SleepingBagBase_Deployed.Cast( targetObject );

		if ( m_sleepingbag )
		{
			if ( BedFrameWork.HasCooldown( player, m_sleepingbag.GetPosition() ) )
			{
				BedFrameWork.NotifyCooldownMessage(player);
				return false;
			}
			if ( m_sleepingbag.GetNumberOfItems() == 0 )
	        {
	            return true;
	        }
		}
		return false;
	}
		
	override void OnFinishProgressServer( ActionData action_data )
	{	
		Object targetObject = action_data.m_Target.GetObject();
		SleepingBagBase_Deployed target = SleepingBagBase_Deployed.Cast( targetObject );
		vector position = target.GetPosition();

		if ( GetGame().IsServer() )
		{
			if (target && target.GetBedOwner() != "" )
			{
				BedFrameWork.DeleteBedData( target.GetBedOwner(), target.GetPosition() );
			}
		}

		target.SpawnTheTent(position);
		target.Delete();
	}
}

// Claiming Bag Action
class ActionClaimRespawnBag: ActionInteractBase
{
	void ActionClaimRespawnBag()
	{
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
		m_CommandUIDProne	= DayZPlayerConstants.CMD_ACTIONFB_PICKUP_HANDS;
		m_Text = "Claim Sleeping Bag";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursorNoRuinCheck();
	}

	override bool HasProneException()
	{
		return true;
	}

	override bool HasProgress()
	{
		return false;
	}
	
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		Object targetObject = target.GetObject();
		SleepingBagBase_Deployed m_sleepingbag = SleepingBagBase_Deployed.Cast( targetObject );

		if ( m_sleepingbag )
		{
			if ( m_sleepingbag.GetNumberOfItems() == 0 )
	        {
	            return true;
	        }
		}
		return false;
	}

	override void OnExecuteServer(ActionData action_data)
	{
		super.OnExecuteServer(action_data);

		if (GetGame().IsServer())
		{
			SleepingBagBase_Deployed bag = SleepingBagBase_Deployed.Cast(action_data.m_Target.GetObject());
			PlayerIdentity pd = action_data.m_Player.GetIdentity();

			if (!bag)
			{
				Print("[Bed-Respawn 2.0] ActionClaimRespawnBag bag is null!");
				return;
			}

			// If the bag already has an owner, we can't claim it.
			if (bag.GetBedOwner() != "" && bag.GetBedOwner() != pd.GetId())
			{
				Print("[Bed-Respawn 2.0] Ran ActionClaimRespawnBag for Sleeping Bag, but it already has an owner!");
				action_data.m_Player.RPCSingleParam(ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>("Sleeping Bag has already been claimed!"), true, pd);
				return;
			}

			// If the player already has a bed saved at this position, we can't claim it again.
			if (BedFrameWork.GetUserBedPosition(pd.GetId()).ToString(false) == bag.GetPosition().ToString(false) || bag.GetBedOwner())
			{
				Print("[Bed-Respawn 2.0] Ran ActionClaimRespawnBag for Sleeping Bag, but player already has a bed at this position!");
				action_data.m_Player.RPCSingleParam(ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>("You've already claimed this sleeping bag!"), true, pd);
				return;
			}

			bag.SetBedOwner(pd.GetId());

			int respawnTime = 0;
			if (BedFrameWork.m_BedConfig.BedRespawnTimeMinutes > 0)
			{
				respawnTime = CF_Date.Now(true).GetTimestamp() + (BedFrameWork.m_BedConfig.BedRespawnTimeMinutes * 60);
			}

			BedFrameWork.RegisterSpawnClaim(action_data.m_Player, new BedData(bag.GetBedOwner(), bag.GetPosition(), respawnTime, BedFrameWork.m_BedConfig.MaxRespawnsBeforeRemoval));
		}

		Print("[BedRespawning] Ran OnExecuteServer!");
	}
}

modded class ActionConstructor
{
    override void RegisterActions(TTypenameArray actions)
    {
        super.RegisterActions(actions);
		actions.Insert(ActionPackRespawnBag);
		actions.Insert(ActionClaimRespawnBag);
    }
};