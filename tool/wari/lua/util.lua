-- /*******************************************************************************

--   Copyright (c) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata
--   All rights reserved.

-- ********************************************************************************

--   Redistribution and use in source and binary forms, with or without 
--   modification, are permitted provided that the following conditions are met:
--     * Redistributions of source code must retain the above copyright
--       notice, this list of conditions and the following disclaimer.
--     * Redistributions in binary form must reproduce the above copyright
--       notice, this list of conditions and the following disclaimer in the
--       documentation and/or other materials provided with the distribution.
--     * Neither the name of the author nor the names of its contributors may
--       be used to endorse or promote products derived from this software
--       without specific prior written permission.

--   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
--   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
--   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
--   DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
--   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
--   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
--   OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
--   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
--   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
--   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

-- ********************************************************************************

--   La redistribución y el uso en las formas de código fuente y binario, con o sin
--   modificaciones, están permitidos siempre que se cumplan las siguientes condiciones:
--     * Las redistribuciones del código fuente deben conservar el aviso de copyright
--       anterior, esta lista de condiciones y el siguiente descargo de responsabilidad.
--     * Las redistribuciones en formato binario deben reproducir el aviso de copyright
--       anterior, esta lista de condiciones y el siguiente descargo de responsabilidad
--       en la documentación y/u otros materiales suministrados con la distribución.
--     * Ni el nombre de los titulares de derechos de autor ni los nombres de sus
--       colaboradores pueden usarse para apoyar o promocionar productos derivados de
--       este software sin permiso previo y por escrito.

--   ESTE SOFTWARE SE SUMINISTRA POR LIFIA "COMO ESTÁ" Y CUALQUIER GARANTÍA EXPRESA
--   O IMPLÍCITA, INCLUYENDO, PERO NO LIMITADO A, LAS GARANTÍAS IMPLÍCITAS DE
--   COMERCIALIZACIÓN Y APTITUD PARA UN PROPÓSITO DETERMINADO SON RECHAZADAS. EN
--   NINGÚN CASO LIFIA SERÁ RESPONSABLE POR NINGÚN DAÑO DIRECTO, INDIRECTO, INCIDENTAL,
--   ESPECIAL, EJEMPLAR O CONSECUENTE (INCLUYENDO, PERO NO LIMITADO A, LA ADQUISICIÓN
--   DE BIENES O SERVICIOS; LA PÉRDIDA DE USO, DE DATOS O DE BENEFICIOS; O INTERRUPCIÓN
--   DE LA ACTIVIDAD EMPRESARIAL) O POR CUALQUIER TEORÍA DE RESPONSABILIDAD, YA SEA
--   POR CONTRATO, RESPONSABILIDAD ESTRICTA O AGRAVIO (INCLUYENDO NEGLIGENCIA O
--   CUALQUIER OTRA CAUSA) QUE SURJA DE CUALQUIER MANERA DEL USO DE ESTE SOFTWARE,
--   INCLUSO SI SE HA ADVERTIDO DE LA POSIBILIDAD DE TALES DAÑOS.

-- *******************************************************************************/

---------------------------------------------------------------------------------
-- Constans
-----------------------------------------------------------------------------------

FALSE = 0
TRUE  = 1

INFO  = "info"
DEBUG = "debug"
TRACE = "trace"

---------------------------------------------------------------------------------
-- Methods
-----------------------------------------------------------------------------------

-- LOG
function log( level, str )
	dtvlog.log( level, "wari", "lua", str )
end

function logMethod( verb_level, func, ... )
	local str = func .. '( '

	for i = 1, #arg do
		str = str .. tostring(arg[i])
		str = str .. ' '
	end
	str = str .. ')'
	log( verb_level, str )
end

function traceback ()
	local level = 1
	while true do
		local info = debug.getinfo(level, "Sl")
		if not info then break end
		if info.what == "C" then   -- is a C function?
			log( "error", "traceback: C function")
		else   -- a Lua function
			log( "error", string.format("%s: %d", info.short_src, info.currentline) )
		end
		level = level + 1
	end
end

function getFont ()
	return 'Helvetica'
end

function getImgsPath( append )
	if append then
		return mainWindow.imagesPath(append)
	else
		return mainWindow.imagesPath()
	end
end

