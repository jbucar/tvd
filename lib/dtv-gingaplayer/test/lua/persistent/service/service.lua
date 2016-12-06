require 'testSystem.testSystem'

-- add test number
assertEquals( persistent.service.aNumber, nil )
persistent.service.aNumber = 1
assertEquals( persistent.service.aNumber, 1, "Expected 1 got " .. tostring(persistent.service.aNumber) )
persistent.service.aNumber = nil

-- add test boolean
assertEquals( persistent.service.aBool, nil )
persistent.service.aBool = true
assertEquals( persistent.service.aBool, true, "Expected true got " .. tostring(persistent.service.aBool) )
persistent.service.aBool = nil

-- add test string
assertEquals( persistent.service.aString, nil )
persistent.service.aString = "string"
assertEquals( persistent.service.aString, "string", "Expected 'string' got " .. persistent.service.aString )
persistent.service.aString = nil

-- add test nil
assertEquals( persistent.service.aNilValue, nil )
persistent.service.aNilValue = nil
assertEquals( persistent.service.aNilValue, nil, "Expected nil got " .. tostring(persistent.service.aNilValue) )

-- add test asociative table
assertEquals( persistent.service.aTable, nil )
table = { apple="green", orange="orange", banana="yellow" }
persistent.service.aTable = table
assertEquals( type(persistent.service.aTable), "table", "Expected persistent.service.aTable to be of type table but is " .. tostring(type(persistent.service.aTable)) )
assertEquals( persistent.service.aTable.apple, "green", "Expected green got " .. tostring(persistent.service.aTable.green) )
assertEquals( persistent.service.aTable.orange, "orange", "Expected orange got " .. tostring(persistent.service.aTable.orange) )
assertEquals( persistent.service.aTable.banana, "yellow", "Expected yellow got " .. tostring(persistent.service.aTable.yellow) )
persistent.service.aTable = nil

-- add test simpleTable
-- assertEquals( persistent.service.aSimpleTable, nil )
-- table2 = { "green", "orange", "yellow" }
-- persistent.service.aSimpleTable = table2
-- assertEquals( persistent.service.aSimpleTable, table2 )