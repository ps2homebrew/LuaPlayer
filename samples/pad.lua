-- A very simple pad test script
print("Lua pad test program by evilo", "Press buttons for testing")

while true do
	screen.waitVblankStart()

	pad = Controls.read()

	if pad:circle() then 
	  print("circle pressed")
	end
	if pad:cross() then 
	  print("cross pressed")
	end
	if pad:triangle() then 
	  print("triangle pressed")
	end
	if pad:square() then 
	  print("square pressed")
	end
	if pad:select() then 
	  print("select pressed")
	end
	if pad:start() then 
	  print("start pressed")
	end
	if pad:up() then 
	  print("up pressed")
	end
	if pad:down() then 
	  print("down pressed")
	end
	if pad:left() then 
	  print("left pressed")
	end
	if pad:right() then 
	  print("right pressed")
	end
	if pad:l1() then 
	  print("l1 pressed")
	end
	if pad:r1() then 
	  print("r1 pressed")
	end
	if pad:l2() then
	  print("l2 pressed")
	end
        if pad:r2() then
          print("r2 pressed")
        end
	if pad:l3() then
	  print("l3 pressed")
	end
	if pad:r3() then
	  print("r3 pressed")
	end
end
