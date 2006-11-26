--[[
PSP Minesweeper v1.3 by JEK, warpony@gmail.com

Thanks to Shine and Nevyn for making the LUA Player

Note: On levels Extreme and up, time will not show during play because it slows things down at the moment.
]]

-- Loading images
background = Image.load("background.png")
background2 = Image.load("background2.png")
i1 = Image.load("1.png")
i2 = Image.load("2.png")
i3 = Image.load("3.png")
i4 = Image.load("4.png")
i5 = Image.load("5.png")
i6 = Image.load("6.png")
i7 = Image.load("7.png")
i8 = Image.load("8.png")
flag = Image.load("flag.png")
empty = Image.load("empty.png")
blank = Image.load("blank.png") 
mine = Image.load("mine.png")
redmine = Image.load("redmine.png")
select = Image.load("select.png")

--Small versions to use on a large board
i1b = Image.load("1b.png")
i2b = Image.load("2b.png")
i3b = Image.load("3b.png")
i4b = Image.load("4b.png")
i5b = Image.load("5b.png")
i6b = Image.load("6b.png")
i7b = Image.load("7b.png")
i8b = Image.load("8b.png")
flagb = Image.load("flagb.png")
emptyb = Image.load("emptyb.png")
blankb = Image.load("blankb.png") 
mineb = Image.load("mineb.png")
redmineb = Image.load("redmineb.png")
selectb = Image.load("selectb.png")

--Hold "normal" images
temp = {i1, i2, i3, i4, i5, i6, i7, i8, flag, empty, blank, mine, redmine, select}

black = Color.new(0, 0, 0)
white = Color.new(255, 255, 255)

offscr = Image.createEmpty(480,272) 
offscr:clear(black)

update = 0
game_over = false
sizex = 22
sizey = 8
mines = 30
difficulty = 3
x = 1
y = 1
redx = 0
redy = 0

--20 big, 10 small
w = 20
h = 20

--x0 and y80 big, 10 and 90 small
offsetx = 0
offsety = 80

--Default for starting level
clip = 440

start_time = 0
end_time = 0
finished = false
used_flags = 0
used_flags2 = 0
game_running = false


function init(sizey2, sizex2, mines2)
   --Initialize variables
   x = 1
   y = 1
   game_over = false
   start_time = 0
   end_time = 0
   finished = false
   used_flags = 0
   used_flags2 = 0
   game_running = false
   sizex = sizex2
   sizey = sizey2
   mines = mines2

   -- Create array
   bricks = {}
   for i=1,sizey do
      bricks[i] = {}
      for j=1,sizex do
         bricks[i][j] = 0
      end
   end

   flags = {}
   for i=1,sizey do
      flags[i] = {}
      for j=1,sizex do
         flags[i][j] = 0
      end
   end
   
   setMines(sizey, sizex, mines)
   calculateNumbers(sizey, sizex)

end

function toggleDifficulty(difficulty2)
   if difficulty2 == 1 then
      difficulty = 2
      sizex = 16
      sizey = 8
      mines = 20
      clip = 320
   end
   if difficulty2 == 2 then
      difficulty = 3
      sizex = 22
      sizey = 8
      mines = 30
      clip = 440
   end
   if difficulty2 == 3 then
      difficulty = 4
      sizex = 22
      sizey = 8
      mines = 50
   end
   if difficulty2 == 4 then
      difficulty = 5
      sizex = 30
      sizey = 16
      mines = 99
      --Set the variables needed for this size
      w = 10
      h = 10
      offsetx = 10
      offsety = 90
      i1 = i1b
      i2 = i2b
      i3 = i3b
      i4 = i4b
      i5 = i5b
      i6 = i6b
      i7 = i7b
      i8 = i8b
      flag = flagb
      empty = emptyb
      blank = blankb
      mine = mineb
      redmine = redmineb
      select = selectb
   end
   if difficulty2 == 5 then
      difficulty = 6
      sizex = 44
      sizey = 16
      mines = 200
   end
   if difficulty2 == 6 then
      difficulty = 7
      sizex = 44
      sizey = 16
      mines = 300
   end
   if difficulty2 == 7 then
      difficulty = 1
      sizex = 8
      sizey = 8
      mines = 10
      --Set back variables needed for "normal" sizes
      w = 20
      h = 20
      offsetx = 0
      offsety = 80
      i1 = temp[1]
      i2 = temp[2]
      i3 = temp[3]
      i4 = temp[4]
      i5 = temp[5]
      i6 = temp[6]
      i7 = temp[7]
      i8 = temp[8]
      flag = temp[9]
      empty = temp[10]
      blank = temp[11]
      mine = temp[12]
      redmine = temp[13]
      select = temp[14]
      clip = 160
   end
