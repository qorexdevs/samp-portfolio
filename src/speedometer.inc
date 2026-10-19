// Спидометр: скорость, топливо и состояние машины на экране водителя.
// Топливо расходуется на ходу, на нуле двигатель глохнет.

#define SPEEDO_UPDATE_MS        200
#define FUEL_PER_TICK           0.02

static PlayerText:gSpeedoTd[MAX_PLAYERS] = {PlayerText:INVALID_TEXT_DRAW, ...};
static Float:gFuel[MAX_VEHICLES];

stock Speedo_Init()
{
    for (new v = 0; v < MAX_VEHICLES; v++) gFuel[v] = 100.0;
    SetTimer("Speedo_Update", SPEEDO_UPDATE_MS, true);
}

stock Speedo_OnStateChange(playerid, newstate)
{
    if (newstate == PLAYER_STATE_DRIVER)
    {
        gSpeedoTd[playerid] = CreatePlayerTextDraw(playerid, 500.0, 360.0, "_");
        PlayerTextDrawFont(playerid, gSpeedoTd[playerid], 2);
        PlayerTextDrawLetterSize(playerid, gSpeedoTd[playerid], 0.32, 1.4);
        PlayerTextDrawSetOutline(playerid, gSpeedoTd[playerid], 1);
        PlayerTextDrawUseBox(playerid, gSpeedoTd[playerid], 1);
        PlayerTextDrawBoxColor(playerid, gSpeedoTd[playerid], 0x00000088);
        PlayerTextDrawTextSize(playerid, gSpeedoTd[playerid], 620.0, 0.0);
        PlayerTextDrawShow(playerid, gSpeedoTd[playerid]);
    }
    else Speedo_Hide(playerid);
}

stock Speedo_Hide(playerid)
{
    if (gSpeedoTd[playerid] == PlayerText:INVALID_TEXT_DRAW) return;
    PlayerTextDrawDestroy(playerid, gSpeedoTd[playerid]);
    gSpeedoTd[playerid] = PlayerText:INVALID_TEXT_DRAW;
}

forward Speedo_Update();
public Speedo_Update()
{
    new text[96];
    for (new p = 0; p < MAX_PLAYERS; p++)
    {
        if (gSpeedoTd[p] == PlayerText:INVALID_TEXT_DRAW) continue;
        new v = GetPlayerVehicleID(p);
        if (!v) continue;

        new Float:vx, Float:vy, Float:vz, Float:health;
        GetVehicleVelocity(v, vx, vy, vz);
        GetVehicleHealth(v, health);
        new speed = floatround(floatsqroot(vx * vx + vy * vy + vz * vz) * 180.0);

        if (speed > 2 && gFuel[v] > 0.0)
        {
            gFuel[v] -= FUEL_PER_TICK;
            if (gFuel[v] <= 0.0)
            {
                gFuel[v] = 0.0;
                Speedo_EngineOff(v);
                SendClientMessage(p, 0xFF6347FF, "Бензин закончился.");
            }
        }

        format(text, sizeof text, "~w~%d ~b~km/h~n~~y~Fuel: ~w~%.0f%%~n~~g~HP: ~w~%.0f",
            speed, gFuel[v], health / 10.0);
        PlayerTextDrawSetString(p, gSpeedoTd[p], text);
    }
}

static stock Speedo_EngineOff(vehicleid)
{
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    SetVehicleParamsEx(vehicleid, 0, lights, alarm, doors, bonnet, boot, objective);
}

// /fill: заправка до полного бака за $5 за литр.
stock Speedo_CmdFill(playerid)
{
    new v = GetPlayerVehicleID(playerid);
    if (!v) return SendClientMessage(playerid, 0xAAAAAAFF, "Нужно сидеть в машине.");

    new liters = floatround(100.0 - gFuel[v], floatround_ceil), price = liters * 5;
    if (!liters) return SendClientMessage(playerid, 0x33CC33FF, "Бак уже полный.");
    if (GetPlayerMoney(playerid) < price) return SendClientMessage(playerid, 0xFF6347FF, "Не хватает денег на заправку.");

    GivePlayerMoney(playerid, -price);
    gFuel[v] = 100.0;
    new msg[64];
    format(msg, sizeof msg, "Залито %d л за $%d.", liters, price);
    return SendClientMessage(playerid, 0x33CC33FF, msg);
}
