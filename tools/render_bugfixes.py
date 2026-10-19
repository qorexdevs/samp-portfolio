"""Render before/after bug-fix cards from src/bugfixes.md on a light canvas."""
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont
from pygments.lexers import CLexer
from pygments.token import Comment, Keyword, Name, Number, Operator, String

ROOT = Path(__file__).resolve().parent.parent
OUT = ROOT / "portfolio-bugs"
FONTS = Path("C:/Windows/Fonts")
CODE_FONT = ImageFont.truetype(str(FONTS / "CascadiaMono.ttf"), 21)
TITLE_FONT = ImageFont.truetype(str(FONTS / "segoeuib.ttf"), 56)
LABEL_FONT = ImageFont.truetype(str(FONTS / "segoeuib.ttf"), 28)
TAG_FONT = ImageFont.truetype(str(FONTS / "segoeui.ttf"), 28)

BG, INK, MUTED = (246, 244, 239), (28, 31, 36), (110, 114, 122)
BAD, GOOD = (214, 69, 65), (34, 139, 84)
COLORS = [
    (Comment, (128, 128, 128)), (String, (163, 21, 21)), (Number, (9, 134, 88)),
    (Keyword, (0, 0, 200)), (Name.Function, (121, 94, 38)), (Operator, (90, 90, 90)),
]


def parse(text):
    cards = []
    for block in text.split("\n## ")[1:]:
        lines = block.splitlines()
        slug, title = lines[0].strip(), lines[1].strip()
        body = "\n".join(lines[2:])
        before = body.split("--- before\n")[1].split("--- after\n")[0].rstrip()
        after = body.split("--- after\n")[1].rstrip()
        cards.append((slug, title, before, after))
    return cards


def color_of(token):
    for kind, color in COLORS:
        if token in kind:
            return color
    return INK


def draw_code(d, code, x0, y, line_h):
    x = x0
    for token, value in CLexer().get_tokens(code):
        for k, part in enumerate(value.split("\n")):
            if k:
                y += line_h
                x = x0
            if part:
                d.text((x, y), part, font=CODE_FONT, fill=color_of(token))
                x += d.textlength(part, font=CODE_FONT)
    return y


def panel(d, top, code, label, color, w):
    line_h = 29
    height = 70 + (code.count("\n") + 1) * line_h + 30
    d.rounded_rectangle([80, top, w - 80, top + height], 16, fill=(255, 255, 255), outline=color, width=3)
    d.rounded_rectangle([80, top, 80 + 12, top + height], 6, fill=color)
    d.text((120, top + 20), label, font=LABEL_FONT, fill=color)
    draw_code(d, code, 120, top + 70, line_h)
    return top + height


def render(slug, title, before, after):
    w, h = 1920, 1280
    img = Image.new("RGB", (w, h), BG)
    d = ImageDraw.Draw(img)
    d.text((80, 50), title, font=TITLE_FONT, fill=INK)
    d.text((80, 130), "Типичная ошибка в моде SAMP / CRMP и как я ее исправляю", font=TAG_FONT, fill=MUTED)

    bottom = panel(d, 200, before, "Было", BAD, w)
    bottom = panel(d, bottom + 30, after, "Стало", GOOD, w)
    if bottom > h - 30:
        raise SystemExit(f"{slug}: card overflows by {bottom - h + 30}px")

    OUT.mkdir(exist_ok=True)
    img.save(OUT / f"bug-{slug}.png")
    img.resize((990, 660), Image.LANCZOS).save(OUT / f"bug-{slug}-cover.jpg", quality=88)


if __name__ == "__main__":
    for card in parse((ROOT / "src" / "bugfixes.md").read_text(encoding="utf-8")):
        render(*card)
        print("ok", card[0])
