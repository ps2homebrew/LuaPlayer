
-- in main
clockImage = createImage(100, 100)
coClock = coroutine.create(drawClock)
coroutine.resume(coClock, clockImage)

-- in lowser:render
blitAlphaImage(250, 80, clockImage)

-- in global
function drawClock(surface)
	background = createImage(100, 100) 
	clockOfs = 50
	clockWidth = 100 
	clockTextPosition = 85 
	clockBigMarkWidth = 7 
	clockSmallMarkWidth = 3 
	x0 = clockOfs 
	y0 = clockOfs - clockWidth 
	pi = 4*math.atan(1) 
	color = getColorNumber(0, 255, 0) 
	for i=0,60 do 
	   x1 = math.sin(pi-i/60*2*pi) * clockWidth + clockOfs 
	   y1 = math.cos(pi-i/60*2*pi) * clockWidth + clockOfs 
	   drawLine(x0, y0, x1, y1, color, background) 
	   xv = (x1 - clockOfs) / clockWidth 
	   yv = (y1 - clockOfs) / clockWidth 
	   if math.mod(i, 5) == 0 then 
		  xt = xv * clockTextPosition + clockOfs 
		  yt = yv * clockTextPosition + clockOfs 
		  value = math.ceil(i / 5) 
		  if value == 0 then 
			 value = 12 
		  end 
		  printDecimal(xt, yt, value, color, background) 
		  xv = xv * (clockWidth - clockBigMarkWidth) + clockOfs 
		  yv = yv * (clockWidth - clockBigMarkWidth) + clockOfs 
		  drawLine(x1, y1, xv, yv, color, background) 
	   else 
		  xv = xv * (clockWidth - clockSmallMarkWidth) + clockOfs 
		  yv = yv * (clockWidth - clockSmallMarkWidth) + clockOfs 
		  drawLine(x1, y1, xv, yv, color, background) 
	   end 
	   x0 = x1 
	   y0 = y1 
	end 

	while true do 
	   blitImage(0, 0, background, surface) 
	   time = os.time()
	
	   hour = dateFields.hour 
	   if hour > 12 then 
		  hour = hour - 12 
	   end 
	   hour = hour + dateFields.min / 60 + dateFields.sec / 3600 
	   x = math.sin(pi-hour/12*2*pi) * clockWidth / 3 * 2 + clockOfs 
	   y = math.cos(pi-hour/12*2*pi) * clockWidth / 3 * 2 + clockOfs 
	   drawLine(clockOfs, clockOfs, x, y, color) 
	
	   min = dateFields.min + dateFields.sec / 60 
	   x = math.sin(pi-min/60*2*pi) * clockWidth + clockOfs 
	   y = math.cos(pi-min/60*2*pi) * clockWidth + clockOfs 
	   drawLine(clockOfs, clockOfs, x, y, color) 
	
	   x = math.sin(pi-dateFields.sec/60*2*pi) * clockWidth + clockOfs 
	   y = math.cos(pi-dateFields.sec/60*2*pi) * clockWidth + clockOfs 
	   drawLine(clockOfs, clockOfs, x, y, color) 
	   
	   printText(0, 0, os.date("%c", time), color, surface) 
	
	   waitVblankStart(60) 
	end 
end