end

function setMines(sizey, sizex, mines)
   --Set random mines
   math.randomseed(os.time())
   fin = 0
   while fin < mines do
      place_x = math.random(sizex)
      place_y = math.random(sizey)
      if bricks[place_y][place_x] == 0 then
         bricks[place_y][place_x] = 9
         fin = fin + 1
      end
   end

end

function cascadeBlanks(x, y, sizey, sizex)
--All
   if(x > 1) and (x < sizex) and (y > 1) and (y < sizey) then
      if bricks[y][x+1] == 0 then
         bricks[y][x+1] = 10
         flags[y][x+1] = 0
         cascadeBlanks(x+1, y, sizey, sizex)
      end
         if bricks[y][x+1] < 9 then
            bricks[y][x+1] = bricks[y][x+1] + 10
         end
      if bricks[y][x-1] == 0 then
         bricks[y][x-1] = 10
         flags[y][x-1] = 0
         cascadeBlanks(x-1, y, sizey, sizex)
      end
         if bricks[y][x-1] < 9 then
            bricks[y][x-1] = bricks[y][x-1] + 10
         end
      if bricks[y+1][x+1] == 0 then
         bricks[y+1][x+1] = 10
         flags[y+1][x+1] = 0
         cascadeBlanks(x+1, y+1, sizey, sizex)
      end
         if bricks[y+1][x+1] < 9 then
            bricks[y+1][x+1] = bricks[y+1][x+1] + 10
         end
      if bricks[y+1][x-1] == 0 then
         bricks[y+1][x-1] = 10
         flags[y+1][x-1] = 0
         cascadeBlanks(x-1, y+1, sizey, sizex)
      end
         if bricks[y+1][x-1] < 9 then
            bricks[y+1][x-1] = bricks[y+1][x-1] + 10
         end
      if bricks[y-1][x+1] == 0 then
         bricks[y-1][x+1] = 10
         flags[y-1][x+1] = 0
         cascadeBlanks(x+1, y-1, sizey, sizex)
      end
         if bricks[y-1][x+1] < 9 then
            bricks[y-1][x+1] = bricks[y-1][x+1] + 10
         end
      if bricks[y-1][x-1] == 0 then
         bricks[y-1][x-1] = 10
         flags[y-1][x-1] = 0
         cascadeBlanks(x-1, y-1, sizey, sizex)
      end
         if bricks[y-1][x-1] < 9 then
            bricks[y-1][x-1] = bricks[y-1][x-1] + 10
         end
      if bricks[y-1][x] == 0 then
         bricks[y-1][x] = 10
         flags[y-1][x] = 0
         cascadeBlanks(x, y-1, sizey, sizex)
      end
         if bricks[y-1][x] < 9 then
            bricks[y-1][x] = bricks[y-1][x] + 10
         end
      if bricks[y+1][x] == 0 then
         bricks[y+1][x] = 10
         flags[y+1][x] = 0
         cascadeBlanks(x, y+1, sizey, sizex)
      end
         if bricks[y+1][x] < 9 then
            bricks[y+1][x] = bricks[y+1][x] + 10
         end
   end

