--Pad Test
--October 5, 2006 by Dan Cook aka Gamecubesuxs for LuaPlayerPS2

--Color Input
red = Color.new(255, 0, 0) 
blue = Color.new(0, 0, 255)
green = Color.new(0, 255, 0)
white = Color.new(255, 255, 255)

--Table Input
myTable = {"Up Button",  "Down Button",  "Left Button",  "Right Button",  "L1 Button",  "L2 Button",  "L3 Button"}
myTable2 = {"Triangle Button",  "Square Button",  "Circle Button",  "Cross Button",  "R1 Button",  "R2 Button",  "R3 Button"}

--Button Variables
upPressed = myTable[1]
downPressed = myTable[2]
leftPressed = myTable[3]
rightPressed = myTable[4]
LPressed = myTable[5]
L2Pressed = myTable[6]
L3Pressed = myTable[7]
trianglePressed = myTable2[1]
squarePressed = myTable2[2]
circlePressed = myTable2[3]
crossPressed = myTable2[4]
RPressed = myTable2[5]
R2Pressed = myTable2[6]
R3Pressed = myTable2[7]

--Program Loop
while true do
	screen:clear()
	screen:print(0, 5, "Pad Test for LuaPlayerPS2 by Dan Cook aka Gamecubesuxs", white)
	screen:print(0, 15, "Press any button to show input and press START to exit", green)
	screen.waitVblankStart()
	pad = Controls.read()
	if pad:up() then
		screen:print(100, 100, upPressed , red)
		end
	if pad:down() then
		screen:print(100, 100, downPressed , blue)
		end
	if pad:left() then
		screen:print(100, 100, leftPressed , green)
		end
	if pad:right() then
		screen:print(100, 100, rightPressed , white)
		end
	if pad:l() then 
		screen:print(100, 100, LPressed , red)
		end
	if pad:l2() then 
		screen:print(100, 100, L2Pressed , red)
		end
	if pad:l3() then 
		screen:print(100, 100, L3Pressed , red)
		end
	if pad:triangle() then
		screen:print(100, 100, trianglePressed, blue)
		end
	if pad:square() then
		screen:print(100, 100, squarePressed, green)
		end
	if pad:circle() then 
		screen:print(100, 100, circlePressed, white)
		end
	if pad:cross() then
		screen:print(100, 100, crossPressed, red)
		end
	if pad:r() then
		screen:print(100, 100, RPressed, blue)
		end
	if pad:r2() then
		screen:print(100, 100, R2Pressed, blue)
		end
	if pad:r3() then
		screen:print(100, 100, R3Pressed, blue)
		end
	if Controls.read():start() then break end
	screen.waitVblankStart()
	screen.flip()
	end