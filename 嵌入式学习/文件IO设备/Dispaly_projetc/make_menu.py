# make_menu.py - 生成主界面 menu.jpg (1024x600)
# 按钮加大 + 文字加粗加大版
# 按钮视觉位置（命中区域在 menu.c 里比视觉大一圈）：
#   幻灯片 x[60,240] 音乐 x[264,444] 触摸 x[468,648] 设置 x[672,852]  y[505,575]
#   音乐键 圆心(920,545) 半径 34
from PIL import Image, ImageDraw, ImageFont

W, H = 1024, 600
FONT_TITLE = "C:/Windows/Fonts/msyhbd.ttc"
FONT_BTN = "C:/Windows/Fonts/msyhbd.ttc"

def font(path, size):
    return ImageFont.truetype(path, size)

img = Image.new("RGB", (W, H))
d = ImageDraw.Draw(img)

# 垂直渐变：深蓝 -> 深紫
top = (16, 24, 60)
bot = (60, 22, 92)
for y in range(H):
    t = y / H
    d.line([(0, y), (W, y)], fill=(
        int(top[0] + (bot[0] - top[0]) * t),
        int(top[1] + (bot[1] - top[1]) * t),
        int(top[2] + (bot[2] - top[2]) * t)))

# 装饰半透明圆圈
ov = Image.new("RGBA", (W, H), (0, 0, 0, 0))
od = ImageDraw.Draw(ov)
od.ellipse([-160, 40, 360, 560], outline=(255, 255, 255, 36), width=3)
od.ellipse([700, -120, 1150, 330], outline=(255, 255, 255, 26), width=3)
od.ellipse([840, 320, 1080, 560], outline=(255, 255, 255, 20), width=3)
img = Image.alpha_composite(img.convert("RGBA"), ov).convert("RGB")
d = ImageDraw.Draw(img)

# 相框图标（顶部）
d.rounded_rectangle([462, 34, 562, 134], radius=10, outline=(255, 255, 255, 255), width=3)
d.line([(462, 118), (497, 82), (522, 102), (541, 80), (562, 104)], fill=(255, 255, 255, 255), width=3)

# 标题
d.text((512, 158), "数 码 相 框", font=font(FONT_TITLE, 48), fill=(255, 255, 255, 255), anchor="mm")

# 底部半透明栏
bar = Image.new("RGBA", (W, 130), (0, 0, 0, 0))
bd = ImageDraw.Draw(bar)
bd.rectangle([0, 0, W, 130], fill=(0, 0, 0, 150))
img = img.convert("RGBA")
img.alpha_composite(bar, (0, H - 130))
img = img.convert("RGB")
d = ImageDraw.Draw(img)

# 按钮几何图标 + 文字
BTN_Y1, BTN_Y2 = 505, 575          # 按钮视觉区域（y 505~575，高 70）
BTN_XS = [60, 264, 468, 672]       # 4 个按钮左边缘
BTN_W = 180
LABELS = ["幻灯片", "音乐", "触摸", "设置"]

def draw_icon(kind, cx, cy):
    """在 (cx,cy) 画一个 24x24 左右的几何图标"""
    if kind == 0:    # 幻灯片：播放三角
        d.polygon([(cx - 13, cy - 11), (cx - 13, cy + 11), (cx + 13, cy)], fill=(255, 255, 255, 255))
    elif kind == 1:  # 音乐：音符（椭圆头 + 杆 + 旗）
        d.ellipse([cx - 9, cy - 15, cx + 7, cy + 3], fill=(255, 255, 255, 255))
        d.line([(cx - 1, cy - 7), (cx - 1, cy + 13)], fill=(255, 255, 255, 255), width=3)
        d.line([(cx - 1, cy - 7), (cx + 11, cy - 2), (cx + 11, cy + 5)], fill=(255, 255, 255, 255), width=3)
    elif kind == 2:  # 触摸：外圈 + 中心点
        d.ellipse([cx - 13, cy - 13, cx + 13, cy + 13], outline=(255, 255, 255, 255), width=3)
        d.ellipse([cx - 4, cy - 4, cx + 4, cy + 4], fill=(255, 255, 255, 255))
    else:            # 设置：圆 + 4 齿
        d.ellipse([cx - 13, cy - 13, cx + 13, cy + 13], outline=(255, 255, 255, 255), width=3)
        d.ellipse([cx - 5, cy - 5, cx + 5, cy + 5], fill=(255, 255, 255, 255))
        for dx, dy in [(-17, 0), (17, 0), (0, -17), (0, 17)]:
            d.rectangle([cx + dx - 4, cy + dy - 4, cx + dx + 4, cy + dy + 4], fill=(255, 255, 255, 255))

for i, x1 in enumerate(BTN_XS):
    x2 = x1 + BTN_W
    d.rounded_rectangle([x1, BTN_Y1, x2, BTN_Y2], radius=14,
                        fill=(30, 34, 56, 255), outline=(255, 255, 255, 255), width=2)
    cx = (x1 + x2) // 2
    draw_icon(i, cx, 524)                        # 图标（按钮内上部）
    d.text((cx, 556), LABELS[i], font=font(FONT_BTN, 34), fill=(255, 255, 255, 255), anchor="mm")  # 大字按钮文字

# 右下角音乐圆钮（音符）
d.ellipse([920 - 34, 545 - 34, 920 + 34, 545 + 34], outline=(255, 255, 255, 255), width=3)
d.ellipse([908, 522, 928, 546], fill=(255, 255, 255, 255))
d.line([(918, 534), (918, 572)], fill=(255, 255, 255, 255), width=4)
d.line([(918, 534), (936, 540), (936, 548)], fill=(255, 255, 255, 255), width=4)

img.save("D:/Gitee/cpp/嵌入式学习/文件IO设备/Dispaly_projetc/menu.jpg", quality=92)
print("menu.jpg written")
