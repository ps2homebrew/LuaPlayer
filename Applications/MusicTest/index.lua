status1 = "Press X before any of arrow or L or R."
status2 = "none"

tcol = Color.new(0,0,64)
function render()
		screen:print(4,4,status1, tcol)
		screen:print(4,14,status2, tcol)
		screen:print(4,24,"Pan: "..pan, tcol)
		screen:print(4,34,"Vol: "..vol, tcol)
		screen:print(4,44,"Freq: "..freq, tcol)
		screen:print(4,54,"newest voice: "..tostring(voice), tcol)
end

voice = 0

Music.volume(96) 
SoundSystem.SFXVolume(128) -- max 
SoundSystem.reverb(4) -- out of 15 
SoundSystem.panoramicSeparation(128) -- 0 is mono

pan, vol, freq = 128, 255, 22000

Music.playFile("music.xm", true)

local sound = Sound.load("sound.wav") -- MUST be mono!

local pApp = Image.load("bg.png")

local pScroll = Image.createEmpty(pApp:width()*2, pApp:height()*2)
pScroll:blit(0,0,pApp)
pScroll:blit(pApp:width(), 0, pApp)
pScroll:blit(0, pApp:height(), pApp)
pScroll:blit(pApp:width(), pApp:height(), pApp)
scrollx = 0
scrolly = 0
screen:blit(0,0,pApp)

local lastkeyin = Controls.read()
while true do
	local keyin = Controls.read()
	
	scrollx = scrollx + (keyin:analogX())/10
	scrolly = scrolly + (keyin:analogY())/10
	
	if scrollx > pApp:width() then scrollx = 0 end
	if scrollx < 0 then scrollx = pApp:width() + scrollx end
	
	if scrolly > pApp:width() then scrolly = 0 end
	if scrolly < 0 then scrolly = pApp:width() + scrolly end
	
	for y=0, screen:height()/pApp:height() do
		for x=0, screen:width()/pApp:width() do
			screen:blit(x*pApp:width(), y*pApp:height(),pScroll,scrollx, scrolly, pApp:width(), pApp:height(), true)
		end
	end
	
	
	
	
	if Music.playing() then
		status1 = "Music file playing"
	else
		status1 = "Music file not playing"
	end

	if keyin ~= lastkeyin then
	
		if keyin:circle() then
			Music.playFile("music.xm", true)
		elseif keyin:triangle() then
			Music.stop()
						
		elseif keyin:cross() then
			status2 = "Playing soundfile "..tostring(sound)
			voice = sound:play()
			voice:pan(pan)
			voice:volume(vol)
			voice:frequency(freq)
		elseif keyin:square() then
			voice:stop()
		end
		
		if keyin:select() then
			if Music.playing() then
				Music.pause()
			else
				Music.resume()
			end
		end
	end
	
	if keyin:l() then
		pan = pan - 1
		if pan < 0 then
			pan = 0
		end
		voice:pan(pan)
		


	elseif keyin:r() then
		pan = pan + 1
		if pan > 255 then
			pan = 255
		end
		voice:pan(pan)

	end
	
	if keyin:down() then
		vol = vol - 10
		if vol < 0 then
			vol = 0
		end
		voice:volume(vol)

	elseif keyin:up() then
		vol = vol + 10
		if vol > 255 then
			vol = 255
		end
		voice:volume(vol)
	end
	
	if keyin:left() then
		freq = freq - 500
		if freq < 0 then
			freq = 0
		end
		voice:frequency(freq)
	elseif keyin:right() then
		freq = freq + 500
		if freq > 88000 then
			freq = 88000
		end
		voice:frequency(freq)
	end
	
	if keyin:start() then
		Music.stop()
		break
	end

	
	
	lastkeyin = keyin
	
	render()
	screen.flip()
	screen.waitVblankStart()
end

