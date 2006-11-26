function rotate(image)
	local w = image:width()
	local h = image:height()
	local result = Image.createEmpty(h, w)
	for x=0,w-1 do
		for y=0,h-1 do
			result:pixel(h-y-1, x, image:pixel(x, y))
		end
	end
	return result
end

function printRotated(x, y, text, color, image, rotateIndex)
	rotateIndex = math.mod(rotateIndex, 4)
	local w = string.len(text)
	local result = Image.createEmpty(w * 8, 8)
	result:print(0, 0, text, color)
	if rotateIndex > 0 then
		rotateIndex = rotateIndex - 1
		for i=0,rotateIndex do
			result = rotate(result)
		end
	end
	image:blit(x, y, result)
end

cadetBlue = Color.new(95, 158, 160)

printRotated(132, 0, "Don't ask what Lua Player", cadetBlue, screen, 0)
printRotated(472, 76, "can do for you,", cadetBlue, screen, 1)
printRotated(160, 264, "ask what you can do", cadetBlue, screen, 2)
printRotated(0, 76, "for Lua Player!", cadetBlue, screen, 3)
screen.flip()
while true do
	screen.waitVblankStart()
end
