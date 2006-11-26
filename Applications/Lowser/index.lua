--[[
Lowser, Copyright (c) 2005 Joachim Bengtsson <joachimb@gmail.com> (aka Nevyn)
http://ncoder.nevyn.nu/

VERSION 0010 (v0.15)
]]

LowserView = {  
	currentIndex = 1,
	drawStartIndex = 1,
	status = "Welcome to Lowser! (c) nCoder 2005, ncoder.nevyn.nu",
	res = {
		genericFolderIcon = Image.load("img/folder.png"),
		genericAppIcon = Image.load("img/app.png"),
		genericIcon = Image.load("img/plain.png"),
		pspAppIcon = genericAppIcon,
		arrowIcon = Image.load("img/arrow.png"),
		
		backgroundImage = Image.load("img/bg.png")
	},
	colors = {
		bg = Color.new(255,255,255),
		text = Color.new(0,0,64)
	},
	contents = {},
	cwd = ""
}


function LowserView:new()
   c = {} 
   setmetatable(c, self) 
   self.__index = self 
   return c
end

function LowserView:renderIconsForCurrentDirectory()

	self.contents = System.listDirectory()
	self.cwd = System.currentDirectory()
	
	table.sort(self.contents, function (a, b) return a.name < b.name end)

	done = false
	while not done do -- filter out unwanted files
		for idx,file in ipairs(self.contents) do
			if string.sub(file.name,1,1) == "." and not string.find(file.name, "icon.png") then
				table.remove(self.contents, idx)
				break
			end
			if idx == table.getn(self.contents) then
				done = true
			end
		end
	end

	for idx, file in ipairs(self.contents) do
		-- defaults
		file.icon = nil
		file.render = nil
		file.ftype = "none"


		-- set icon
		if file.directory then
			-- traverse and assign correct icon and type
			System.currentDirectory(file.name)
			subdirconts = System.listDirectory()
			
			for subidx, subfile in ipairs(subdirconts) do
				if string.lower(subfile.name) == "index.lua" then
					file.icon = self.res.genericAppIcon
					file.ftype = "appdir"
				end
			end
			for subidx, subfile in ipairs(subdirconts) do
				if string.lower(subfile.name) == "icon.png" then
					file.icon = Image.load(subfile.name)
				end
			end
			System.currentDirectory(self.cwd)
			
			if file.icon == nil then
				file.icon = self.res.genericFolderIcon
			end
			if file.ftype == "none" then
				file.ftype = "dir"
			end
		else -- is a file
			-- DEFINE AND SET FILE TYPES
			if string.lower(string.sub(file.name, -4)) == ".lua" then
				file.icon = self.res.genericAppIcon
				file.ftype = "app"
			elseif string.lower(string.sub(file.name, -4)) == ".PBP" then
				file.icon = self.res.pspAppIcon
				file.ftype = "pbp"
			else
				file.icon = self.res.genericIcon
			end
		end -- setting icon
		
		-- render
		
		file.render = Image.createEmpty(192, 32)
		file.render:clear(self.colors.bg)
		
		file.render:blit(0,0,file.icon)
		
		file.render:print(32+4, 32/2-8/2, file.name, self.colors.text)
		
	end -- loop through dir()
end -- function call

function LowserView:cd(newdir)
	self.currentIndex = 1
	self.drawStartIndex = 1
	System.currentDirectory(newdir)
	self.cwd = System.currentDirectory()
	self:renderIconsForCurrentDirectory() -- (hopefully expected) side effect.
end
function LowserView:render()
	local x = 14
	local y = 8
	local dirName = string.gsub(string.sub(self.cwd,5), "/", " > ")
	
	screen:blit(0,0,self.res.backgroundImage)
	
	screen:print(x,y, dirName, self.colors.text)
	y = y + 8 + 14

	for i=self.drawStartIndex, math.min(self.drawStartIndex + (272-12)/36 -2, table.getn(self.contents)) do
		curfile = self.contents[i]
		if i == self.currentIndex then
			screen:blit(x,y+32/2-16/2,self.res.arrowIcon)
		end
		screen:blit(x+16+4, y, curfile.render)
		
		y = y + 32 + 4
	end
	
	screen:print(10,272-12, self.status, self.colors.text)
	
	
	screen.flip()
