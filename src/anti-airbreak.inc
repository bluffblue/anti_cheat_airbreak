#if defined _anti_airbreak_included
    #endinput
#endif
#define _anti_airbreak_included

#if !defined _pawnraknet_included
    #error "Please include PawnRakNet first!"
#endif

#define MAX_AIRBREAK_DISTANCE 15.0
#define MAX_AIRBREAK_HEIGHT 5.0
#define AIRBREAK_CHECK_INTERVAL 500
#define MAX_SPEED_MULTIPLIER 1.5
#define FALL_SPEED_THRESHOLD -0.1
#define MAX_FALL_DISTANCE 100.0

static Float:LastPosition[MAX_PLAYERS][3];
static LastUpdateTick[MAX_PLAYERS];
static PlayerVehicleID[MAX_PLAYERS];
static bool:IsPlayerMonitored[MAX_PLAYERS];
static LastInterior[MAX_PLAYERS];
static LastVirtualWorld[MAX_PLAYERS];
static Float:LastVelocity[MAX_PLAYERS][3];
static bool:IsPlayerFalling[MAX_PLAYERS];

static Float:GetDistanceBetweenPoints3D(Float:x1, Float:y1, Float:z1, Float:x2, Float:y2, Float:z2) {
    return floatsqroot((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2));
}

static bool:IsPlayerInValidState(playerid) {
    new animlib[32], animname[32];
    GetAnimationName(GetPlayerAnimationIndex(playerid), animlib, sizeof(animlib), animname, sizeof(animname));
    
    if(!strcmp(animlib, "PARACHUTE", true)) return false;
    if(!strcmp(animlib, "FALL", true)) return false;
    if(GetPlayerState(playerid) == PLAYER_STATE_SPECTATING) return false;
    if(GetPlayerState(playerid) == PLAYER_STATE_WASTED) return false;
    if(IsPlayerFalling[playerid]) return false;
    return true;
}

static bool:IsPlayerFallingAnimation(playerid) {
    new animlib[32], animname[32];
    GetAnimationName(GetPlayerAnimationIndex(playerid), animlib, sizeof(animlib), animname, sizeof(animname));
    
    if(!strcmp(animlib, "PED", true) && !strcmp(animname, "FALL_FALL", true)) return true;
    if(!strcmp(animlib, "PARACHUTE", true)) return true;
    if(!strcmp(animlib, "FALL", true)) return true;
    return false;
}

stock AirBreak_Init(playerid) {
    if(!IsPlayerConnected(playerid)) return 0;
    
    LastUpdateTick[playerid] = GetTickCount();
    PlayerVehicleID[playerid] = INVALID_VEHICLE_ID;
    IsPlayerMonitored[playerid] = true;
    IsPlayerFalling[playerid] = false;
    GetPlayerPos(playerid, LastPosition[playerid][0], LastPosition[playerid][1], LastPosition[playerid][2]);
    GetPlayerVelocity(playerid, LastVelocity[playerid][0], LastVelocity[playerid][1], LastVelocity[playerid][2]);
    LastInterior[playerid] = GetPlayerInterior(playerid);
    LastVirtualWorld[playerid] = GetPlayerVirtualWorld(playerid);
    return 1;
}

stock AirBreak_Stop(playerid) {
    IsPlayerMonitored[playerid] = false;
    IsPlayerFalling[playerid] = false;
    return 1;
}

stock AirBreak_Check(playerid) {
    if(!IsPlayerConnected(playerid) || !IsPlayerMonitored[playerid]) return 0;
    
    new tick = GetTickCount();
    if(tick - LastUpdateTick[playerid] < AIRBREAK_CHECK_INTERVAL) return 0;
    
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    
    new Float:velX, Float:velY, Float:velZ;
    GetPlayerVelocity(playerid, velX, velY, velZ);
    
    new currentInterior = GetPlayerInterior(playerid);
    new currentVW = GetPlayerVirtualWorld(playerid);
    
    
    if(velZ < FALL_SPEED_THRESHOLD || IsPlayerFallingAnimation(playerid)) {
        IsPlayerFalling[playerid] = true;
        
        
        new Float:fallDistance = GetDistanceBetweenPoints3D(x, y, z, LastPosition[playerid][0], LastPosition[playerid][1], LastPosition[playerid][2]);
        if(fallDistance < MAX_FALL_DISTANCE) {
            LastPosition[playerid][0] = x;
            LastPosition[playerid][1] = y;
            LastPosition[playerid][2] = z;
            LastVelocity[playerid][0] = velX;
            LastVelocity[playerid][1] = velY;
            LastVelocity[playerid][2] = velZ;
            LastUpdateTick[playerid] = tick;
            return 0;
        }
    } else if(IsPlayerFalling[playerid] && velZ > -0.1 && velZ < 0.1) {
        IsPlayerFalling[playerid] = false;
        LastPosition[playerid][0] = x;
        LastPosition[playerid][1] = y;
        LastPosition[playerid][2] = z;
        LastVelocity[playerid][0] = velX;
        LastVelocity[playerid][1] = velY;
        LastVelocity[playerid][2] = velZ;
        LastUpdateTick[playerid] = tick;
        return 0;
    }
    
    if(currentInterior != LastInterior[playerid] || currentVW != LastVirtualWorld[playerid]) {
        LastPosition[playerid][0] = x;
        LastPosition[playerid][1] = y;
        LastPosition[playerid][2] = z;
        LastVelocity[playerid][0] = velX;
        LastVelocity[playerid][1] = velY;
        LastVelocity[playerid][2] = velZ;
        LastInterior[playerid] = currentInterior;
        LastVirtualWorld[playerid] = currentVW;
        LastUpdateTick[playerid] = tick;
        return 0;
    }
    
    if(LastPosition[playerid][0] != 0.0 || LastPosition[playerid][1] != 0.0 || LastPosition[playerid][2] != 0.0) {
        if(IsPlayerInValidState(playerid)) {
            new Float:distance = GetDistanceBetweenPoints3D(x, y, z, LastPosition[playerid][0], LastPosition[playerid][1], LastPosition[playerid][2]);
            new Float:maxAllowedDistance = MAX_AIRBREAK_DISTANCE;
            new Float:heightDiff = z - LastPosition[playerid][2];
            
            new vehicleid = GetPlayerVehicleID(playerid);
            if(vehicleid != INVALID_VEHICLE_ID) {
                maxAllowedDistance *= MAX_SPEED_MULTIPLIER;
                
                new Float:vehVelX, Float:vehVelY, Float:vehVelZ;
                GetVehicleVelocity(vehicleid, vehVelX, vehVelY, vehVelZ);
                new Float:speed = floatsqroot(vehVelX * vehVelX + vehVelY * vehVelY + vehVelZ * vehVelZ) * 200.0;
                
                if(speed > 0.0) {
                    maxAllowedDistance += speed * (AIRBREAK_CHECK_INTERVAL / 1000.0);
                }
                
                if(distance > maxAllowedDistance || heightDiff > MAX_AIRBREAK_HEIGHT) {
                    return 2;
                }
            } else if(distance > maxAllowedDistance || heightDiff > MAX_AIRBREAK_HEIGHT) {
                return 1;
            }
        }
    }
    
    LastPosition[playerid][0] = x;
    LastPosition[playerid][1] = y;
    LastPosition[playerid][2] = z;
    LastVelocity[playerid][0] = velX;
    LastVelocity[playerid][1] = velY;
    LastVelocity[playerid][2] = velZ;
    LastInterior[playerid] = currentInterior;
    LastVirtualWorld[playerid] = currentVW;
    LastUpdateTick[playerid] = tick;
    return 0;
}