--x=1, y=1
   if(x == 1) and (y == 1) then
      if bricks[y][x+1] == 0 then
         bricks[y][x+1] = 10
         flags[y][x+1] = 0
         cascadeBlanks(x+1, y, sizey, sizex)
      end
         if bricks[y][x+1] < 9 then
            bricks[y][x+1] = bricks[y][x+1] + 10
         end
      if bricks[y+1][x+1] == 0 then
         bricks[y+1][x+1] = 10
         flags[y+1][x+1] = 0
         cascadeBlanks(x+1, y+1, sizey, sizex)
      end
         if bricks[y+1][x+1] < 9 then
            bricks[y+1][x+1] = bricks[y+1][x+1] + 10
         end
      if bricks[y+1][x] == 0 then
         bricks[y+1][x] = 10
         flags[y+1][x] = 0
         cascadeBlanks(x, y+1, sizey, sizex)
      end
         if bricks[y+1][x] < 9 then
            bricks[y+1][x] = bricks[y+1][x] + 10
         end
   end

--x=sizex, y=sizey
   if (x == sizex) and (y == sizey) then
      if bricks[y][x-1] == 0 then
         bricks[y][x-1] = 10
         flags[y][x-1] = 0
         cascadeBlanks(x-1, y, sizey, sizex)
      end
         if bricks[y][x-1] < 9 then
            bricks[y][x-1] = bricks[y][x-1] + 10
         end
      if bricks[y-1][x-1] == 0 then
         bricks[y-1][x-1] = 10
         flags[y-1][x-1] = 0
         cascadeBlanks(x-1, y-1, sizey, sizex)
      end
         if bricks[y-1][x-1] < 9 then
            bricks[y-1][x-1] = bricks[y-1][x-1] + 10
         end
      if bricks[y-1][x] == 0 then
         bricks[y-1][x] = 10
         flags[y-1][x] = 0
         cascadeBlanks(x, y-1, sizey, sizex)
      end
         if bricks[y-1][x] < 9 then
            bricks[y-1][x] = bricks[y-1][x] + 10
         end
   end
   
--x=1, y=sizey   
   if(x == 1) and (y == sizey) then
      if bricks[y][x+1] == 0 then
         bricks[y][x+1] = 10
         flags[y][x+1] = 0
         cascadeBlanks(x+1, y, sizey, sizex)
      end
         if bricks[y][x+1] < 9 then
            bricks[y][x+1] = bricks[y][x+1] + 10
         end
      if bricks[y-1][x+1] == 0 then
         bricks[y-1][x+1] = 10
         flags[y-1][x+1] = 0
         cascadeBlanks(x+1, y-1, sizey, sizex)
      end
         if bricks[y-1][x+1] < 9 then
            bricks[y-1][x+1] = bricks[y-1][x+1] + 10
         end
      if bricks[y-1][x] == 0 then
         bricks[y-1][x] = 10
         flags[y-1][x] = 0
         cascadeBlanks(x, y-1, sizey, sizex)
      end
         if bricks[y-1][x] < 9 then
            bricks[y-1][x] = bricks[y-1][x] + 10
         end
   end

--x=sizex, y=1   
   if (x == sizex) and (y == 1) then
      if bricks[y][x-1] == 0 then
         bricks[y][x-1] = 10
         flags[y][x-1] = 0
         cascadeBlanks(x-1, y, sizey, sizex)
      end
         if bricks[y][x-1] < 9 then
            bricks[y][x-1] = bricks[y][x-1] + 10
         end
      if bricks[y+1][x-1] == 0 then
         bricks[y+1][x-1] = 10
         flags[y+1][x-1] = 0
         cascadeBlanks(x-1, y+1, sizey, sizex)
      end
         if bricks[y+1][x-1] < 9 then
            bricks[y+1][x-1] = bricks[y+1][x-1] + 10
         end
      if bricks[y+1][x] == 0 then
         bricks[y+1][x] = 10
         flags[y+1][x] = 0
         cascadeBlanks(x, y+1, sizey, sizex)
      end
         if bricks[y+1][x] < 9 then
            bricks[y+1][x] = bricks[y+1][x] + 10
         end
   end
   
