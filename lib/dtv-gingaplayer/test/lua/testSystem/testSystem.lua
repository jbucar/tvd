local keepTesting = true

function showResult()
	canvas:drawRect("fill",0,0,720,576)
	canvas:flush()
end

function dbgInfo( errorMsg )
	str = "[lua::TestSystem] Assertion failed at line "
	if debug ~= nil then
		str = str .. debug.getinfo(3).currentline
	end
	if errorMsg ~= nil then
		str = str .. ": " .. errorMsg
	end
	return str
end

function test( value, dbgStr )
	if keepTesting then
		if not value then
			print(dbgStr)
			color = "red"
			keepTesting = false
		else
			color = "green"
		end
		canvas:attrColor(color)
		showResult()
	end
	return keepTesting
end

function assertEquals( value, anotherValue, errorMsg )
	return test( value == anotherValue, dbgInfo(errorMsg) )
end

function assertTrue( value, errorMsg )
	return test(value, dbgInfo(errorMsg))
end

function assertFalse( value, errorMsg )
	return test(not value, dbgInfo(errorMsg))
end

function assertThrow( fnc, errorMsg )
	return test(not pcall( fnc ), dbgInfo(errorMsg))
end

function assertNoThrow( fnc, errorMsg )
	return test(pcall( fnc ), dbgInfo(errorMsg))
end
