class CfgPatches
{
	class BedRespawning
	{
		units[]=
		{
			"br_sleepingbag",
			"br_sleepingbag_deployed"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Scripts"
		};
	};
};

class CfgMods
{
	class BedRespawning
	{
		type = "mod";
		dir="BedRespawning";
		name="BedRespawning";
		credits="Mka0207";
		author="Mka0207";
		dependencies[]=
		{
			"World",
			"Mission"
		};
		
	    class defs
	    {
            class worldScriptModule
            {
                value="";
                files[]={"BedRespawning/scripts/4_World"};
            };

			class missionScriptModule
			{
				value="";
				files[]={"BedRespawning/scripts/5_Mission"};
			};
        };
    };
};

class CfgVehicles
{
	class Inventory_Base;
	class SleepingBag: Inventory_Base
	{
		scope=0;
		displayName="Packed Sleeping Bag";
		descriptionShort="A packed red sleeping bag.";
		model="\BedRespawning\sleeping_bagnew.p3d";
		weight=2000;
		itemBehaviour=2;
		isMeleeWeapon=0;
		absorbency=0;
		reversed=0;
		rotationFlags=8;
		itemSize[]={3,4};
		lootCategory="Materials";
		hiddenSelections[]=
		{
			"Texture"
		};
		hiddenSelectionsTextures[]=
		{
			"BedRespawning\data\sleepingbagnew_co.paa"
		};
	};
	class SleepingBagBase_Deployed: Inventory_Base
	{
		scope=0;
		displayName="Sleeping Bag";
		descriptionShort="Made out of comfy material that serves as a spawn point.";
		model="\BedRespawning\sleepingbag_placed.p3d";
		weight=1800;
		physLayer="item_large";
		itemsCargoSize[]={5,5};
		itemSize[]={8,4};
		carveNavmesh=1;
		rotationFlags=0;
		itemBehaviour=2;
		storageCategory=1;
		hiddenSelections[]=
		{
			"placing"
		};
		hiddenSelectionsTextures[]=
		{
			"BedRespawning\data\sleepingbagplaced_co.paa"
		};
		hologramMaterial="sleepingbag_placed";
		hologramMaterialPath="BedRespawning\data";
	};
	class br_sleepingbag: SleepingBag
	{
		scope=2;
		displayName="Sleeping Bag";
		hiddenSelectionsTextures[]=
		{
			"BedRespawning\data\sleepingbagnew_co.paa"
		};
	};
	class br_sleepingbag_deployed: SleepingBagBase_Deployed
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"BedRespawning\data\sleepingbagplaced_co.paa"
		};
	};
};