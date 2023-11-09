from PIL import Image

colors = [
"8c8fae",
"584563",
"3e2137",
"9a6348",
"d79b7d",
"f5edba",
"c0c741",
"647d34",
"e4943a",
"9d303b",
"d26471",
"70377f",
"7ec4c1",
"34859d",
"17434b",
"1f0e1c",
]

filename = "sphere"
width = 32
height = 32

img = Image.open(f"{filename}.png").convert("RGB")
pixels = img.load()

def rgb2hex(r, g, b):
    return '{:02x}{:02x}{:02x}'.format(r, g, b)

with open(f"{filename}.c", "w") as f:
    
    f.write(f"char sprite[{int(width*height/2)}] = {{\n")

    for y in range(height):
        f.write("\t")

        for x in range(0, width, 2):
            color_index_0 = colors.index(rgb2hex(*pixels[(x, y)]))
            color_index_1 = colors.index(rgb2hex(*pixels[(x + 1, y)]))
            _hex = f"0x{hex(color_index_0)[2]}{hex(color_index_1)[2]}"
            f.write(f"{_hex}, ")
            
        f.write("\n")

    f.write(f"}};")

