// Админ-система: уровни доступа, админ-чат, телепорт, выдача машины и кик с причиной.

#define ADMIN_HELPER            1
#define ADMIN_MODERATOR         2
#define ADMIN_LEAD              3

static gAdminLevel[MAX_PLAYERS];
static const gLevelName[][] = {"Игрок", "Хелпер", "Модератор", "Гл. администратор"};

stock Admin_OnConnect(playerid)
{
    // На демо-сервере локальный игрок получает полный доступ, на проекте уровень берется из базы.
    gAdminLevel[playerid] = (IsPlayerAdmin(playerid) || !strcmp(Player_Ip(playerid), "127.0.0.1")) ? ADMIN_LEAD : 0;
}

static stock bool:Admin_Has(playerid, level)
{
    if (gAdminLevel[playerid] >= level) return true;
    SendClientMessage(playerid, 0xFF6347FF, "Недостаточно прав для этой команды.");
    return false;
}

// /admins: кто из администрации в сети.
stock Admin_CmdAdmins(playerid)
{
    new list[512] = "Ник\tДолжность\n", line[64], name[MAX_PLAYER_NAME];
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (!IsPlayerConnected(i) || !gAdminLevel[i]) continue;
        GetPlayerName(i, name, sizeof name);
        format(line, sizeof line, "%s\t{FFCC00}%s\n", name, gLevelName[gAdminLevel[i]]);
        strcat(list, line);
    }
    return ShowPlayerDialog(playerid, 1200, DIALOG_STYLE_TABLIST_HEADERS, "Администрация онлайн", list, "Ок", "");
}

// /a [текст]: сообщение в админ-чат.
stock Admin_CmdChat(playerid, const params[])
{
    if (!Admin_Has(playerid, ADMIN_HELPER)) return 1;
    if (!params[0]) return SendClientMessage(playerid, 0xAAAAAAFF, "Использование: /a [текст]");

    new msg[144], name[MAX_PLAYER_NAME];
    GetPlayerName(playerid, name, sizeof name);
    format(msg, sizeof msg, "[A] %s %s: %s", gLevelName[gAdminLevel[playerid]], name, params);
    for (new i = 0; i < MAX_PLAYERS; i++)
        if (IsPlayerConnected(i) && gAdminLevel[i]) SendClientMessage(i, 0x99CC00FF, msg);
    return 1;
}

// /goto [id]
stock Admin_CmdGoto(playerid, const params[])
{
    if (!Admin_Has(playerid, ADMIN_MODERATOR)) return 1;
    new target = strval(params);
    if (!IsPlayerConnected(target)) return SendClientMessage(playerid, 0xAAAAAAFF, "Использование: /goto [id]");

    new Float:x, Float:y, Float:z;
    GetPlayerPos(target, x, y, z);
    SetPlayerInterior(playerid, GetPlayerInterior(target));
    SetPlayerVirtualWorld(playerid, GetPlayerVirtualWorld(target));
    return SetPlayerPos(playerid, x + 1.0, y, z);
}

// /veh [модель 400-611]
stock Admin_CmdVeh(playerid, const params[])
{
    if (!Admin_Has(playerid, ADMIN_LEAD)) return 1;
    new model = strval(params);
    if (model < 400 || model > 611) return SendClientMessage(playerid, 0xAAAAAAFF, "Использование: /veh [400-611]");

    new Float:x, Float:y, Float:z, Float:a;
    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, a);
    return PutPlayerInVehicle(playerid, CreateVehicle(model, x, y, z + 0.5, a, -1, -1, -1), 0);
}

// /kick [id] [причина]
stock Admin_CmdKick(playerid, const params[])
{
    if (!Admin_Has(playerid, ADMIN_MODERATOR)) return 1;
    new idx, target = strval(Cmd_NextArg(params, idx));
    if (!IsPlayerConnected(target) || !params[idx]) return SendClientMessage(playerid, 0xAAAAAAFF, "Использование: /kick [id] [причина]");

    new msg[144], admin[MAX_PLAYER_NAME], name[MAX_PLAYER_NAME];
    GetPlayerName(playerid, admin, sizeof admin);
    GetPlayerName(target, name, sizeof name);
    format(msg, sizeof msg, "Администратор %s кикнул %s. Причина: %s", admin, name, params[idx]);
    SendClientMessageToAll(0xFF6347FF, msg);
    // Kick в том же тике обрезает сообщение у кикнутого, поэтому отправляем его с задержкой.
    SetTimerEx("Admin_DoKick", 300, false, "i", target);
    return 1;
}

forward Admin_DoKick(playerid);
public Admin_DoKick(playerid) Kick(playerid);

static stock Player_Ip(playerid)
{
    new ip[16];
    GetPlayerIp(playerid, ip, sizeof ip);
    return ip;
}