--x>1, x<sizex, y==1   
   if(x > 1) and (x < sizex) and (y == 1) then
      if bricks[y][x+1] == 0 then
         bricks[y][x+1] = 10
         flags[y][x+1] = 0
         cascadeBlanks(x+1, y, sizey, sizex)
      end
         if bricks[y][x+1] < 9 then
            bricks[y][x+1] = bricks[y][x+1] + 10
         end
      if bricks[y][x-1] == 0 then
         bricks[y][x-1] = 10
         flags[y][x-1] = 0
         cascadeBlanks(x-1, y, sizey, sizex)
      end
         if bricks[y][x-1] < 9 then
            bricks[y][x-1] = bricks[y][x-1] + 10
         end
      if bricks[y+1][x+1] == 0 then
         bricks[y+1][x+1] = 10
         flags[y+1][x+1] = 0
         cascadeBlanks(x+1, y+1, sizey, sizex)
      end
         if bricks[y+1][x+1] < 9 then
            bricks[y+1][x+1] = bricks[y+1][x+1] + 10
         end
      if bricks[y+1][x-1] == 0 then
         bricks[y+1][x-1] = 10
         flags[y+1][x-1] = 0
         cascadeBlanks(x-1, y+1, sizey, sizex)
      end
         if bricks[y+1][x-1] < 9 then
            bricks[y+1][x-1] = bricks[y+1][x-1] + 10
         end
      if bricks[y+1][x] == 0 then
         bricks[y+1][x] = 10
         flags[y+1][x] = 0
         cascadeBlanks(x, y+1, sizey, sizex)
      end
         if bricks[y+1][x] < 9 then
            bricks[y+1][x] = bricks[y+1][x] + 10
         end
   end
   
--x==1, y > 1, y < sizey   
   if(x == 1) and (y > 1) and (y < sizey) then
      if bricks[y][x+1] == 0 then
         bricks[y][x+1] = 10
         flags[y][x+1] = 0
         cascadeBlanks(x+1, y, sizey, sizex)
      end
         if bricks[y][x+1] < 9 then
            bricks[y][x+1] = bricks[y][x+1] + 10
         end
      if bricks[y+1][x+1] == 0 then
         bricks[y+1][x+1] = 10
         flags[y+1][x+1] = 0
         cascadeBlanks(x+1, y+1, sizey, sizex)
      end
         if bricks[y+1][x+1] < 9 then
            bricks[y+1][x+1] = bricks[y+1][x+1] + 10
         end
      if bricks[y-1][x+1] == 0 then
         bricks[y-1][x+1] = 10
         flags[y-1][x+1] = 0
         cascadeBlanks(x+1, y-1, sizey, sizex)
      end
         if bricks[y-1][x+1] < 9 then
            bricks[y-1][x+1] = bricks[y-1][x+1] + 10
         end
      if bricks[y-1][x] == 0 then
         bricks[y-1][x] = 10
         flags[y-1][x] = 0
         cascadeBlanks(x, y-1, sizey, sizex)
      end
         if bricks[y-1][x] < 9 then
            bricks[y-1][x] = bricks[y-1][x] + 10
         end
      if bricks[y+1][x] == 0 then
         bricks[y+1][x] = 10
         flags[y+1][x] = 0
         cascadeBlanks(x, y+1, sizey, sizex)
      end
         if bricks[y+1][x] < 9 then
            bricks[y+1][x] = bricks[y+1][x] + 10
         end
   end

