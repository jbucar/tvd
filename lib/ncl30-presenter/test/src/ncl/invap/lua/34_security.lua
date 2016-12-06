local dx, dy = canvas:attrSize() 

local y = 10
canvas:attrColor ('white')
canvas:drawRect('fill', 0, 0, dx, dy)
canvas:attrColor('white')
canvas:attrFont('vera', 20, 'bold')

function printFail(text)
	y = y + 30
	canvas:attrColor('red')
	canvas:drawText(10, y, text)
	canvas:flush()
end

function printOk(text)
	y = y + 30
	canvas:attrColor('green')
	canvas:drawText(10, y, text)
	canvas:flush()
end

--os.clock

print("os.clock(): Return CPU time since Lua started in seconds.")
if os.clock == nil then printOk("os.clock desactivado"); else printFail("os.clock esta activado"); end

--os.execute
print("os.execute([command]): Execute an operating system shell command. This is like the C system() function. The system dependent status code is returned. ")
if os.execute == nil then printOk("os.execute desactivado"); else printFail("os.execute esta activado"); end

--os.exit
print("os.exit([code]): Calls the C function exit, with an optional code, to terminate the host program. The default value for code is the success code.")
if os.exit == nil then printOk("os.exit desactivado"); else printFail("os.exit esta activado"); end

--os.getenv
print("os.getenv(varname): Returns the value of the process environment variable varname, or nil if the variable is not defined.")
if os.getenv == nil then printOk("os.getenv desactivado"); else printFail("os.getenv esta activado"); end

--os.remove
print("os.remove(filename): Deletes the file with the given name. If this function fails, it returns nil, plus a string describing the error.")
if os.remove == nil then printOk("os.remove desactivado"); else printFail("os.remove esta activado"); end

--os.rename
print("os.rename(oldname, newname): Renames file named oldname to newname. If this function fails, it returns nil, plus a string describing the error.")
if os.rename == nil then printOk("os.rename desactivado"); else printFail("os.rename esta activado"); end

--os.tmpname ()
print("os.tmpname: Generate a name that can be used for a temporary file. This only generates a name, it does not open a file.")
if os.tmpname == nil then printOk("os.tmpname desactivado"); else printFail("os.tmpname esta activado"); end

--os.setlocale()
print("os.setlocale(locale [, category])")
if os.setlocale == nil then printOk("os.setlocale desactivado"); else printFail("os.setlocale esta activado"); end

--[[Fin OS]]

--io.open
print("io")
if io == nil then printOk("io desactivado"); else printFail("io esta activado"); end

--debug
if debug == nil then printOk("debug desactivado"); else printFail("debug esta activo"); end


