#Michaela Papallo
#WORK 0: Make a Thing

img = open("image.ppm", "w")
img.write("P3 500 500 255\n")

for i in range(500):
    for j in range(500):
        r = (i * j) % 256
        g = (i + j) % 256
        b = 255
        img.write(str(r) + " " + str(g) + " " + str(b) + " ")
    img.write("\n")

img.close()