end

function LowserView:action(entry)
	if entry.ftype == "dir" then
		self:cd(entry.name)
		self:render()
		
	elseif entry.ftype == "appdir" then
		self.status = "Running bundle index "..entry.name.."/index.lua".."..."
		System.currentDirectory(entry.name)
		dofile("index.lua")
		screen.waitVblankStart(10)
		System.currentDirectory("..")
		self:render()
		
	elseif entry.ftype == "app" then
		lowser.status = "Loading script "..entry.name.."..."
		lowser:render()
		self.status = "Running script "..entry.name.."..."
		dofile(entry.name)
		screen.waitVblankStart(10)
		self:render()
	else
		self.status = "Can't do anything with this file, sorry."
		self:render()
	end
end
function LowserView:filemenu(entry)
	self:action(entry) -- placeholder
end

function LowserView:event(e)
	keyin = e.value
	if keyin:up() then
		if self.currentIndex > 1 then
			self.currentIndex = self.currentIndex - 1
			self.drawStartIndex = math.min(self.currentIndex, self.drawStartIndex)
		end
		self:render(contents)
	elseif keyin:down() then
		if self.currentIndex < table.getn(self.contents) then
			self.currentIndex = self.currentIndex + 1
			if (self.currentIndex - self.drawStartIndex) > ((272-12)/36 -2) then
				self.drawStartIndex = self.drawStartIndex + 1
			end
		end
		self:render(contents)
	elseif keyin:cross() then
		self:filemenu(lowser.contents[self.currentIndex])
	elseif keyin:right() then
		if self.contents[self.currentIndex].ftype == "dir" then
			self:action(self.contents[self.currentIndex])
		else
			self:filemenu(self.contents[self.currentIndex])
		end
	elseif keyin:left() or keyin:circle() then
		self:cd("..")
		self:render()
		
--	elseif keyin:l() then
--		isDiskMode = true
--		screen.waitVblankStart(40)
--		while isDiskMode do
--			System.usbDiskModeActivate()
--			screen:clear(Color.new(20, 126, 180))
--			screen:print(screen:width()-screen:width()/2-130, screen:height()/2-24, "Disk mode active. Press L to exit.", Color.new(219, 219, 250))
--			datef = os.date("*t", os.time())
--			f = function(s) 
--				if s < 10 then
--					return "0"..s
--				else return s end
--			end
--			screen:print(screen:width()-70, 5, f(datef.hour)..":"..f(datef.min)..":"..f(datef.sec))
--			diskmodekey = Controls.read()
--			isDiskMode = not diskmodekey:l()
--			screen.waitVblankStart()
--			screen:flip()
--		end
--		System.usbDiskModeDeactivate()
--		self:render()
--		screen.waitVblankStart(40)
	end

end


Lontroller = { 
	responders = {}
}

function Lontroller:new() -- This is just a copypaste of Shines code...
   c = {} 
   setmetatable(c, self) 
   self.__index = self 
   return c
end

function Lontroller:addResponder(responder)
	table.insert(self.responders, responder)
end

function Lontroller:dispatchEvent(e)
	self.responders[table.getn(self.responders)](e)
end






-- ================================
-- 			   Main app
-- ================================
lowser = LowserView:new() 
lowser:cd(appsDir)
lowser:render()

controller = Lontroller:new()
controller:addResponder(function (e) lowser.status = "No responder! message:"..e.value; lowser:render(); end)
controller:addResponder(function (e) lowser:event(e); end)


local lastkeyin = Controls.read()
while true do
	keyin = Controls.read()
	if keyin ~= lastkeyin then
		event = { type="key", value=keyin }
		controller:dispatchEvent(event)
		
		if keyin:start() then
			screen.waitVblankStart(40)
			if Controls.read():start() then
				lowser.status = "Bye!"
				lowser:render()
				break
			end
		end
		if keyin:triangle() then
			screen:save(appsDir.."/../".."lowserShot.png")
		end
	end
	lastkeyin = keyin
	screen.waitVblankStart()
end



