modded class ModItemRegisterCallbacks
{
    override void RegisterTwoHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
    {
        pType.AddItemInHandsProfileIK("SleepingBag", "dz/anims/workspaces/player/player_main/weapons/player_main_2h_extinguisher.asi", pBehavior, "dz/anims/anm/player/ik/two_handed/WaterproofBag.anm");
        super.RegisterTwoHanded(pType, pBehavior);
    }
}

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

		target.SpawnTheTent(position);
		target.Delete();
	}
}

modded class ActionConstructor
{
    override void RegisterActions(TTypenameArray actions)
    {
        super.RegisterActions(actions);
		actions.Insert(ActionPackRespawnBag);
    }
};

class SleepingBagBase extends ItemBase
{
	ref protected EffectSound 	m_DeployLoopSound;
	protected Object			SleepingBag_Deployed;
	protected Object			SB_ClutterCutter;


	override bool IsBasebuildingKit()
	{
		return true;
	}
	string BagDeployClass()
	{
		return "br_sleepingbag";
	}
	
	void SleepingBagBase()
	{
		RegisterNetSyncVariableBool("m_IsSoundSynchRemote");
	}
	
	override void EEInit()
	{
		super.EEInit();
	}
	

	override void OnItemLocationChanged( EntityAI old_owner, EntityAI new_owner ) 
	{
		super.OnItemLocationChanged( old_owner, new_owner );
	}		

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		if ( IsDeploySound() )
		{
			PlayDeploySound();
		}
				
		if ( CanPlayDeployLoopSound() )
		{
			PlayDeployLoopSound();
		}
					
		if ( m_DeployLoopSound && !CanPlayDeployLoopSound() )
		{
			StopDeployLoopSound();
		}
	}	
		
	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0"  )
	{
		super.OnPlacementComplete( player, position, orientation );
		
		PlayerBase pb = PlayerBase.Cast( player );
		if ( GetGame().IsServer() )
		{
			PlayerBase player_base = PlayerBase.Cast( player );
				
			SleepingBag_Deployed = GetGame().CreateObjectEx(this.BagDeployClass(), GetPosition(), ECE_PLACE_ON_SURFACE );				
			SleepingBag_Deployed.SetPosition( position);
			SleepingBag_Deployed.SetOrientation( orientation );

			SB_ClutterCutter = GetGame().CreateObject( "ClutterCutterFireplace", pb.GetLocalProjectionPosition(), false );
			SB_ClutterCutter.SetPosition( position);
			SB_ClutterCutter.SetOrientation( orientation );

			this.Delete();			
		}	
		
		SetIsDeploySound( true );
	}
	
	override bool IsDeployable()
	{
		return true;
	}	
	
	override string GetDeploySoundset()
	{
		return "placeMediumTent_SoundSet";
	}
	
	override string GetLoopDeploySoundset()
	{
		return "mediumtent_deploy_SoundSet";
	}
	
	void PlayDeployLoopSound()
	{		
		if ( GetGame().IsMultiplayer() && GetGame().IsClient() || !GetGame().IsMultiplayer() )
		{		
			m_DeployLoopSound = SEffectManager.PlaySound( GetLoopDeploySoundset(), GetPosition());
		}
	}
	
	void StopDeployLoopSound()
	{
		if ( GetGame().IsMultiplayer() && GetGame().IsClient() || !GetGame().IsMultiplayer() )
		{	
			m_DeployLoopSound.SoundStop();
			delete m_DeployLoopSound;
		}
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionTogglePlaceObject);
		AddAction(ActionDeployObject);
	}
};

class br_sleepingbag extends SleepingBagBase {
	override string BagDeployClass()
	{
		return "br_sleepingbag_deployed";
	}
};

class SleepingBagBase_Deployed extends ItemBase
{
	protected Object SB_ClutterCutter;

	string BagDeployClass()
	{
		return "br_sleepingbag";
	}

	void SpawnTheTent(vector position)
	{
		GetGame().CreateObject(this.BagDeployClass(), position, false );
	}

	override bool CanPutInCargo( EntityAI parent )
    {
        return false;
    }

    override bool CanPutIntoHands(EntityAI parent)
    {
        return false;
    }

	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionPackRespawnBag);
	}

};

modded class Hologram
{
	override string ProjectionBasedOnParent()
	{
		ItemBase item_in_hands = ItemBase.Cast( m_Player.GetHumanInventory().GetEntityInHands() );

		if ( item_in_hands.IsInherited( br_sleepingbag ))
		{
			return "br_sleepingbag_deployed";
		}

		return super.ProjectionBasedOnParent();
	}
}