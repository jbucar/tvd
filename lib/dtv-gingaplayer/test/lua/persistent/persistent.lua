require 'testSystem.testSystem'
TType = "table"

assertEquals( type(persistent), TType, "Expected persistent to be of type table" )

-- add test
assertThrow( function () persistent.myVar = 1 end )
assertEquals( persistent.myVar, nil )

-- test persistent.shared
assertEquals( type(persistent.shared), TType, "Expected persistent.shared to be of type table" )
assertThrow( function () persistent.shared = 1 end )
assertEquals( type(persistent.shared), TType, "Expected persistent.shared to be of type table" )

-- test persistent.channel
assertEquals( type(persistent.channel), TType, "Expected persistent.channel to be of type table" )
assertThrow( function () persistent.channel = 1 end )
assertEquals( type(persistent.channel), TType, "Expected persistent.channel to be of type table" )

-- test persistent.service
assertEquals( type(persistent.service), TType, "Expected persistent.service to be of type table" )
assertThrow( function () persistent.service = 1 end )
assertEquals( type(persistent.service), TType, "Expected persistent.service to be of type table" )
