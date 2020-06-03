class CfgPatches
{
	class BedRespawning
	{
		requiredAddons[]=
		{
			//""
		};
	};
};

class CfgMods
{
	class BedRespawning
	{
	    type = "mod";
		
	    class defs
	    {
			
			/*class worldScriptModule		
            {
                value = "";
                files[] = {"BedRespawning/WorldScripts"};
            };*/
			
			 /*
            script module config classes are optional, define only what you want to mod
            class gameLibScriptModule
            {
                value="";
                files[]={"BedRespawning/scripts/2_GameLib"};
            };*/
            
			/* class gameScriptModule
            {
                value="CreateGameMod"; // when value is filled, default script module entry function name is overwritten by it
                files[]={"BedRespawning/scripts/3_Game"};
            };*/
             
            class worldScriptModule
            {
                value="";
                files[]={"BedRespawning/scripts/4_World"};
            };
             
			/*
            class missionScriptModule
            {
                value="";
                files[]={"BedRespawning/scripts/5_Mission"};
            };*/
        };
    };
};