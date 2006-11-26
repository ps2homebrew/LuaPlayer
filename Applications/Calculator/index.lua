keys = {
	{ "C", "sin", "cos", "tan" },
	{ "1/x", "x^2", "sqr", "/" },
	{ "7", "8", "9", "*" },
	{ "4", "5", "6", "-" },
	{ "1", "2", "3", "+" },
	{ "0", "+/-", ".", "=" } }

color = Color.new(0, 255, 0)

function drawRect(x0, y0, w, h)
	screen:drawLine(x0, y0, x0+w, y0, color)
	screen:drawLine(x0, y0, x0, y0+h, color)
	screen:drawLine(x0+w, y0, x0+w, y0+h, color)
	screen:drawLine(x0+w, y0+h, x0, y0+h, color)
end

oldPad = Controls.read()
x = 1
y = 1
text = "0"
lastNumber = 0
deleteOnKey = true
lastOperation = ""
value = ""
number = 0

while true do
	pad = Controls.read()
	if pad ~= oldPad then
		if pad:triangle() then
			screen:save("calculator.tga")
		end
		if pad:left() then
			x = x - 1
			if x == 0 then
				x = 4
			end
		end
		if pad:right() then
			x = x + 1
			if x == 5 then
				x = 1
			end
		end
		if pad:up() then
			y = y - 1
			if y == 0 then
				y = 6
			end
		end
		if pad:down() then
			y = y + 1
			if y == 7 then
				y = 1
			end
		end
		if pad:cross() then
			value = keys[y][x]
			number = tonumber(text)
			if value == "C" then
				text = "0"
				lastNumber = 0
				lastOperation = ""
				deleteOnKey = true
			elseif value == "1/x" then
				text = tostring(1/number)
			elseif value == "sin" then
				text = tostring(math.sin(number))
			elseif value == "cos" then
				text = tostring(math.cos(number))
			elseif value == "tan" then
				text = tostring(math.tan(number))
			elseif value == "x^2" then
				text = tostring(number*number)
			elseif value == "sqr" then
				text = tostring(math.sqrt(number))
			elseif value == "/" or value == "*" or value == "-" or value == "+" or value == "=" then
				if lastOperation == "/" then
					text = tostring(lastNumber / number)
				elseif lastOperation == "*" then
					text = tostring(lastNumber * number)
				elseif lastOperation == "-" then
					text = tostring(lastNumber - number)
				elseif lastOperation == "+" then
					text = tostring(lastNumber + number)
				end
				number = tonumber(text)
				if value == "=" then
					text = tostring(number)
					lastOperation = ""
				else
					lastNumber = number
					lastOperation = value
				end
				deleteOnKey = true
			elseif value == "+/-" then
				text = tostring(-number)
			else
				if deleteOnKey then
					text = value
					deleteOnKey = false
				else
					text = text .. value
				end
			end
		end
		if pad:start() then
			break
		end
		oldPad = pad
	end
	
	screen:clear()
	yk = 0
	w = 40
	h = 30
	drawRect(w, 0, w * 4, h-2)
	screen:print(w+4, 4, text, color)
	for yindex,line in ipairs(keys) do
		for xindex,key in ipairs(line) do
			x0 = xindex * w
			y0 = yindex * h
			drawRect(x0, y0, w, h)
			if xindex == x and yindex == y then
				screen:fillRect(x0, y0, w, h, color)
				foreground = Color.new(0, 0, 0)
			else
				foreground = color
			end
			screen:print(x0 + 5, y0 + 5, key, foreground)
		end
	end
	screen.waitVblankStart()
	screen.flip()
end
