# DayZ-Bed-Respawning
A mod to allow respawning on beds.

Workshop Link : https://steamcommunity.com/sharedfiles/filedetails/?id=2111275052

## Instructions :
Open the init.c file and find the line of old code (CTRL+F) and replace it with the new code :

## Old code -
`playerEnt = GetGame().CreatePlayer(identity, characterName, pos, 0, "NONE");`

## New code -
`playerEnt = GetGame().CreatePlayer(identity, characterName, BedFrameWork.AttemptBedSpawn(identity,pos,false), 0, "NONE");`

## It should look like this AFTER :
![alt text](https://i.imgur.com/tGGaAxt.png "Example")

Now if all things were done correctly, simply mount the mod, copy the key into the key folder and restart the server then you should be good to go!

### How to use :
After placing a "Single Bed Kit" from Base_Storage or any sleeping bag from MunghardsItempack, the position will be saved so you spawn there after death.
Placing a bed kit after you already have one will just overwrite the previous bed respawn location.
If the bed is deconstructed then you cannot spawn there. Beds are loaded/saved from a text file.

Positions / Guids / Config of placed beds are stored in the servers profile folder under the directory name "BedRespawn".
BedDataConfig.txt can be edited to turn certain items on or off as respawn capable items, meaning you don't have to allow "Single Bed Kit" as a spawning location if you don't want.
Example : 0 = off and 1 = on.

### Disclaimer : Any repacking / reuploading / modifying of this github code is prohibited.

### Please contact me for permission / feedback!
