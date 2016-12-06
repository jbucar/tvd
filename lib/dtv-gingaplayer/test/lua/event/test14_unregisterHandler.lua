-- Tests unregistering an unregistered handler

require 'testSystem.testSystem'

function unregisteredHandler(evt)
end

event.unregister(unregisteredHandler)

assertTrue(true)