--EPG
function getCurrentTime()
	return os.time() -- This returns the system clock time (in Unix system)

	-- Should someone decide to take current time from stream
	-- the next lines must be uncommented instead of the previous one.
	-- Since the time taken from the streams is not always consistent,
	-- it was decided to leave it like this. (Useful for debugging).
	-- return os.time{
	-- 	year = tonumber(mainWindow.getTime("%Y")),
	-- 	month = tonumber(mainWindow.getTime("%m")),
	-- 	day = tonumber(mainWindow.getTime("%d")),
	-- 	hour = tonumber(mainWindow.getTime("%H")),
	-- 	min = tonumber(mainWindow.getTime("%M")),
	-- 	sec = tonumber(mainWindow.getTime("%S")),
	-- 	}
end

function parseSecondsOfDuration( aDurationString )
	local split = function (self, sep)
		local sep, fields = sep or ":", {}
		local pattern = string.format("([^%s]+)", sep)
		self:gsub(pattern, function(c) fields[#fields+1] = c end)
		return fields
	end
	local durationTable = split(aDurationString)
	return durationTable[1] * 3600 + durationTable[2] * 60 + durationTable[3]
end

function getShowWithClosestStartToGiven( anInstant, shows )
	local answer = shows[1]
	if (answer == nil) then return nil end
	local difference = math.abs(anInstant - 
			parseTimestampFromString( answer.startDate.day .. " " .. answer.startDate.time ))
	for _,v in pairs(shows) do
		if math.abs(anInstant - parseTimestampFromString( v.startDate.day .. " " .. v.startDate.time )) 
				< difference then 
			answer = v
			difference = math.abs(anInstant - parseTimestampFromString( v.startDate.day .. " " .. v.startDate.time ))
		end
	end
	if (difference > EPG_BLOCK_TIME_GAP / 2) then
		if (parseTimestampFromString( answer.stopDate.day .. " " .. answer.stopDate.time ) <
				anInstant + EPG_BLOCK_TIME_GAP / 2) then
			return nil
		end
	end
	return answer
end

function parseTimestampFromString( aString )
	local split = function (self, sep)
		local sep, fields = sep or ":", {}
		local pattern = string.format("([^%s]+)", sep)
		self:gsub(pattern, function(c) fields[#fields+1] = c end)
		return fields
	end
	

	local startDate = split(split(aString, " ")[1], "-")
	local startTime = split(split(aString, " ")[2])
	return os.time({year = startDate[1], month = startDate[2], day = startDate[3],
					hour = startTime[1], min = startTime[2] --[[, sec = startTime[3] ]]})
end

------------------------------------------------------------------------------------
-- IMPORTANT: Intended for debug purposes only!! Besides, inefficient implementation
-- Tables with a string rep of length > 5000 chars will raise error.
------------------------------------------------------------------------------------
function tableAsString(t, indent)
	if (t == nil) then
		return "t table is nil"
	end
	local o = ""
	indent = indent or 0

	local keys = {}

	for k in pairs(table) do
		keys[#keys+1] = k;
		table.sort(keys, function(a, b)
			local ta, tb = type(a), type(b);
			if (ta ~= tb) then
				return ta < tb;
			else
				return a < b;
			end
			end);
	end
	o = o .. string.rep('  ', indent)..'{\n'
	indent = indent + 1;
	for k, v in pairs(t) do

		local key = k;
		if (type(key) == 'string') then
			if not (string.match(key, '^[A-Za-z_][0-9A-Za-z_]*$')) then
				key = "['"..key.."']";
			end
		elseif (type(key) == 'number') then
			key = "["..key.."]";
		end
		if (type(v) == 'table') then
			if (next(v)) then
				o = o .. ("%s%s = "):format(string.rep('  ', indent), tostring(key))
				o = o .. tableAsString(v, indent)
			else
				o = o .. ("%s%s = {},"):format( string.rep('  ', indent), tostring(key) )
			end 
		elseif (type(v) == 'string') then
			o = o .. ("%s%s = %s,"):format( string.rep('  ', indent), tostring(key), "'"..v.."'" )
		else
			o = o .. ("%s%s = %s,"):format( string.rep('  ', indent), tostring(key), tostring(v) )
		end
	end
	indent = indent - 1;
	o = o .. string.rep('  ', indent)..'}\n'
	if (string.len(o) > 5000) then error("String too lengthy: Checkout for circular references") end
	return o
end

function removeValuesFromTable (aTable, value)
	for k,v in pairs(aTable) do
		if (v == value) then
			table.remove(aTable, k)
		end
	end
end

--- Strings

function firstToUpper(str)
	return (str:gsub("^%l", string.upper))
end

