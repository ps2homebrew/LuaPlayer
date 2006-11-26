import sys

if __name__ == "__main__":
	fname = sys.argv[1]
	f = open(fname)
	buf = "char foo[] = \""
	for char in f.read():
		if char == "\n":
			buf += "\\n\\\n"
		elif char == "\"":
			buf += "\\\""
		else:
			buf += char
	buf +="\";"
	print buf