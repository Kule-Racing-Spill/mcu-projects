from PIL import Image

colors = [
"000000",
"283f61",
"89b2a0",
"170e9f",
"0202f0",
"00bef8",
"353439",
"c15a1d",
"d4b7f1",
"356349",
"16c54c",
"19e16d",
"bad3a9",
"98eafe",
"f4ff00",
"101010",
]


for i in range(10):
    filename = f"number{i+1}"

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

