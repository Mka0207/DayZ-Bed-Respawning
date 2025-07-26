/*
* Copyright (C) 2025 fwkzt.com <https://steamcommunity.com/id/mka0207/>
* Licensed under "All Rights Reserved"
*/

modded class ModItemRegisterCallbacks
{
    override void RegisterTwoHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
    {
        pType.AddItemInHandsProfileIK("SleepingBag", "dz/anims/workspaces/player/player_main/weapons/player_main_2h_extinguisher.asi", pBehavior, "dz/anims/anm/player/ik/two_handed/WaterproofBag.anm");
        super.RegisterTwoHanded(pType, pBehavior);
    }
}

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
		//RegisterNetSyncVariableBool("m_IsSoundSynchRemote");
	}
	
	override void EEInit()
	{
		super.EEInit();
	}
	
	override void OnItemLocationChanged( EntityAI old_owner, EntityAI new_owner ) 
	{
		super.OnItemLocationChanged( old_owner, new_owner );
	}		

	//Disabled because it doesnt stop playing the deploy loop sound when the bag is packed.
	override void OnVariablesSynchronized()
	{
		//super.OnVariablesSynchronized();
		
		/*if ( IsDeploySound() )
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
		}*/
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
		
		//SetIsDeploySound( true );
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

class SleepingBagBase_Deployed : ItemBase
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
		AddAction(ActionClaimRespawnBag);
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