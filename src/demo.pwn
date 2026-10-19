// Демо-мод для портфолио: автосалон, ГИБДД, поворотники, спидометр и админка.

#include <a_samp>

// Классические include SA-MP объявляют строки без const, отсюда ложные 239 на каждом литерале.
#pragma warning disable 239

// Следующий аргумент команды, разделитель пробел. idx сдвигается на начало остатка.
stock Cmd_NextArg(const params[], &idx)
{
    new arg[32], len = strlen(params), i = 0;
    while (idx < len && params[idx] == ' ') idx++;
    while (idx < len && params[idx] != ' ' && i < sizeof arg - 1) arg[i++] = params[idx++];
    while (idx < len && params[idx] == ' ') idx++;
    return arg;
}

#include "autosalon.inc"
#include "gibdd.inc"
#include "turnsignals.inc"
#include "speedometer.inc"
#include "admin.inc"

main() {}

public OnGameModeInit()
{
    SetGameModeText("Portfolio demo");
    UsePlayerPedAnims();
    AddPlayerClass(0, 2120.0, -1145.0, 24.5, 270.0, 0, 0, 0, 0, 0, 0);

    Autosalon_Init();
    Signals_Init();
    Speedo_Init();
    print("[demo] autosalon, gibdd, turnsignals, speedometer, admin: loaded");
    return 1;
}

public OnPlayerConnect(playerid)
{
    Gibdd_OnConnect(playerid);
    Admin_OnConnect(playerid);
    GivePlayerMoney(playerid, 1000000);
    SendClientMessage(playerid, 0xFFFFFFFF, "Команды: /duty /fine /fines /payfine /fill /admins /a /goto /veh /kick");
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    Autosalon_OnDisconnect(playerid);
    Speedo_Hide(playerid);
    return 1;
}

public OnPlayerPickUpPickup(playerid, pickupid)
{
    Autosalon_OnPickup(playerid, pickupid);
    return 1;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    if (Autosalon_OnDialog(playerid, dialogid, response, listitem)) return 1;
    if (Gibdd_OnDialog(playerid, dialogid, response, listitem)) return 1;
    return 0;
}

public OnPlayerStateChange(playerid, newstate, oldstate)
{
    Speedo_OnStateChange(playerid, newstate);
    return 1;
}

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    Signals_OnKey(playerid, newkeys, oldkeys);
    return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
    new cmd[24], i = 1, p = 0;
    while (cmdtext[i] > ' ' && p < sizeof cmd - 1) cmd[p++] = cmdtext[i++];
    while (cmdtext[i] == ' ') i++;
    new params[128];
    strmid(params, cmdtext, i, strlen(cmdtext));

    if (!strcmp(cmd, "duty", true))    return Gibdd_CmdDuty(playerid);
    if (!strcmp(cmd, "fine", true))    return Gibdd_CmdFine(playerid, params);
    if (!strcmp(cmd, "fines", true))   return Gibdd_CmdFines(playerid);
    if (!strcmp(cmd, "payfine", true)) return Gibdd_CmdPayFine(playerid);
    if (!strcmp(cmd, "fill", true))    return Speedo_CmdFill(playerid);
    if (!strcmp(cmd, "admins", true))  return Admin_CmdAdmins(playerid);
    if (!strcmp(cmd, "a", true))       return Admin_CmdChat(playerid, params);
    if (!strcmp(cmd, "goto", true))    return Admin_CmdGoto(playerid, params);
    if (!strcmp(cmd, "veh", true))     return Admin_CmdVeh(playerid, params);
    if (!strcmp(cmd, "kick", true))    return Admin_CmdKick(playerid, params);
    return 0;
}
