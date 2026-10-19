"""Render portfolio cards: highlighted Pawn excerpts from src/ on a dark canvas."""
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont
from pygments.lexers import CLexer
from pygments.token import Comment, Keyword, Name, Number, Operator, String

ROOT = Path(__file__).resolve().parent.parent
OUT = ROOT / "portfolio"
FONTS = Path("C:/Windows/Fonts")
CODE_FONT = ImageFont.truetype(str(FONTS / "consola.ttf"), 22)
TITLE_FONT = ImageFont.truetype(str(FONTS / "segoeuib.ttf"), 64)
SUB_FONT = ImageFont.truetype(str(FONTS / "segoeui.ttf"), 32)
TAG_FONT = ImageFont.truetype(str(FONTS / "segoeuib.ttf"), 26)

BG, PANEL, BAR = (13, 17, 23), (22, 27, 34), (33, 38, 46)
COLORS = [
    (Comment, (125, 133, 144)), (String, (165, 214, 255)), (Number, (121, 192, 255)),
    (Keyword, (255, 123, 114)), (Name.Function, (210, 168, 255)), (Operator, (255, 166, 87)),
]
DEFAULT = (230, 237, 243)

WORKS = [
    ("autosalon", "Автосалон", "Список машин с ценами, покупка с проверкой денег, одна личная машина",
     "autosalon.inc", ["Autosalon_OnDialog"], (51, 204, 255)),
    ("gibdd", "Система ГИБДД", "Смена инспектора, штрафы с причиной, список и оплата штрафов",
     "gibdd.inc", ["Gibdd_CmdFine"], (51, 153, 255)),
    ("turnsignals", "Поворотники и аварийка", "Лампы по габаритам модели, мигание по таймеру, Q, E и аварийка",
     "turnsignals.inc", ["Signals_ShowLamps", "Signals_Lamp"], (255, 170, 0)),
    ("speedometer", "Спидометр и топливо", "Скорость, бензин и состояние на экране, глохнет на пустом баке",
     "speedometer.inc", ["Speedo_Update"], (60, 210, 120)),
    ("admin", "Админ-система", "Уровни доступа, админ-чат, телепорт, выдача машины, кик с причиной",
     "admin.inc", ["Admin_CmdChat", "Admin_CmdKick"], (255, 204, 0)),
]


def excerpt(path, names):
    lines = path.read_text(encoding="utf-8").splitlines()
    picked = []
    for name in names:
        start = next(i for i, line in enumerate(lines)
                     if line.startswith(("stock", "static stock", "public")) and f"{name}(" in line)
        while start > 0 and lines[start - 1].startswith("//"):
            start -= 1
        end = next(i for i in range(start, len(lines)) if lines[i] == "}")
        picked += lines[start:end + 1] + [""]
    return "\n".join(picked).rstrip()


def color_of(token):
    for kind, color in COLORS:
        if token in kind:
            return color
    return DEFAULT


def render(slug, title, subtitle, src, names, accent):
    code = excerpt(ROOT / "src" / src, names)
    w, h = 1920, 1280
    img = Image.new("RGB", (w, h), BG)
    d = ImageDraw.Draw(img)

    d.rectangle([0, 0, 14, h], fill=accent)
    d.text((80, 60), title, font=TITLE_FONT, fill=(255, 255, 255))
    d.text((80, 150), subtitle, font=SUB_FONT, fill=(170, 180, 192))
    tag_x = 80
    for tag in ("SAMP", "CRMP", "open.mp", "Pawn"):
        tw = d.textlength(tag, font=TAG_FONT)
        d.rounded_rectangle([tag_x, 215, tag_x + tw + 32, 259], 10, fill=BAR)
        d.text((tag_x + 16, 219), tag, font=TAG_FONT, fill=accent)
        tag_x += tw + 48

    top, left = 300, 80
    d.rounded_rectangle([left, top, w - 80, h - 60], 18, fill=PANEL)
    d.rounded_rectangle([left, top, w - 80, top + 52], 18, fill=BAR)
    d.rectangle([left, top + 30, w - 80, top + 52], fill=BAR)
    for i, c in enumerate([(255, 95, 86), (255, 189, 46), (39, 201, 63)]):
        d.ellipse([left + 24 + i * 30, top + 18, left + 40 + i * 30, top + 34], fill=c)
    d.text((left + 130, top + 12), src, font=TAG_FONT, fill=(170, 180, 192))

    x0, y, line_h = left + 40, top + 76, 28
    max_lines = (h - 80 - y) // line_h
    lines_drawn, x = 0, x0
    for token, value in CLexer().get_tokens(code):
        for k, part in enumerate(value.split("\n")):
            if k:
                lines_drawn += 1
                y += line_h
                x = x0
            if lines_drawn >= max_lines:
                break
            if part:
                d.text((x, y), part, font=CODE_FONT, fill=color_of(token))
                x += d.textlength(part, font=CODE_FONT)

    OUT.mkdir(exist_ok=True)
    img.save(OUT / f"{slug}.png")
    # Cover for the portfolio card: the same design cropped to the header and first code lines.
    img.crop((0, 0, 1920, 1280)).resize((990, 660), Image.LANCZOS).save(OUT / f"{slug}-cover.jpg", quality=88)
    return lines_drawn, code.count("\n") + 1


if __name__ == "__main__":
    for work in WORKS:
        shown, total = render(*work)
        print(work[0], f"{shown}/{total} lines")
