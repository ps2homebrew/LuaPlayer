--[[
Regression and speed tests.

Usage: write a function, which returns the time and some string, which
are used for compare the functionality of this function againts a reference
version of Lua Player and add the function name to the "tests" array.

Results: PNGs are created for the tests and the reference.txt, which
can be copied into this script, when started from a working Lua Player
version. Errors are displayed in red at the end of the test. If it is
mor than 10% slower than the reference, it is counted as an error.

Looks like sometimes (very rare) the alpha blitting fails, but would be
changed with the pspgl implementation anyway.
]]

System.usbDiskModeActivate()

function profileStart()
	theTimer = Timer.new()
	theTimer:start()
end

function profile()
	return theTimer:time()
end

function createTestImage(destiantion, width, height)
	math.randomseed(0)
	for x = 0, width - 1 do
		for y = 0, height - 1 do
			destiantion:pixel(x, y, Color.new(math.random(0, 255), math.random(0, 255), math.random(0, 255)))
		end
	end
end

function md5ForFile(filename)
	local file = io.open(filename, "r")
	if file then
		local md5sum = System.md5sum(file:read("*a"))
		file:close()
		return md5sum
	end
	return -1
end

function testSmallImage(pngName)
	width = 31
	height = 43
	profileStart()
	for i = 1, 100 do
		image = Image.createEmpty(width, height)
		createTestImage(image, width, height)
	end
	time = profile()
	image:save(pngName)
	return time, md5ForFile(pngName)
end

function testFullScreenPixelPlot(pngName)
	profileStart()
	createTestImage(screen, 480, 272)
	time = profile()
	screen.waitVblankStart()
	screen.flip()
	screen:save(pngName)
	return time, md5ForFile(pngName)
end

function testTransparencyImage(pngName)
	profileStart()
	width = 31
	height = 43
	image = Image.createEmpty(width, height)
	createTestImage(image, width, height)
	image:fillRect(10, 10, 10, 10)
	i2 = Image.createEmpty(100, 100)
	i2:clear(red)
	for i = 1, 2000 do
		i2:blit(5, 7, image)
		i2:blit(50, 9, image, 0, 0, image:width(), image:height(), false)
	end
	time = profile()
	i2:save(pngName)
	return time, md5ForFile(pngName)
end

function testTransparencyScreen(pngName)
	profileStart()
	width = 31
	height = 43
	image = Image.createEmpty(width, height)
	createTestImage(image, width, height)
	image:fillRect(10, 10, 10, 10)
	screen:clear(red)
	for i = 1, 1000 do
		screen:blit(5, 7, image)
		screen:blit(71, 9, image, 0, 0, image:width(), image:height(), false)
	end
	time = profile()
	screen.waitVblankStart()
	screen.flip()
	screen:save(pngName)
	return time, md5ForFile(pngName)
end

function testClippingImage(pngName)
	profileStart()
	width = 31
	height = 43
	image = Image.createEmpty(width, height)
	createTestImage(image, width, height)
	i2 = Image.createEmpty(100, 100)
	i2:clear(red)
	for i = 1, 1000 do 
		i2:blit(5, 7, image)
		i2:blit(-5, 7, image)
		i2:blit(-5, -7, image)
		i2:blit(5, -7, image)
		i2:blit(80, -7, image)
		i2:blit(80, 7, image)
		i2:blit(50, 80, image)
		i2:blit(80, 80, image)
		i2:blit(30, 30, image, 2, 3, 10, 10)
		i2:blit(-5, 93, image, 2, 3, 10, 10)
	end
	time = profile()
	i2:save(pngName)
	return time, md5ForFile(pngName)
end

function testClippingScreen(pngName)
	profileStart()
	width = 31
	height = 43
	image = Image.createEmpty(width, height)
	createTestImage(image, width, height)
	screen:clear(red)
	for i = 1, 1000 do 
		screen:blit(5, 7, image)
		screen:blit(-5, 7, image)
		screen:blit(-5, -7, image)
		screen:blit(5, -7, image)
		screen:blit(470, -7, image)
		screen:blit(470, 7, image)
		screen:blit(260, 470, image)
		screen:blit(260, 470, image)
		screen:blit(130, 30, image, 2, 3, 10, 10)
		screen:blit(-5, 265, image, 2, 3, 10, 10)
	end
	time = profile()
	screen.waitVblankStart()
	screen.flip()
	screen:save(pngName)
	return time, md5ForFile(pngName)
end

function testLine(target, pngName)
	target:clear()
	profileStart()
	for c = 0, 1000 do
		for i = 0, 20 do
			x0 = i/20*479
			y1 = 271-i/20*271
			target:drawLine(x0, 271, 479, y1, green)
		end
	end
	time = profile()
	screen.waitVblankStart()
	screen.flip()
	target:save(pngName)
	return time, md5ForFile(pngName)
end

function testLineImage(pngName)
	return testLine(Image.createEmpty(480, 272), pngName)
end

function testLineScreen(pngName)
	return testLine(screen, pngName)
end

function testText(target, pngName)
	target:clear()
	profileStart()
	for c = 0, 10000 do
		target:print(11, 13, "Hello", red)
		target:print(11, 130, "Hello", green)
	end
	time = profile()
	screen.waitVblankStart()
	screen.flip()
	target:save(pngName)
	return time, md5ForFile(pngName)
end

function testTextImage(pngName)
	return testText(Image.createEmpty(480, 272), pngName)
end

function testTextScreen(pngName)
	return testText(screen, pngName)
end