--x=sizex, y>1, y<sizey
   if (x == sizex) and (y > 1) and (y < sizey) then
      if bricks[y][x-1] == 0 then
         bricks[y][x-1] = 10
         flags[y][x-1] = 0
         cascadeBlanks(x-1, y, sizey, sizex)
      end
         if bricks[y][x-1] < 9 then
            bricks[y][x-1] = bricks[y][x-1] + 10
         end
      if bricks[y+1][x-1] == 0 then
         bricks[y+1][x-1] = 10
         flags[y+1][x-1] = 0
         cascadeBlanks(x-1, y+1, sizey, sizex)
      end
         if bricks[y+1][x-1] < 9 then
            bricks[y+1][x-1] = bricks[y+1][x-1] + 10
         end
      if bricks[y-1][x-1] == 0 then
         bricks[y-1][x-1] = 10
         flags[y-1][x-1] = 0
         cascadeBlanks(x-1, y-1, sizey, sizex)
      end
         if bricks[y-1][x-1] < 9 then
            bricks[y-1][x-1] = bricks[y-1][x-1] + 10
         end
      if bricks[y-1][x] == 0 then
         bricks[y-1][x] = 10
         flags[y-1][x] = 0
         cascadeBlanks(x, y-1, sizey, sizex)
      end
         if bricks[y-1][x] < 9 then
            bricks[y-1][x] = bricks[y-1][x] + 10
         end
      if bricks[y+1][x] == 0 then
         bricks[y+1][x] = 10
         flags[y+1][x] = 0
         cascadeBlanks(x, y+1, sizey, sizex)
      end
         if bricks[y+1][x] < 9 then
            bricks[y+1][x] = bricks[y+1][x] + 10
         end
   end

--x>1 x<sizex, y==sizey
   if(x > 1) and (x < sizex) and (y == sizey) then
      if bricks[y][x+1] == 0 then
         bricks[y][x+1] = 10
         flags[y][x+1] = 0
         cascadeBlanks(x+1, y, sizey, sizex)
      end
         if bricks[y][x+1] < 9 then
            bricks[y][x+1] = bricks[y][x+1] + 10
         end
      if bricks[y][x-1] == 0 then
         bricks[y][x-1] = 10
         flags[y][x-1] = 0
         cascadeBlanks(x-1, y, sizey, sizex)
      end
         if bricks[y][x-1] < 9 then
            bricks[y][x-1] = bricks[y][x-1] + 10
         end
      if bricks[y-1][x+1] == 0 then
         bricks[y-1][x+1] = 10
         flags[y-1][x+1] = 0
         cascadeBlanks(x+1, y-1, sizey, sizex)
      end
         if bricks[y-1][x+1] < 9 then
            bricks[y-1][x+1] = bricks[y-1][x+1] + 10
         end
      if bricks[y-1][x-1] == 0 then
         bricks[y-1][x-1] = 10
         flags[y-1][x-1] = 0
         cascadeBlanks(x-1, y-1, sizey, sizex)
      end
         if bricks[y-1][x-1] < 9 then
            bricks[y-1][x-1] = bricks[y-1][x-1] + 10
         end
      if bricks[y-1][x] == 0 then
         bricks[y-1][x] = 10
         flags[y-1][x] = 0
         cascadeBlanks(x, y-1, sizey, sizex)
      end
         if bricks[y-1][x] < 9 then
            bricks[y-1][x] = bricks[y-1][x] + 10
         end
   end

end

