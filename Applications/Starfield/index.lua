size = 200
zMax = 5
speed = 0.1

width = 480
height = 272

starfield = {}
math.randomseed(os.time())

function createStar(i)
	starfield[i] = {}
	starfield[i].x = math.random(2*width) - width
	starfield[i].y = math.random(2*height) - height
	starfield[i].z = zMax
end

for i = 1, size do
	createStar(i)
	starfield[i].z = math.random(zMax)
end

white = Color.new(255, 255, 255)
black = Color.new(0, 0, 0)

while true do
	screen:clear(black)
	for i = 1, size do
		starfield[i].z = starfield[i].z - speed
		if starfield[i].z < speed then createStar(i) end
		x = width / 2 + starfield[i].x / starfield[i].z
		y = height / 2 + starfield[i].y / starfield[i].z
		if x < 0 or y < 0 or x >= width or y >= height then
			createStar(i)
		else
			screen:pixel(x, y, white)
		end
	end
	screen:print(272, 264, "Starfield for PSP by Shine", white)
	screen.waitVblankStart()
	screen.flip()
	if Controls.read():start() then break end
end
