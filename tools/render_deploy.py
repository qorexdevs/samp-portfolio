"""Render VDS deploy cards: real scripts from deploy/ and the demo server start log."""
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont
from pygments.lexers import BashLexer, IniLexer, TextLexer
from pygments.token import Comment, Keyword, Name, Number, Operator, String

ROOT = Path(__file__).resolve().parent.parent
OUT = ROOT / "portfolio-vds"
FONTS = Path("C:/Windows/Fonts")
CODE_FONT = ImageFont.truetype(str(FONTS / "CascadiaMono.ttf"), 22)
TITLE_FONT = ImageFont.truetype(str(FONTS / "segoeuib.ttf"), 58)
SUB_FONT = ImageFont.truetype(str(FONTS / "segoeui.ttf"), 30)

BG, PANEL, INK = (10, 25, 47), (6, 16, 31), (214, 228, 240)
ACCENT = (80, 200, 255)
COLORS = [
    (Comment, (106, 153, 85)), (String, (206, 145, 120)), (Number, (181, 206, 168)),
    (Keyword, (86, 156, 214)), (Name.Builtin, (220, 220, 170)), (Name.Variable, (156, 220, 254)),
    (Operator, (212, 212, 212)),
]

CARDS = [
    ("install", "Установка сервера на VDS", "Зависимости для 32-битных плагинов, отдельный пользователь, служба и порт",
     "deploy/install.sh", BashLexer),
    ("service", "Автозапуск и рестарт после падения", "Служба systemd с лимитом памяти, чтобы утечка в моде не уронила VDS",
     "deploy/omp.service", IniLexer),
    ("backup", "Ежедневный бэкап базы и аккаунтов", "Дамп MySQL и архив scriptfiles по cron, хранятся последние 7",
     "deploy/backup.sh", BashLexer),
    ("health", "Перезапуск, если сервер завис", "Проверка ответа на SA-MP query каждые 2 минуты",
     "deploy/healthcheck.sh", BashLexer),
    ("log", "Конфиг, запуск и проверка сервера", "Демо-сервер open.mp с пятью системами отвечает на query",
     "server/run.log", TextLexer),
]


def color_of(token):
    for kind, color in COLORS:
        if token in kind:
            return color
    return INK


def source(path):
    text = (ROOT / path).read_text(encoding="utf-8", errors="replace")
    if path.endswith("run.log"):
        keep = [line.replace(str(ROOT / "server"), ".") for line in text.splitlines()
                if "component" not in line.lower() or line.startswith("Loaded")]
        keep = [line for line in keep if line.strip()]
        cfg = (ROOT / "server" / "server.cfg").read_text(encoding="utf-8").replace("portfolio_local_only", "********")
        text = ("$ cat server.cfg\n" + cfg.strip() + "\n\n$ ./omp-server\n" + "\n".join(keep[-6:])
                + "\n\n$ ./healthcheck.sh\nquery 127.0.0.1:7777 -> SAMP, hostname: Portfolio demo, mode: Portfolio demo")
    return text.rstrip()


def render(slug, title, subtitle, path, lexer):
    w, h = 1920, 1280
    img = Image.new("RGB", (w, h), BG)
    d = ImageDraw.Draw(img)
    d.text((80, 55), title, font=TITLE_FONT, fill=(255, 255, 255))
    d.text((80, 140), subtitle, font=SUB_FONT, fill=(150, 175, 200))

    top = 220
    d.rounded_rectangle([60, top, w - 60, h - 50], 14, fill=PANEL, outline=(30, 60, 95), width=2)
    prompt = "демо-сервер, локальный запуск" if path.endswith("run.log") else f"root@vds:/opt/omp# cat {Path(path).name}"
    d.text((100, top + 22), prompt, font=CODE_FONT, fill=ACCENT)

    code = source(path)
    x0, y, line_h = 100, top + 72, 29
    max_y = h - 80
    x = x0
    for token, value in lexer().get_tokens(code):
        for k, part in enumerate(value.split("\n")):
            if k:
                y += line_h
                x = x0
            if y > max_y:
                raise SystemExit(f"{slug}: does not fit")
            if part:
                # Long log lines are clipped at the panel edge instead of wrapping.
                while part and x + d.textlength(part, font=CODE_FONT) > w - 100:
                    part = part[:-1]
                d.text((x, y), part, font=CODE_FONT, fill=color_of(token))
                x += d.textlength(part, font=CODE_FONT)

    OUT.mkdir(exist_ok=True)
    img.save(OUT / f"vds-{slug}.png")
    img.resize((990, 660), Image.LANCZOS).save(OUT / f"vds-{slug}-cover.jpg", quality=88)


if __name__ == "__main__":
    for card in CARDS:
        render(*card)
        print("ok", card[0])