function calculateNumbers(sizey, sizex)
   --Set numbers
   --Upper left
   if bricks[1][2] == 9 then
      if bricks[1][1] ~=9 then
         bricks[1][1] = bricks[1][1] + 1
      end
   end
   if bricks[2][1] == 9 then
      if bricks[1][1] ~=9 then
         bricks[1][1] = bricks[1][1] + 1
      end
   end
   if bricks[2][2] == 9 then
      if bricks[1][1] ~=9 then
         bricks[1][1] = bricks[1][1] + 1
      end
   end

   --Lower right
   if bricks[sizey-1][sizex] == 9 then
      if bricks[sizey][sizex] ~=9 then
         bricks[sizey][sizex] = bricks[sizey][sizex] + 1
      end
   end
   if bricks[sizey][sizex-1] == 9 then
      if bricks[sizey][sizex] ~=9 then
         bricks[sizey][sizex] = bricks[sizey][sizex] + 1
      end
   end
   if bricks[sizey-1][sizex-1] == 9 then
      if bricks[sizey][sizex] ~=9 then
         bricks[sizey][sizex] = bricks[sizey][sizex] + 1
      end
   end

   --Lower left
   if bricks[sizey][2] == 9 then
      if bricks[sizey][1] ~=9 then
         bricks[sizey][1] = bricks[sizey][1] + 1
      end
   end
   if bricks[sizey-1][2] == 9 then
      if bricks[sizey][1] ~=9 then
         bricks[sizey][1] = bricks[sizey][1] + 1
      end
   end
   if bricks[sizey-1][1] == 9 then
      if bricks[sizey][1] ~=9 then
         bricks[sizey][1] = bricks[sizey][1] + 1
      end
   end

   --Upper right
   if bricks[2][sizex] == 9 then
      if bricks[1][sizex] ~=9 then
         bricks[1][sizex] = bricks[1][sizex] + 1
      end
   end
   if bricks[2][sizex-1] == 9 then
      if bricks[1][sizex] ~=9 then
         bricks[1][sizex] = bricks[1][sizex] + 1
      end
   end
   if bricks[1][sizex-1] == 9 then
      if bricks[1][sizex] ~=9 then
         bricks[1][sizex] = bricks[1][sizex] + 1
      end
   end

   --top border
   for j=2,sizex-1 do
      if bricks[2][j] == 9 then
         if bricks[1][j] ~=9 then
            bricks[1][j] = bricks[1][j] + 1
         end
      end
      if bricks[2][j-1] == 9 then
         if bricks[1][j] ~=9 then
            bricks[1][j] = bricks[1][j] + 1
         end
      end
      if bricks[2][j+1] == 9 then
         if bricks[1][j] ~=9 then
            bricks[1][j] = bricks[1][j] + 1
         end
      end
      if bricks[1][j-1] == 9 then
         if bricks[1][j] ~=9 then
            bricks[1][j] = bricks[1][j] + 1
         end
      end
      if bricks[1][j+1] == 9 then
         if bricks[1][j] ~=9 then
            bricks[1][j] = bricks[1][j] + 1
         end
      end
   end

   --left border
   for j=2,sizey-1 do
      if bricks[j][2] == 9 then
         if bricks[j][1] ~=9 then
            bricks[j][1] = bricks[j][1] + 1
         end
      end
      if bricks[j-1][2] == 9 then
         if bricks[j][1] ~=9 then
            bricks[j][1] = bricks[j][1] + 1
         end
      end
      if bricks[j+1][2] == 9 then
         if bricks[j][1] ~=9 then
            bricks[j][1] = bricks[j][1] + 1
         end
      end
      if bricks[j-1][1] == 9 then
         if bricks[j][1] ~=9 then
            bricks[j][1] = bricks[j][1] + 1
         end
      end
      if bricks[j+1][1] == 9 then
         if bricks[j][1] ~=9 then
            bricks[j][1] = bricks[j][1] + 1
         end
      end
   end

   --bottom border
   for j=2,sizex-1 do
      if bricks[sizey-1][j] == 9 then
         if bricks[sizey][j] ~=9 then
            bricks[sizey][j] = bricks[sizey][j] + 1
         end
      end
      if bricks[sizey-1][j-1] == 9 then
         if bricks[sizey][j] ~=9 then
            bricks[sizey][j] = bricks[sizey][j] + 1
         end
      end
      if bricks[sizey-1][j+1] == 9 then
         if bricks[sizey][j] ~=9 then
            bricks[sizey][j] = bricks[sizey][j] + 1
         end
      end
      if bricks[sizey][j-1] == 9 then
         if bricks[sizey][j] ~=9 then
            bricks[sizey][j] = bricks[sizey][j] + 1
         end
      end
      if bricks[sizey][j+1] == 9 then
         if bricks[sizey][j] ~=9 then
            bricks[sizey][j] = bricks[sizey][j] + 1
         end
      end
   end

   --right border
   for j=2,sizey-1 do
      if bricks[j][sizex-1] == 9 then
         if bricks[j][sizex] ~=9 then
            bricks[j][sizex] = bricks[j][sizex] + 1
         end
      end
      if bricks[j-1][sizex-1] == 9 then
         if bricks[j][sizex] ~=9 then
            bricks[j][sizex] = bricks[j][sizex] + 1
         end
      end
      if bricks[j+1][sizex-1] == 9 then
         if bricks[j][sizex] ~=9 then
            bricks[j][sizex] = bricks[j][sizex] + 1
         end
      end
      if bricks[j-1][sizex] == 9 then
         if bricks[j][sizex] ~=9 then
            bricks[j][sizex] = bricks[j][sizex] + 1
         end
      end
      if bricks[j+1][sizex] == 9 then
         if bricks[j][sizex] ~=9 then
            bricks[j][sizex] = bricks[j][sizex] + 1
         end
      end
   end

   --All the other non border squares
   for ty=2,sizey-1 do
      for tx=2, sizex-1 do
         --down
         if bricks[ty-1][tx] == 9 then
            if bricks[ty][tx] ~=9 then
               bricks[ty][tx] = bricks[ty][tx] + 1
            end
         end
         --up
         if bricks[ty+1][tx] == 9 then
            if bricks[ty][tx] ~=9 then
               bricks[ty][tx] = bricks[ty][tx] + 1
            end
         end
         --left
         if bricks[ty][tx-1] == 9 then
            if bricks[ty][tx] ~=9 then
               bricks[ty][tx] = bricks[ty][tx] + 1
            end
         end
         --right
         if bricks[ty][tx+1] == 9 then
            if bricks[ty][tx] ~=9 then
               bricks[ty][tx] = bricks[ty][tx] + 1
            end
         end
         --down left
         if bricks[ty-1][tx-1] == 9 then
            if bricks[ty][tx] ~=9 then
               bricks[ty][tx] = bricks[ty][tx] + 1
            end
         end
         --up left
         if bricks[ty+1][tx-1] == 9 then
            if bricks[ty][tx] ~=9 then
               bricks[ty][tx] = bricks[ty][tx] + 1
            end
         end
         --down right
         if bricks[ty-1][tx+1] == 9 then
            if bricks[ty][tx] ~=9 then
               bricks[ty][tx] = bricks[ty][tx] + 1
            end
         end
         --up right
         if bricks[ty+1][tx+1] == 9 then
            if bricks[ty][tx] ~=9 then
               bricks[ty][tx] = bricks[ty][tx] + 1
            end
         end
      end
   end
   --End Set numbers
