class CfgPatches
{
	class BedRespawning
	{
		units[]=
		{
			"sleepingbag_tan",
			"sleepingbag_tan_Deployed"
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
		displayName="Sleepingbag packed";
		descriptionShort="A tightly packed sleepingbag.";
		model="\BedRespawning\sleeping_bagnew.p3d";
		weight=2000;
		itemBehaviour=2;
		isMeleeWeapon=0;
		absorbency=0;
		reversed=0;
		rotationFlags=8;
		itemSize[]={3,4};
		lootCategory="Materials";
		hiddenSelectionsTextures[]=
		{
			"BedRespawning\data\sleepingbagnew_co.paa"
		};
	};
	class SleepingBagBase_colorbase_Deployed: Inventory_Base
	{
		scope=0;
		displayName="Sleepingbag";
		descriptionShort="Sleepingbag";
		model="\BedRespawning\sleepingbag_placed.p3d";
		weight=2000;
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
	class sleepingbag_tan: SleepingBag
	{
		scope=2;
		displayName="Sleepingbag Tan";
		hiddenSelectionsTextures[]=
		{
			"BedRespawning\data\sleepingbagnew_co.paa"
		};
	};
	class sleepingbag_tan_Deployed: SleepingBagBase_colorbase_Deployed
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"BedRespawning\data\sleepingbagplaced_co.paa"
		};
	};
};