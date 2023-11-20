from PIL import Image

colors = [
"757bb8",
"c6d9a1",
"2540c1",
"004bff",
"00beff",
"684026",
"d24c20",
"e8aae8",
"1d6d3b",
"47b74a",
"8fe968",
"eef78f",
"79dcf5",
"d2cfd5",
"a2a2a2",
"000000",
]

filename = "barrel"

img = Image.open(f"{filename}.png").convert("RGB")
pixels = img.load()

def rgb2hex(r, g, b):
    return '{:02x}{:02x}{:02x}'.format(r, g, b)

with open(f"{filename}.c", "w") as f:
    
    width = img.width
    height = img.height

    f.write(f"uint8_t sprite_{filename}[{int(width*height/2)}] = {{\n")

    for y in range(height):
        f.write("\t")

        for x in range(0, width, 2):
            color_index_0 = colors.index(rgb2hex(*pixels[(x, y)]))
            color_index_1 = colors.index(rgb2hex(*pixels[(x + 1, y)]))
            _hex = f"0x{hex(color_index_0)[2]}{hex(color_index_1)[2]}"
            f.write(f"{_hex}, ")
            
        f.write("\n")

    f.write(f"}};")

