require 'testSystem.testSystem'

-- add test number
assertEquals( persistent.channel.aNumber, nil )
persistent.channel.aNumber = 1
assertEquals( persistent.channel.aNumber, 1, "Expected 1 got " .. tostring(persistent.channel.aNumber) )
persistent.channel.aNumber = nil

-- add test boolean
assertEquals( persistent.channel.aBool, nil )
persistent.channel.aBool = true
assertEquals( persistent.channel.aBool, true, "Expected true got " .. tostring(persistent.channel.aBool) )
persistent.channel.aBool = nil

-- add test string
assertEquals( persistent.channel.aString, nil )
persistent.channel.aString = "string"
assertEquals( persistent.channel.aString, "string", "Expected 'string' got " .. persistent.channel.aString )
persistent.channel.aString = nil

-- add test nil
assertEquals( persistent.channel.aNilValue, nil )
persistent.channel.aNilValue = nil
assertEquals( persistent.channel.aNilValue, nil, "Expected nil got " .. tostring(persistent.channel.aNilValue) )

-- add test asociative table
assertEquals( persistent.channel.aTable, nil )
table = { apple="green", orange="orange", banana="yellow" }
persistent.channel.aTable = table
assertEquals( type(persistent.channel.aTable), "table", "Expected persistent.channel.aTable to be of type table but is " .. tostring(type(persistent.channel.aTable)) )
assertEquals( persistent.channel.aTable.apple, "green", "Expected green got " .. tostring(persistent.channel.aTable.green) )
assertEquals( persistent.channel.aTable.orange, "orange", "Expected orange got " .. tostring(persistent.channel.aTable.orange) )
assertEquals( persistent.channel.aTable.banana, "yellow", "Expected yellow got " .. tostring(persistent.channel.aTable.yellow) )
persistent.channel.aTable = nil

-- add test simpleTable
-- assertEquals( persistent.channel.aSimpleTable, nil )
-- table2 = { "green", "orange", "yellow" }
-- persistent.channel.aSimpleTable = table2
-- assertEquals( persistent.channel.aSimpleTable, table2 )