-- Testea que #measureText funcione con caracteres especiales
require 'testSystem.testSystem'

local THRESHOLD = 3
local texto1 = "ñÑáÁéÉíÍóÓúÚ"
local texto2 = "nNaAeEiIoOuU"

canvas:attrFont("Tiresias",10)
local w1,h1 = canvas:measureText(texto1)
local w2,h2 = canvas:measureText(texto2)

assertTrue( math.abs(w1-w2) <= THRESHOLD )
assertTrue( math.abs(h1-h2) <= THRESHOLD )
