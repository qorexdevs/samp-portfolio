#!/usr/bin/env bash
# Проверка, что сервер отвечает на SA-MP query. Если завис, перезапускаем службу.
# cron: */2 * * * * /opt/omp/healthcheck.sh
set -uo pipefail

HOST=127.0.0.1
PORT=7777

# Пакет query "i": SAMP + IP + порт + опкод. Шлем сразу в nc: в переменной bash нулевые байты теряются.
query() {
    printf 'SAMP\x7f\x00\x00\x01'
    printf "\\x$(printf %02x $((PORT & 255)))\\x$(printf %02x $((PORT >> 8)))"
    printf 'i'
}

if ! query | timeout 3 nc -u -w 2 "$HOST" "$PORT" | head -c 4 | grep -q SAMP; then
    logger -t omp-health "сервер не отвечает на query, перезапуск"
    systemctl restart omp
fi
