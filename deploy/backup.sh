#!/usr/bin/env bash
# Ежедневный бэкап аккаунтов и базы. Хранит последние 7 архивов.
# cron: 30 5 * * * /opt/omp/backup.sh >> /var/log/omp-backup.log 2>&1
set -euo pipefail

SRC=/opt/omp
DEST=/var/backups/omp
STAMP=$(date +%Y%m%d-%H%M)
mkdir -p "$DEST"

# Дамп MySQL делаем отдельно, копия файлов работающей базы может быть битой.
if command -v mysqldump >/dev/null; then
    mysqldump --single-transaction samp > "$SRC/scriptfiles/db-$STAMP.sql"
fi

tar -czf "$DEST/omp-$STAMP.tar.gz" -C "$SRC" scriptfiles server.cfg config.json 2>/dev/null || \
    tar -czf "$DEST/omp-$STAMP.tar.gz" -C "$SRC" scriptfiles
rm -f "$SRC"/scriptfiles/db-*.sql

ls -1t "$DEST"/omp-*.tar.gz | tail -n +8 | xargs -r rm -f
echo "$(date '+%F %T') backup ok: omp-$STAMP.tar.gz"
