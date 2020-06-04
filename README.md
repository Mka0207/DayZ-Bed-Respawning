# DayZ-Bed-Respawning
A mod to allow respawning on beds.

Workshop Link : https://steamcommunity.com/sharedfiles/filedetails/?id=2111275052

## Instructions :
Open the init.c file and find the old line of code (CTRL+F) and replace it with the new code :

## old code -
`playerEnt = GetGame().CreatePlayer(identity, characterName, pos, 0, "NONE");`

## new code -
`playerEnt = GetGame().CreatePlayer(identity, characterName, BedFrameWork.AttemptBedSpawn(identity,pos), 0, "NONE");`

## It should look like this AFTER :
![alt text](https://i.imgur.com/HfAkfcH.png "Example")

Now if all things were done correctly, simply mount the mod, copy the key into the key folder and restart the server then you should be good to go!

### How to use :
After placing a "Single Bed Kit", the position will be saved so you respawn there after death.
Placing a bed kit after you already have one will just overwrite the previous bed respawn location.
If the bed is deconstructed then you cannot respawn there. Beds are loaded/saved from a text file.

Positions / Guids of placed beds are stored in the servers profile folder under the filename "BedData.txt".

## Disclaimer : Any repacking / reuploading / modifying of this github code is prohibited.

Please contact me for permission / feedback!
