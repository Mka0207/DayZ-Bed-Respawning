class CfgPatches
{
	class BedRespawningMSP
	{
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Scripts",
			"BedRespawning"
		};
	};
};

class CfgMods
{
	class BedRespawningMSP
	{
		type = "mod";
		dir="BedRespawning-MSP";
		name="BedRespawningMSP";
		credits="Mka0207";
		author="Mka0207";
		dependencies[]=
		{
			"World"
		};
		
	    class defs
	    {
            class worldScriptModule
            {
                value="";
                files[]={"BedRespawning-MSP/scripts/4_World"};
            };
        };
    };
};