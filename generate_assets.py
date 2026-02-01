from PIL import Image, ImageDraw

# Create assets directory if not exists
import os
os.makedirs("assets/minecraft/textures/blocks", exist_ok=True)
os.makedirs("assets/minecraft/font", exist_ok=True)

# Generate block textures (16x16)
def create_block(name, color):
    img = Image.new("RGBA", (16, 16), color)
    draw = ImageDraw.Draw(img)
    draw.rectangle([0, 0, 15, 15], outline=(0, 0, 0, 50))
    img.save(f"assets/minecraft/textures/blocks/{name}.png")

create_block("stone", (128, 128, 128))
create_block("dirt", (134, 96, 67))
create_block("grass_top", (124, 189, 110))
create_block("grass_side", (134, 96, 67))
create_block("log_top", (100, 80, 50))
create_block("log_side", (80, 60, 40))
create_block("leaves", (50, 150, 50))

# Generate ascii.png (128x128 grid of 8x8 characters)
font_img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
draw = ImageDraw.Draw(font_img)
# Draw some simple letters for verification
for i in range(256):
    x = (i % 16) * 8
    y = (i // 16) * 8
    char = chr(i) if 32 <= i <= 126 else "?"
    draw.text((x + 1, y), char, fill=(255, 255, 255))

font_img.save("assets/minecraft/font/ascii.png")
print("Placeholder assets generated.")