end

-- Prepare game for starting
init(sizey, sizex, mines)


oldPad = Controls.read()

while true do
	
	-- Read controls
	pad = Controls.read()
	if pad ~= oldPad then
		if pad:triangle() then
            game_running = true
            if start_time == 0 then
               start_time = os.time()
            end
			if (flags[y][x] == 0) and (bricks[y][x] < 10) then
			   flags[y][x] = 1
			else
			   flags[y][x] = 0
			end
		end
		if pad:left() then
			x = x - 1
			if x == 0 then
				x = sizex
			end
		end
		if pad:right() then
			x = x + 1
			if x > sizex then
				x = 1
			end
		end
		if pad:up() then
			y = y - 1
			if y == 0 then
				y = sizey
			end
		end
		if pad:down() then
			y = y + 1
			if y > sizey then
				y = 1
			end
		end
		if pad:cross() then
         game_running = true
         if start_time == 0 then
            start_time = os.time()
         end
         if (game_over == false) and (finished == false) then
           if (bricks[y][x] ~= 10) and (bricks[y][x] == 0) then
              bricks[y][x] = bricks[y][x] + 10
              flags[y][x] = 0
              cascadeBlanks(x, y, sizey, sizex)
           end
           if (bricks[y][x] ~= 10) and (bricks[y][x] < 9) then
              bricks[y][x] = bricks[y][x] + 10
              flags[y][x] = 0
           end
           if bricks[y][x] == 9 then
              bricks[y][x] = 19
              game_over = true
           end
         end
		end
		if pad:square() then
			init(sizey, sizex, mines)
		end
		if pad:circle() then
		    --Finds next difficulty
		    toggleDifficulty(difficulty)
			init(sizey, sizex, mines)
		end
		if pad:start() then
		   break
		end
		--Need some speed on bigger levels!
		if difficulty < 5 then
		   oldPad = pad
		end
	end
	
	
	screen:clear()
	offscr:clear(black)

   finished = true
  
   --Draw background image
   screen:blit(0, 0, background, 0, 0, background:width(), background:height(), false)

	-- Draw board
    for i=1,sizey do
       for j=1,sizex do
			x0 = j * w+offsetx
			y0 = i * h+offsety
			
			--DRAW IMAGES
            --Draw blank tiles
            if bricks[i][j] < 11 then
 		       offscr:blit(x0, y0, blank)
 		    end
			
			if game_over == true then
			   if bricks[i][j] == 9 then
			      offscr:blit(x0, y0, mine)
			   end
			end
			--If numbers have been clicked, show them
			if bricks[i][j] > 9 then
               if bricks[i][j] == 10 then
 		          offscr:blit(x0, y0, empty)
               end
               if bricks[i][j] == 11 then
 		          offscr:blit(x0, y0, i1)
               end
               if bricks[i][j] == 12 then
 		          offscr:blit(x0, y0, i2)
               end
               if bricks[i][j] == 13 then
 		          offscr:blit(x0, y0, i3)
               end
               if bricks[i][j] == 14 then
 		          offscr:blit(x0, y0, i4)
               end
               if bricks[i][j] == 15 then
 		          offscr:blit(x0, y0, i5)
               end
               if bricks[i][j] == 16 then
 		          offscr:blit(x0, y0, i6)
               end
               if bricks[i][j] == 17 then
 		          offscr:blit(x0, y0, i7)
               end
               if bricks[i][j] == 18 then
 		          offscr:blit(x0, y0, i8)
               end
               if bricks[i][j] == 19 then
   		          offscr:blit(x0, y0, redmine)
		       end
			end
            if (flags[i][j] == 1) and (bricks[i][j] < 10) then
               offscr:blit(j*w+offsetx, i*h+offsety, flag)
            end
            if j == x and i == y then
 		        offscr:blit(x0, y0, select)
			end
 		    
            if bricks[i][j] < 9 then
               finished = false
 	        end

		end
	end

   --Draw "buffer" to screen
   screen:blit(20, 100, offscr, 20, 100, clip, 160, false)

    for f=1,sizey do
       for ff=1,sizex do
          if (flags[f][ff] == 1) and (bricks[f][ff] < 10) then
             used_flags = used_flags + 1
          end
       end
    end

   used_flags2 = used_flags
   used_flags = 0

    if finished == true then
      screen:blit(127, 128, background2, 0, 0, background2:width(), background2:height(), false)
      if end_time == 0 then
         end_time = os.time()
      end
      screen:print(170, 180, "Time: ", black)
 	  screen:print(230, 180, os.difftime(end_time, start_time), black)
      screen:print(250, 180, " seconds", black)

   end

      --Time info
      if (finished ~= true) and (difficulty < 5) then
         end_time = os.time()
      end
      if (game_running == true) and (game_over == false) and (difficulty < 5) then
         screen:print(70, 79, os.difftime(end_time, start_time), white)
      end
      screen:print(20, 79, "Time: ", white)
      screen:print(90, 79, " seconds", white)

      --Mines info
      screen:print(170, 79, "Mines: ", white)
      screen:print(220, 79, mines, white)

      --Flags info
      screen:print(250, 79, "Flags: ", white)
      screen:print(300, 79, used_flags2, white)

      --Difficulty info
      screen:print(330, 79, "Level: ", white)
      if difficulty == 1 then
         screen:print(380, 79, "Easy", white)
	  end
      if difficulty == 2 then
         screen:print(380, 79, "Medium", white)
	  end
      if difficulty == 3 then
         screen:print(380, 79, "Difficult", white)
	  end
      if difficulty == 4 then
         screen:print(380, 79, "Hard", white)
	  end
      if difficulty == 5 then
         screen:print(380, 79, "Expert", white)
	  end
      if difficulty == 6 then
         screen:print(380, 79, "Extreme", white)
	  end
      if difficulty == 7 then
         screen:print(380, 79, "Ultimate", white)
	  end
	
    screen.waitVblankStart()
    screen.flip()
end
