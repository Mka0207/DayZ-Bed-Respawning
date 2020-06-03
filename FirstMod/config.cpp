class CfgPatches
{
	class FirstMod
	{
		requiredAddons[]=
		{
			// ""
		};
	};
};

class CfgMods
{
	class FirstMod
	{
	    type = "mod";
		
	    class defs
	    {
			
			/*class worldScriptModule		
            {
                value = "";
                files[] = {"FirstMod/WorldScripts"};
            };*/
			
			 /*
            script module config classes are optional, define only what you want to mod
            class gameLibScriptModule
            {
                value="";
                files[]={"FirstMod/scripts/2_GameLib"};
            };*/
            
			/* class gameScriptModule
            {
                value="CreateGameMod"; // when value is filled, default script module entry function name is overwritten by it
                files[]={"FirstMod/scripts/3_Game"};
            };*/
             
            class worldScriptModule
            {
                value="";
                files[]={"FirstMod/scripts/4_World"};
            };
             
			/*
            class missionScriptModule
            {
                value="";
                files[]={"FirstMod/scripts/5_Mission"};
            };*/
        };
    };
};