#include <a_samp>
#include <PawnRakNet>
#include "src/anti-airbreak"

#define DIALOG_NONE -1

new bool:PlayerKicked[MAX_PLAYERS];

public OnFilterScriptInit()
{
    print("Anti-Airbreak System Loaded!");
    return 1;
}

public OnFilterScriptExit()
{
    return 1;
}

public OnPlayerConnect(playerid)
{
    PlayerKicked[playerid] = false;
    AirBreak_Init(playerid);
    SendClientMessage(playerid, -1, "Anti-Airbreak monitoring started.");
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    PlayerKicked[playerid] = false;
    AirBreak_Stop(playerid);
    return 1;
}

public OnPlayerSpawn(playerid)
{
    if(!PlayerKicked[playerid])
    {
        AirBreak_Init(playerid);
    }
    return 1;
}

public OnPlayerDeath(playerid, killerid, reason)
{
    if(!PlayerKicked[playerid])
    {
        AirBreak_Stop(playerid);
    }
    return 1;
}

public OnPlayerUpdate(playerid)
{
    if(PlayerKicked[playerid]) return 0;

    new detection = AirBreak_Check(playerid);
    
    if(detection > 0)
    {
        new name[MAX_PLAYER_NAME];
        GetPlayerName(playerid, name, sizeof(name));
        
        new str[144];
        
        switch(detection)
        {
            case 1: // On foot airbreak
            {
                format(str, sizeof(str), "AdmCmd: %s[%d] has been automatically kicked for airbreak (on foot)", name, playerid);
            }
            case 2: // Vehicle airbreak
            {
                format(str, sizeof(str), "AdmCmd: %s[%d] has been automatically kicked for airbreak (in vehicle)", name, playerid);
            }
        }
        
        SendClientMessageToAll(0xFF0000FF, str);
        
        PlayerKicked[playerid] = true;
        AirBreak_Stop(playerid);
        
        // Delay kick untuk memastikan pesan terkirim
        SetTimerEx("DelayedKick", 500, false, "i", playerid);
    }
    return 1;
}

forward DelayedKick(playerid);
public DelayedKick(playerid)
{
    if(IsPlayerConnected(playerid) && PlayerKicked[playerid])
    {
        Kick(playerid);
    }
    return 1;
}
