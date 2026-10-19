// Поворотники и аварийка: Q и E включают левый и правый, H (горн) аварийку.
// Лампы крепятся к углам машины по размерам модели и мигают по таймеру.

#define SIGNAL_OFF              0
#define SIGNAL_LEFT             1
#define SIGNAL_RIGHT            2
#define SIGNAL_HAZARD           3
#define SIGNAL_LAMP_MODEL       19294   // оранжевый источник света
#define SIGNAL_BLINK_MS         450

static gSignal[MAX_VEHICLES];
static gLamp[MAX_VEHICLES][4];
static bool:gLampVisible[MAX_VEHICLES];

stock Signals_Init()
{
    for (new v = 0; v < MAX_VEHICLES; v++)
        for (new i = 0; i < 4; i++) gLamp[v][i] = INVALID_OBJECT_ID;
    SetTimer("Signals_Blink", SIGNAL_BLINK_MS, true);
}

stock Signals_OnKey(playerid, newkeys, oldkeys)
{
    new vehicleid = GetPlayerVehicleID(playerid);
    if (!vehicleid || GetPlayerState(playerid) != PLAYER_STATE_DRIVER) return 0;

    #define PRESSED(%0) ((newkeys & (%0)) && !(oldkeys & (%0)))
    if (PRESSED(KEY_LOOK_LEFT))       Signals_Toggle(vehicleid, SIGNAL_LEFT);
    else if (PRESSED(KEY_LOOK_RIGHT)) Signals_Toggle(vehicleid, SIGNAL_RIGHT);
    else if (PRESSED(KEY_CROUCH))     Signals_Toggle(vehicleid, SIGNAL_HAZARD);
    #undef PRESSED
    return 1;
}

static stock Signals_Toggle(vehicleid, mode)
{
    gSignal[vehicleid] = (gSignal[vehicleid] == mode) ? SIGNAL_OFF : mode;
    Signals_HideLamps(vehicleid);
}

forward Signals_Blink();
public Signals_Blink()
{
    for (new v = 1; v < MAX_VEHICLES; v++)
    {
        if (gSignal[v] == SIGNAL_OFF) continue;
        if (!GetVehicleModel(v)) { gSignal[v] = SIGNAL_OFF; Signals_HideLamps(v); continue; }

        if (gLampVisible[v]) Signals_HideLamps(v);
        else Signals_ShowLamps(v);
    }
}

static stock Signals_ShowLamps(vehicleid)
{
    new Float:sx, Float:sy, Float:sz;
    GetVehicleModelInfo(GetVehicleModel(vehicleid), VEHICLE_MODEL_INFO_SIZE, sx, sy, sz);
    new Float:hx = sx / 2.0 - 0.1, Float:hy = sy / 2.0 - 0.1;

    // 0 перед-лево, 1 зад-лево, 2 перед-право, 3 зад-право
    new bool:left = gSignal[vehicleid] != SIGNAL_RIGHT, bool:right = gSignal[vehicleid] != SIGNAL_LEFT;
    if (left)
    {
        gLamp[vehicleid][0] = Signals_Lamp(vehicleid, -hx,  hy);
        gLamp[vehicleid][1] = Signals_Lamp(vehicleid, -hx, -hy);
    }
    if (right)
    {
        gLamp[vehicleid][2] = Signals_Lamp(vehicleid, hx,  hy);
        gLamp[vehicleid][3] = Signals_Lamp(vehicleid, hx, -hy);
    }
    gLampVisible[vehicleid] = true;
}

static stock Signals_Lamp(vehicleid, Float:x, Float:y)
{
    new obj = CreateObject(SIGNAL_LAMP_MODEL, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 60.0);
    AttachObjectToVehicle(obj, vehicleid, x, y, 0.0, 0.0, 0.0, 0.0);
    return obj;
}

static stock Signals_HideLamps(vehicleid)
{
    for (new i = 0; i < 4; i++)
    {
        if (gLamp[vehicleid][i] == INVALID_OBJECT_ID) continue;
        DestroyObject(gLamp[vehicleid][i]);
        gLamp[vehicleid][i] = INVALID_OBJECT_ID;
    }
    gLampVisible[vehicleid] = false;
}
