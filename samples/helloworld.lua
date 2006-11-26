green = Color.new(0, 255, 0)

screen:print(200, 100, "Hello World!", green)

for i=0,20 do
	x0 = i/20*479
	y1 = 271-i/20*271
	screen:drawLine(x0, 271, 479, y1, green)
end

screen.flip()
while true do
	screen.waitVblankStart()
end
