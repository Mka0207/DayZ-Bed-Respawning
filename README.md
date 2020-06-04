# DayZ-Bed-Respawning
A mod to allow respawning on beds.

Workshop Link : https://steamcommunity.com/sharedfiles/filedetails/?id=2111275052

## Instructions :
Open the init.c file and find the old line of code (CTRL+F) and replace it with the new one :
# old code -
`playerEnt = GetGame().CreatePlayer(identity, characterName, pos, 0, "NONE");`

# new code -
`playerEnt = GetGame().CreatePlayer(identity, characterName, BedFrameWork.AttemptBedSpawn(identity,pos), 0, "NONE");`

## Disclaimer : Any repacking / reuploading / modifying of this github code is prohibited.

Please contact me for permission / feedback!
