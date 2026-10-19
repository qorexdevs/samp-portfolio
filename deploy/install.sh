#!/usr/bin/env bash
# Установка open.mp сервера на Ubuntu/Debian VDS из архива проекта.
# Использование: sudo ./install.sh server.tar.gz
set -euo pipefail

ARCHIVE="${1:?укажите архив сервера}"
USER_NAME=omp
HOME_DIR=/opt/omp

apt-get update -q
# Плагины SA-MP собраны под 32 бита, без этих библиотек сервер не стартует.
dpkg --add-architecture i386
apt-get update -q
apt-get install -y -q libc6:i386 libstdc++6:i386 libssl3:i386 unzip

id "$USER_NAME" >/dev/null 2>&1 || useradd --system --home "$HOME_DIR" --shell /usr/sbin/nologin "$USER_NAME"
mkdir -p "$HOME_DIR"
tar -xzf "$ARCHIVE" -C "$HOME_DIR"
chmod +x "$HOME_DIR/omp-server"
chown -R "$USER_NAME:$USER_NAME" "$HOME_DIR"

install -m 644 omp.service /etc/systemd/system/omp.service
systemctl daemon-reload
systemctl enable --now omp

ufw allow 7777/udp
echo "Сервер запущен, лог: journalctl -u omp -f"
