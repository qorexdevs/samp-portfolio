# Типичные баги SAMP и исправления (для карточек портфолио)

## kick
Сообщение о кике не доходит до игрока
--- before
format(msg, sizeof msg, "Вы кикнуты: %s", reason);
SendClientMessage(target, -1, msg);
Kick(target); // сообщение обрезается, игрок видит только дисконнект
--- after
format(msg, sizeof msg, "Вы кикнуты: %s", reason);
SendClientMessage(target, -1, msg);
SetTimerEx("DelayedKick", 300, false, "i", target);

forward DelayedKick(playerid);
public DelayedKick(playerid) Kick(playerid);

## vehicle
Купленные машины копятся на карте после выхода игроков
--- before
public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    CreateVehicle(model, x, y, z, a, -1, -1, -1); // id никуда не сохраняется
    return 1;
}
--- after
static gPlayerCar[MAX_PLAYERS] = {INVALID_VEHICLE_ID, ...};

if (gPlayerCar[playerid] != INVALID_VEHICLE_ID) DestroyVehicle(gPlayerCar[playerid]);
gPlayerCar[playerid] = CreateVehicle(model, x, y, z, a, -1, -1, -1);

public OnPlayerDisconnect(playerid, reason)
{
    if (gPlayerCar[playerid] != INVALID_VEHICLE_ID) DestroyVehicle(gPlayerCar[playerid]);
    gPlayerCar[playerid] = INVALID_VEHICLE_ID;
    return 1;
}

## listitem
Run time error 4 при закрытии диалога
--- before
public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    // при нажатии "Выход" listitem = -1
    new price = gSalonCars[listitem][sc_Price];
    GivePlayerMoney(playerid, -price);
    return 1;
}
--- after
public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    if (!response || listitem < 0 || listitem >= sizeof gSalonCars) return 1;
    new price = gSalonCars[listitem][sc_Price];
    if (GetPlayerMoney(playerid) < price) return SendClientMessage(playerid, -1, "Не хватает денег.");
    GivePlayerMoney(playerid, -price);
    return 1;
}

## timer
Таймер игрока продолжает работать после выхода
--- before
public OnPlayerConnect(playerid)
{
    SetTimerEx("PayDay", 60000, true, "i", playerid);
    return 1;
}
// новый игрок с тем же id получает зарплату дважды
--- after
static gPayTimer[MAX_PLAYERS];

public OnPlayerConnect(playerid)
{
    gPayTimer[playerid] = SetTimerEx("PayDay", 60000, true, "i", playerid);
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    KillTimer(gPayTimer[playerid]);
    return 1;
}

## buffer
Обрезанный текст и порча памяти в диалоге
--- before
new list[64];
for (new i = 0; i < sizeof gSalonCars; i++)
    format(list, sizeof list, "%s%s\t$%d\n", list, gSalonCars[i][sc_Name], gSalonCars[i][sc_Price]);
// format пишет в тот же буфер, который читает, и не влезает в 64 символа
--- after
new list[512] = "Модель\tЦена\n", line[64];
for (new i = 0; i < sizeof gSalonCars; i++)
{
    format(line, sizeof line, "%s\t$%d\n", gSalonCars[i][sc_Name], gSalonCars[i][sc_Price]);
    strcat(list, line);
}
