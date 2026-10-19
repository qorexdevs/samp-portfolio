// ГИБДД: служба по команде, штрафы с причиной, список и оплата штрафов.

#define DIALOG_FINES            1100
#define MAX_FINES               5

enum E_FINE
{
    f_Amount,
    f_Reason[48]
}

static bool:gOnDuty[MAX_PLAYERS];
static gFines[MAX_PLAYERS][MAX_FINES][E_FINE];
static gFineCount[MAX_PLAYERS];

stock Gibdd_OnConnect(playerid)
{
    gOnDuty[playerid] = false;
    gFineCount[playerid] = 0;
}

// /duty: заступить или уйти со смены инспектора.
stock Gibdd_CmdDuty(playerid)
{
    gOnDuty[playerid] = !gOnDuty[playerid];
    if (gOnDuty[playerid])
    {
        SetPlayerSkin(playerid, 284);
        SetPlayerColor(playerid, 0x3399FFFF);
        GivePlayerWeapon(playerid, 3, 1);
        SendClientMessage(playerid, 0x3399FFFF, "Вы заступили на смену инспектора ГИБДД.");
    }
    else
    {
        SetPlayerColor(playerid, 0xFFFFFFFF);
        SendClientMessage(playerid, 0x3399FFFF, "Смена окончена.");
    }
    return 1;
}

// /fine [id] [сумма] [причина]
stock Gibdd_CmdFine(playerid, const params[])
{
    if (!gOnDuty[playerid]) return SendClientMessage(playerid, 0xFF6347FF, "Штрафы выписывает только инспектор на смене (/duty).");

    new idx, target = strval(Cmd_NextArg(params, idx)), amount = strval(Cmd_NextArg(params, idx));
    new reason[48];
    strmid(reason, params[idx], 0, sizeof reason - 1);

    if (!IsPlayerConnected(target) || amount <= 0 || !reason[0])
        return SendClientMessage(playerid, 0xAAAAAAFF, "Использование: /fine [id] [сумма] [причина]");
    if (gFineCount[target] >= MAX_FINES)
        return SendClientMessage(playerid, 0xFF6347FF, "У водителя уже максимум неоплаченных штрафов.");

    new slot = gFineCount[target]++;
    gFines[target][slot][f_Amount] = amount;
    strmid(gFines[target][slot][f_Reason], reason, 0, strlen(reason), 48);

    new msg[128];
    format(msg, sizeof msg, "Инспектор выписал вам штраф $%d: %s. Оплата: /payfine", amount, reason);
    SendClientMessage(target, 0xFFCC00FF, msg);
    GameTextForPlayer(target, "~r~SHTRAF", 2500, 1);
    return SendClientMessage(playerid, 0x3399FFFF, "Штраф выписан.");
}

// /fines: список неоплаченных штрафов.
stock Gibdd_CmdFines(playerid)
{
    if (!gFineCount[playerid]) return SendClientMessage(playerid, 0x33CC33FF, "Неоплаченных штрафов нет.");

    new list[512] = "Причина\tСумма\n", line[80];
    for (new i = 0; i < gFineCount[playerid]; i++)
    {
        format(line, sizeof line, "%s\t{FF6347}$%d\n", gFines[playerid][i][f_Reason], gFines[playerid][i][f_Amount]);
        strcat(list, line);
    }
    return ShowPlayerDialog(playerid, DIALOG_FINES, DIALOG_STYLE_TABLIST_HEADERS, "Штрафы ГИБДД", list, "Оплатить", "Закрыть");
}

stock Gibdd_OnDialog(playerid, dialogid, response, listitem)
{
    if (dialogid != DIALOG_FINES) return 0;
    if (response && listitem >= 0 && listitem < gFineCount[playerid]) Gibdd_Pay(playerid, listitem);
    return 1;
}

// /payfine: оплатить первый штраф из списка.
stock Gibdd_CmdPayFine(playerid)
{
    if (!gFineCount[playerid]) return SendClientMessage(playerid, 0x33CC33FF, "Неоплаченных штрафов нет.");
    return Gibdd_Pay(playerid, 0);
}

static stock Gibdd_Pay(playerid, slot)
{
    new amount = gFines[playerid][slot][f_Amount];
    if (GetPlayerMoney(playerid) < amount) return SendClientMessage(playerid, 0xFF6347FF, "Не хватает денег на оплату штрафа.");

    GivePlayerMoney(playerid, -amount);
    for (new i = slot; i < gFineCount[playerid] - 1; i++) gFines[playerid][i] = gFines[playerid][i + 1];
    gFineCount[playerid]--;
    return SendClientMessage(playerid, 0x33CC33FF, "Штраф оплачен.");
}
