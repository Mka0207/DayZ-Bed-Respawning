class CfgPatches
{
	class BedRespawningBSS
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
	class BedRespawningBSS
	{
		type = "mod";
		dir="BedRespawning-BSS";
		name="BedRespawningBSS";
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
                files[]={"BedRespawning-BSS/scripts/4_World"};
            };
        };
    };
};