from PIL import Image

colors = {
    (0, 0, 0):      0,  # transparent
    (8, 8, 8):      1,  # black
    (41, 41, 41):   2,  # dark gray
    (74, 66, 66):   3,  # gray
    (181, 0, 0):    4,  # dark red
    (255, 0, 0):    5,  # red
    (0, 33, 173):   6,  # blue
    (198, 140, 16): 7,  # yellow
    (132, 49, 41):  8,  # skin shadow
    (189, 99, 74):  9,  # skin
}

filename = "mario-32"
width = 32
height = 32

img = Image.open(f"{filename}.png").convert("RGB")
pixels = img.load()

with open(f"{filename}.c", "w") as f:
    
    f.write(f"char sprite[{int(width*height/2)}] = {{\n")

    for y in range(height):
        f.write("\t")

        for x in range(0, width, 2):
            color_index_0 = colors[pixels[(x, y)]]
            color_index_1 = colors[pixels[(x + 1, y)]]
            _hex = f"0x{hex(color_index_0)[2]}{hex(color_index_1)[2]}"
            f.write(f"{_hex}, ")
            
        f.write("\n")

    f.write(f"}};")

