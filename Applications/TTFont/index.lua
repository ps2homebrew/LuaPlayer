green = Color.new(0, 255, 0)
yellow = Color.new(255, 255, 0)
cyan = Color.new(0, 255, 255)
monoSpaced = Font.createMonoSpaced()
proportional = Font.createProportional()
proportionalBig = Font.createProportional()
proportional:setPixelSizes(0, 32)
proportionalBig:setPixelSizes(0, 64)
monoSpaced:setPixelSizes(0, 14)
t = 0
pi = math.atan(1) * 4
while true do
	screen:clear()
	x = math.cos(pi / 16 * t) * 30
	y = math.sin(pi / 16 * t) * 80
	t = t + 1
	if t == 63 then
		t = 0
	end
	screen:fontPrint(proportional, 10, 60, "Hello", green)
	screen:fontPrint(proportionalBig, 40 + x, 140 + y, "Lua Player", yellow)
	screen:fontPrint(monoSpaced, 10, 200, "Above is the proportional font and this is the mono-spaced", cyan)
	screen:fontPrint(monoSpaced, 10, 214, "font, where every character has the same width", cyan)
	if Controls.read():start() then break end
	screen.waitVblankStart()
	screen.flip()
end