function testBlitSpeedAlpha(source, target, pngName)
	source:clear()
	target:clear()
	createTestImage(source, 480, 272)
	profileStart()
	for c = 0, 100 do
		target:blit(0, 0, source)
	end
	time = profile()
	screen.waitVblankStart()
	screen.flip()
	target:save(pngName)
	return time, md5ForFile(pngName)
end

function testBlitSpeedAlphaImage(pngName)
	return testBlitSpeedAlpha(Image.createEmpty(480, 272), Image.createEmpty(480, 272), pngName)
end

function testBlitSpeedAlphaScreen(pngName)
	return testBlitSpeedAlpha(Image.createEmpty(480, 272), screen, pngName)
end

function testBlitSpeedCopy(source, target, pngName)
	source:clear()
	target:clear()
	createTestImage(source, 480, 272)
	profileStart()
	for c = 0, 1000 do
		target:blit(0, 0, source, 0, 0, 480, 272, false)
	end
	time = profile()
	screen.waitVblankStart()
	screen.flip()
	target:save(pngName)
	return time, md5ForFile(pngName)
end

function testBlitSpeedCopyImage(pngName)
	return testBlitSpeedCopy(Image.createEmpty(480, 272), Image.createEmpty(480, 272), pngName)
end

function testBlitSpeedCopyScreen(pngName)
	return testBlitSpeedCopy(Image.createEmpty(480, 272), screen, pngName)
end

--[[
the old 5551 timings:

	{ name="testSmallImage", time=8131, result="10d7fcf1f0a4c5c94984542e526dca9b" },
	{ name="testFullScreenPixelPlot", time=7178, result="5361a7ccb80fc2f861c878a55bd1bb1c" },
	{ name="testTransparencyImage", time=508, result="24814c6078ac85280d39fc9ae2ca1e95" },
	{ name="testTransparencyScreen", time=443, result="1b0d0d3a508e40565870cdf2d63182c1" },
	{ name="testClippingImage", time=734, result="f3b0612cf7f817144a8a367b3dc7777f" },
	{ name="testClippingScreen", time=1492, result="46849356d3a32a55ac461c62c96ab3e8" },
	{ name="testLineImage", time=2011, result="771f552f193347f3b2587833e5d72e97" },
	{ name="testLineScreen", time=660, result="90a154ffd897c6879087cf3e44f71994" },
	{ name="testTextImage", time=720, result="60da8160e4c9315529da05490c27b9f9" },
	{ name="testTextScreen", time=596, result="9098e15f373da5b19eaa8ef93dbea872" },
	{ name="testBlitSpeedAlphaImage", time=1020, result="858b57847ffc5d527203f98ea98ed8e2" },
	{ name="testBlitSpeedAlphaScreen", time=474, result="5361a7ccb80fc2f861c878a55bd1bb1c" },
	{ name="testBlitSpeedCopyImage", time=8382, result="858b57847ffc5d527203f98ea98ed8e2" },
	{ name="testBlitSpeedCopyScreen", time=1694, result="5361a7ccb80fc2f861c878a55bd1bb1c" },
	
testBlitSpeedAlphaScreen and testBlitSpeedCopyScreen needs to be faster for the 8888 mode
]]

tests = {
	{ name="testSmallImage", time=8757, result="01d42086a28ec1cf03c551ce75ddd30a" },
	{ name="testFullScreenPixelPlot", time=7760, result="b24f32a46df7088f08587d51e7071bd0" },
	{ name="testTransparencyImage", time=647, result="9b6cad02f04c0cb4942f5b602ba13357" },
	{ name="testTransparencyScreen", time=577, result="595c0b4d67c4f15142daf639d4ad4461" },
	{ name="testClippingImage", time=954, result="71fc06f295443cba12db8bdcd85ad078" },
	{ name="testClippingScreen", time=1787, result="d9a719f406a5f8f50e6a1c66758c081d" },
	{ name="testLineImage", time=2228, result="995675cd3da15e1918255a5aa6b53aae" },
	{ name="testLineScreen", time=737, result="213678f024aa302c7431a4c991fc355e" },
	{ name="testTextImage", time=807, result="7a0baad1a6a22afa622f05d749f7fffa" },
	{ name="testTextScreen", time=588, result="7716950c9b560863494f51b542dbd854" },
	{ name="testBlitSpeedAlphaImage", time=1332, result="5d917a000187d605ba4d07d4ff32bda3" },
	{ name="testBlitSpeedAlphaScreen", time=955, result="b24f32a46df7088f08587d51e7071bd0" },
	{ name="testBlitSpeedCopyImage", time=11198, result="5d917a000187d605ba4d07d4ff32bda3" },
	{ name="testBlitSpeedCopyScreen", time=3415, result="b24f32a46df7088f08587d51e7071bd0" },
}

textY = 0

green = Color.new(0, 255, 0)
red = Color.new(255, 0, 0)

reference = io.open("reference.txt", "w")
for _, test in ipairs(tests) do
	test.measuredTime, test.measuredResult = _G[test.name](test.name .. ".png")
	reference:write("\t{ name=\"" .. test.name
		.. "\", time=" .. test.measuredTime
		.. ", result=\"" .. test.measuredResult .. "\" },\n")
end
reference:close()

screen:clear()
for _, test in ipairs(tests) do
	if test.result ~= test.measuredResult then
		result = "not ok"
		color = red
	else
		result = "ok"
		color = green
	end
	delta = test.measuredTime - test.time
	if delta > test.time / 10 then color = red end
	screen:print(0, textY, test.name .. ": time delta: " .. delta .. ", result: " .. result, color)
	textY = textY + 8
end	

screen.flip()

while true do
	screen.waitVblankStart()
	if Controls.read():start() then break end
end
