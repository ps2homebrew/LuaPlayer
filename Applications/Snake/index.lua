--[[
Snake, Copyright (c) 2005 Frank Buss <fb@frank-buss.de> (aka Shine)

   artworks by Gundrosen
   coding by Shine
   background music: "Stranglehold", composed by Jeroen Tel, (C) 1995 Maniacs of Noise
]]

-- default options
options = { speed = 2, level = "desert", music = "on", sound = "on" }

-- possible options for menu
possibleOptions = {
	speed = {
		displayName = "Speed",
		values = { 1, 2, 3 } },
	level = {
		displayName = "Level",
		values = { "grass", "desert" } },
	music = {
		displayName = "Music",
		values = { "on", "off" } },
	sound = {
		displayName = "Sound",
		values = { "on", "off" } }
}

-- load images
backgroundImages = { grass = Image.load("grass.png"), desert = Image.load("desert.png") }
optionsImage = Image.load("options.png")

background = backgroundImages.desert
tiles = Image.load("tiles.png")
snake = Image.createEmpty(480, 272)

scorePosition = { grass = { x = 377, y = 91 }, desert = { x = 374, y = 72 } }
highScorePosition = { grass = { x = 377, y = 138 }, desert = { x = 374, y = 113 } }
scoreColor = { grass = Color.new(0, 0, 0), desert = Color.new(255, 255, 255) }

-- load sounds
explode = Sound.load("explode.wav")
clack = Sound.load("clack.wav")

-- define tile positions
tailR = { x = 0, y = 0 }
tailT = { x = 1, y = 0 }
tailL = { x = 2, y = 0 }
tailB = { x = 3, y = 0 }

headR = { x = 0, y = 1 }
headT = { x = 1, y = 1 }
headL = { x = 2, y = 1 }
headB = { x = 3, y = 1 }

bodyLT = { x = 0, y = 2 }
bodyLB = { x = 1, y = 2 }
bodyRT = { x = 2, y = 2 }
bodyRB = { x = 3, y = 2 }
bodyLR = { x = 4, y = 2 }
bodyTB = { x = 5, y = 2 }

apple = { x = 0, y = 3 }
fly = { x = 1, y = 3 }
scorpion = { x = 2, y = 3 }

-- define globals
tails = {}
heads = {}
bodies = {}

RIGHT = 0
TOP = 1
LEFT = 2
BOTTOM = 3

tails[RIGHT] = tailR
tails[TOP] = tailT
tails[LEFT] = tailL
tails[BOTTOM] = tailB

heads[RIGHT] = headR
heads[TOP] = headT
heads[LEFT] = headL
heads[BOTTOM] = headB

bodies[RIGHT] = bodyLR
bodies[TOP] = bodyTB
bodies[LEFT] = bodyLR
bodies[BOTTOM] = bodyTB

score = 0
high = 0

target = {}
targetImage = 0

dx = 0
dy = 0
cellHead = {}
cellTail = {}

function createRandomTarget()
	target.x = math.random(21)
	target.y = math.random(15)
	if math.random(2) == 1 then
		targetImage = apple
	else
		if options.level == "desert" then
			targetImage = scorpion
		else
			targetImage = fly
		end
	end
end

function clearTileInSnakeImage(x, y)
	snake:fillRect(16*x, 16*y, 16, 16)
end

function drawTileToOffscreen(tile, x, y)
	screen:blit(16*x, 16*y, tiles, 16*tile.x, 16*tile.y, 16, 16)
end

function drawTileToSnakeImage(tile, x, y)
	snake:blit(16*x, 16*y, tiles, 16*tile.x, 16*tile.y, 16, 16)
end	

function newGame()
	snake:clear()
	dx = 0
	dy = 0
	x = 12
	y = 8
	cellTail.x = x
	cellTail.y = y
	cellTail.direction = RIGHT
	x = x+1
	cellHead.x = x
	cellHead.y = y
	cellHead.direction = RIGHT
	cellTail.next = cellHead
	createRandomTarget()
	drawTileToSnakeImage(tailR, cellTail.x, cellTail.y)
	drawTileToSnakeImage(headR, cellHead.x, cellHead.y)

	score = 0
