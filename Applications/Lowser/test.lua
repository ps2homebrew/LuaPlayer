a = "hejsan"
b = "yaah"
if a < b then
	print("Yeah.")
end

print("-----")
qu = { z = 1 }

function qu:foo(a)
	print("yeah: "..a)
end
function qu_foo(a)
	qu:foo(a)
end

functions = { qu_foo }

functions[table.getn(functions)]("gargh!antuan")


print("-----")


LowserView = {  
	currentIndex = 1,
	status = "Welcome to Lowser! (c) nCoder 2005, ncoder.nevyn.nu",
	res = {
		genericFolderIcon = "yeah"
	},
	colors = {
		bg = 1,
		text = 2
	},
	contents = {},
	cwd = ""
}

function LowserView:new() -- This is just a copypaste of Shines code...
   c = {} 
   setmetatable(c, self) 
   self.__index = self 
   
   --for i, v in ipairs(self) do
   
   
   return c
end

function LowserView:cd(newdir)
	self.currentIndex = 1
	self.cwd = newdir
end

foo = LowserView:new()
foo:cd("kuoo")

bar = LowserView:new()
bar:cd("kaah")

print(foo.cwd)
print(bar.cwd)

