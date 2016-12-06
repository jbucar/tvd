require 'testSystem.testSystem'

-- add test number
assertEquals( persistent.shared.aNumber, nil )
persistent.shared.aNumber = 1
assertEquals( persistent.shared.aNumber, 1, "Expected 1 got " .. tostring(persistent.shared.aNumber) )
persistent.shared.aNumber= nil;

-- add test boolean
assertEquals( persistent.shared.aBool, nil )
persistent.shared.aBool = true
assertEquals( persistent.shared.aBool, true, "Expected true got " .. tostring(persistent.shared.aBool) )
persistent.shared.aBool = nil

-- add test string
assertEquals( persistent.shared.aString, nil )
persistent.shared.aString = "string"
assertEquals( persistent.shared.aString, "string", "Expected 'string' got " .. persistent.shared.aString )
persistent.shared.aString = nil

-- add test nil
assertEquals( persistent.shared.aNilValue, nil )
persistent.shared.aNilValue = nil
assertEquals( persistent.shared.aNilValue, nil, "Expected nil got " .. tostring(persistent.shared.aNilValue) )

-- add test asociative table
assertEquals( persistent.shared.aTable, nil )
table = { apple="green", orange="orange", banana="yellow" }
persistent.shared.aTable = table
assertEquals( type(persistent.shared.aTable), "table", "Expected persistent.shared.aTable to be of type table but is " .. tostring(type(persistent.shared.aTable)) )
assertEquals( persistent.shared.aTable.apple, "green", "Expected green got " .. tostring(persistent.shared.aTable.green) )
assertEquals( persistent.shared.aTable.orange, "orange", "Expected orange got " .. tostring(persistent.shared.aTable.orange) )
assertEquals( persistent.shared.aTable.banana, "yellow", "Expected yellow got " .. tostring(persistent.shared.aTable.yellow) )
persistent.shared.aTable = nil

-- add test simpleTable
-- assertEquals( persistent.shared.aSimpleTable, nil )
-- table2 = { "green", "orange", "yellow" }
-- persistent.shared.aSimpleTable = table2
-- assertEquals( persistent.shared.aSimpleTable, table2 )
