// Автосалон: пикап у входа, список машин с ценами, покупка с проверкой денег.

#define DIALOG_AUTOSALON        1000
#define AUTOSALON_X             2131.8
#define AUTOSALON_Y             -1150.4
#define AUTOSALON_Z             24.1

enum E_SALON_CAR
{
    sc_Model,
    sc_Name[24],
    sc_Price
}

static const gSalonCars[][E_SALON_CAR] =
{
    {402, "Buffalo",  310000},
    {445, "Admiral",  150000},
    {560, "Sultan",   420000},
    {541, "Bullet",   780000},
    {411, "Infernus", 950000}
};

static gSalonPickup;
static gPlayerCar[MAX_PLAYERS] = {INVALID_VEHICLE_ID, ...};

stock Autosalon_Init()
{
    gSalonPickup = CreatePickup(1274, 1, AUTOSALON_X, AUTOSALON_Y, AUTOSALON_Z, 0);
    Create3DTextLabel("Автосалон\nвстаньте на значок", 0x33CCFFFF,
        AUTOSALON_X, AUTOSALON_Y, AUTOSALON_Z + 0.8, 15.0, 0, 1);
}

stock Autosalon_OnPickup(playerid, pickupid)
{
    if (pickupid != gSalonPickup) return 0;
    Autosalon_ShowMenu(playerid);
    return 1;
}

stock Autosalon_ShowMenu(playerid)
{
    new list[512] = "Модель\tЦена\n", line[64];
    for (new i = 0; i < sizeof gSalonCars; i++)
    {
        format(line, sizeof line, "%s\t{33CC33}$%d\n", gSalonCars[i][sc_Name], gSalonCars[i][sc_Price]);
        strcat(list, line);
    }
    ShowPlayerDialog(playerid, DIALOG_AUTOSALON, DIALOG_STYLE_TABLIST_HEADERS,
        "Автосалон", list, "Купить", "Выход");
}

stock Autosalon_OnDialog(playerid, dialogid, response, listitem)
{
    if (dialogid != DIALOG_AUTOSALON) return 0;
    if (!response || listitem < 0 || listitem >= sizeof gSalonCars) return 1;

    new price = gSalonCars[listitem][sc_Price];
    if (GetPlayerMoney(playerid) < price)
    {
        SendClientMessage(playerid, 0xFF6347FF, "Не хватает денег на эту машину.");
        return 1;
    }

    // У игрока одна личная машина: старую убираем, чтобы не плодить транспорт на карте.
    if (gPlayerCar[playerid] != INVALID_VEHICLE_ID) DestroyVehicle(gPlayerCar[playerid]);

    GivePlayerMoney(playerid, -price);
    gPlayerCar[playerid] = CreateVehicle(gSalonCars[listitem][sc_Model],
        AUTOSALON_X + 6.0, AUTOSALON_Y, AUTOSALON_Z, 90.0, -1, -1, -1);
    PutPlayerInVehicle(playerid, gPlayerCar[playerid], 0);

    new msg[96];
    format(msg, sizeof msg, "Вы купили %s за $%d. Машина ваша.", gSalonCars[listitem][sc_Name], price);
    SendClientMessage(playerid, 0x33CC33FF, msg);
    return 1;
}

stock Autosalon_OnDisconnect(playerid)
{
    if (gPlayerCar[playerid] != INVALID_VEHICLE_ID)
    {
        DestroyVehicle(gPlayerCar[playerid]);
        gPlayerCar[playerid] = INVALID_VEHICLE_ID;
    }
}