end

function keyboardControl()
	screen.waitVblankStart()
	pad = Controls.read()
	if pad:up() then
		dx = 0
		dy = -1
	elseif pad:down() then
		dx = 0
		dy = 1
	elseif pad:left() then
		dx = -1
		dy = 0
	elseif pad:right() then
		dx = 1
		dy = 0
	elseif pad:circle() then
		screen:save("screenshot.tga")
	end
end

function move()
	-- do nothing, if no movement vector defined
	if dx == 0 and dy == 0 then
		return
	end
	
	-- save last head direction and position
	lastX = cellHead.x
	lastY = cellHead.y
	lastDirection = cellHead.direction

	-- add new head
	cellHead.next = {}
	cellHead = cellHead.next
	cellHead.x = lastX + dx
	cellHead.y = lastY + dy
	
	-- set direction for new head
	direction = 0
	if dx == 1 then
		direction = RIGHT
	elseif dy == -1 then
		direction = TOP
	elseif dx == -1 then
		direction = LEFT
	elseif dy == 1 then
		direction = BOTTOM
	end
	cellHead.direction = direction
	
	-- check which body to draw at the place of the old head
	clearTileInSnakeImage(lastX, lastY)
	if lastDirection == RIGHT and direction == TOP then
		drawTileToSnakeImage(bodyLT, lastX, lastY)
	elseif lastDirection == TOP and direction == LEFT then
		drawTileToSnakeImage(bodyLB, lastX, lastY)
	elseif lastDirection == LEFT and direction == BOTTOM then
		drawTileToSnakeImage(bodyRB, lastX, lastY)
	elseif lastDirection == BOTTOM and direction == RIGHT then
		drawTileToSnakeImage(bodyRT, lastX, lastY)
	elseif lastDirection == RIGHT and direction == BOTTOM then
		drawTileToSnakeImage(bodyLB, lastX, lastY)
	elseif lastDirection == TOP and direction == RIGHT then
		drawTileToSnakeImage(bodyRB, lastX, lastY)
	elseif lastDirection == LEFT and direction == TOP then
		drawTileToSnakeImage(bodyRT, lastX, lastY)
	elseif lastDirection == BOTTOM and direction == LEFT then
		drawTileToSnakeImage(bodyLT, lastX, lastY)
	else
		drawTileToSnakeImage(bodies[direction], lastX, lastY)
	end
	
	-- draw new head
	drawTileToSnakeImage(heads[cellHead.direction], cellHead.x, cellHead.y)
	
	-- check for target
	if cellHead.x == target.x and cellHead.y == target.y then
		createRandomTarget()
		score = score + 1
		if options.sound == "on" then clack:play() end
	else
		-- remove tail
		clearTileInSnakeImage(cellTail.x, cellTail.y)
		cellTail = cellTail.next

		-- draw new tail image
		if cellTail.direction ~= cellTail.next.direction then
			cellTail.direction = cellTail.next.direction
		end
		clearTileInSnakeImage(cellTail.x, cellTail.y)
		drawTileToSnakeImage(tails[cellTail.direction], cellTail.x, cellTail.y)
	end
end

function isGameOver()
	lastX = cellHead.x
	lastY = cellHead.y
	gameOver = false
	if lastX >= 22 then
		gameOver = true
	end
	if lastX < 1 then
		gameOver = true
	end
	if lastY >= 16 then
		gameOver = true
	end
	if lastY < 1 then
		gameOver = true
	end
	cell = cellTail
	while cell ~= cellHead do
		if cell.x == lastX and cell.y == lastY then
			gameOver = true
			break
		end
		cell = cell.next
	end
	return gameOver
end
	
selectedOptionNumber = 0
oldPad = Controls.read()

function showMenu()
	deactiveColor = Color.new(0, 255, 0)
	activeColor = Color.new(255, 255, 255)
	x0 = 80
	y0 = 80
	black = Color.new(0, 0, 0)
	screen:blit(x0, y0, optionsImage)
	screen:print(x0 + 16, y0 + 26, "digital pad: change options", activeColor)
	screen:print(x0 + 16, y0 + 34, "o: save options", activeColor)
	screen:print(x0 + 16, y0 + 42, "x: start game", activeColor)
	y = 0
	selectedOptionValues = nil
	selectedOptionIndex = nil
	selectedOptionName = nil
	for optionKey, possibleOption in pairs(possibleOptions) do
		color = deactiveColor
		if selectedOptionNumber == y then
			color = activeColor
			selectedOptionValues = possibleOption.values
		end
		screen:print(x0 + 16, y * 10 + y0 + 60, possibleOption.displayName, color)
		for i, value in ipairs(possibleOption.values) do
			color = deactiveColor
			if options[optionKey] == value then
				color = activeColor
				if selectedOptionNumber == y then
					selectedOptionIndex = i
					selectedOptionName = optionKey
				end
			end
			screen:print(i * 75 + 70 + 8, y * 10 + y0 + 60, value, color)
		end
		y = y + 1
	end

	pad = Controls.read()
	if pad ~= oldPad then
		if pad:circle() then saveOptions() end
		if pad:cross() then return false end
		if pad:up() then
			selectedOptionNumber = selectedOptionNumber - 1
			if selectedOptionNumber < 0 then selectedOptionNumber = y - 1 end
		end
		if pad:down() then
			selectedOptionNumber = selectedOptionNumber + 1
			if selectedOptionNumber >= y then selectedOptionNumber = 0 end
		end
		len = table.getn(selectedOptionValues)
		if pad:left() then
			selectedOptionIndex = selectedOptionIndex - 1
			if selectedOptionIndex <= 0 then selectedOptionIndex = len end
			options[selectedOptionName] = selectedOptionValues[selectedOptionIndex]
		end
		if pad:right() then
			selectedOptionIndex = selectedOptionIndex + 1
			if selectedOptionIndex > len then selectedOptionIndex = 1 end
			options[selectedOptionName] = selectedOptionValues[selectedOptionIndex]
		end
		oldPad = pad
	end
	background = backgroundImages[options.level]
	if options.music == "on" then
		if not Music.playing() then Music.playFile("stranglehold.xm", true) end
	else
		if Music.playing() then Music.stop() end
	end
	return true
end

optionsFile = "options.txt"

function saveOptions()
	file = io.open(optionsFile, "w")
	if file then
		for key, value in ipairs(options) do
			file:write(key .. "=" .. value .. "\n")
		end
		file:close()
	end
end

function loadOptions()
	file = io.open(optionsFile, "r")
	if file then
		for line in file:lines() do
			equal = string.find(line, "=")
			if equal then
				key = string.sub(line, 1, equal - 1)
				value = string.sub(line, equal + 1)
				if key == "speed" then value = tonumber(value) end
				options[key] = value
			end
		end
		file:close()
	end
end

highscoreFile = "highscore.txt"

function saveHighscore()
	file = io.open(highscoreFile, "w")
	if file then
		file:write(high)
		file:close()
	end
end

function loadHighscore()
	file = io.open(highscoreFile, "r")
	if file then
		high = file:read("*n")
		file:close()
	end
end

-- init
loadOptions()
loadHighscore()
loadedHighscore = high
math.randomseed(os.time())
newGame()

-- game loop
menu = true
while not Controls.read():start() do
	if not menu then
		for i=0,(4 - options.speed) * 2 do
			keyboardControl()
		end
		move()
	end
	screen:blit(0, 0, background, 0, 0, background:width(), background:height(), false)
	screen:blit(0, 0, snake)
	drawTileToOffscreen(targetImage, target.x, target.y)
	if score > high then
		high = score
	end
	screen:print(scorePosition[options.level].x, scorePosition[options.level].y, score, scoreColor[options.level])
	screen:print(highScorePosition[options.level].x, highScorePosition[options.level].y, high, scoreColor[options.level])
	if menu then
		if not showMenu() then
			newGame()
			menu = false
		end
	else
		if isGameOver() then
			if options.sound == "on" then explode:play() end
			screen.waitVblankStart(50)
			menu = true
			if high > loadedHighscore then
				saveHighscore()
				loadedHighscore = high
			end
		end
	end
	screen.waitVblankStart()
	screen.flip()
end
Music.stop()
