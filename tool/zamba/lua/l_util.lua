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

local M = {}
M.table = {}
M.string = {}
M.math = {}

function M.table.has_key(t, k)
	for key, _ in pairs(t) do
		if k == key then
			return true
		end
	end
	return false
end

function M.table.has_value(t, v)
	for _, val in ipairs(t) do
		if v == val then
			return true
		end
	end
	return false
end

function M.table.shallow_copy( orig )
	local copy = {}
	if type(orig) == 'table' then
		copy = {}
		for orig_key, orig_value in pairs(orig) do
			copy[orig_key] = orig_value
		end
	end

	return copy
end

-- Shallow equals
function M.table.equals( one, two )
	local res = true
	for one_key, one_value in pairs(one) do
		if not (type(one_value) == 'table') then
			res = res and (two[one_key] == one_value)
		end
	end

	return res
end

function M.table.deep_equals(tbl1,tbl2)
	if #tbl1 ~= #tbl2 then return false end
	for k, v in pairs(tbl1) do
		if type(tbl2[k]) == 'table' and type(v) == 'table' then return M.table.deep_equals(v, tbl2[k]) end
		if (tbl2[k] ~= v) then
			return false
		end
	end
	return true
end

function M.table.extend( orig, extension )
	local copy = {}
    if type(orig) == 'table' then
		for orig_key, orig_value in pairs(orig) do
			copy[orig_key] = orig_value
		end
		for extension_key, extension_value in pairs(extension) do
			copy[extension_key] = extension_value
		end
	end

	return copy
end

function M.table.print(table)
	if type(table) == "table" then
		print("Print " .. tostring(table))
		for k,v in pairs(table) do
			print("Item: " .. k .. " = " .. tostring(v))
		end
	end
end

function M.string.split(inputstr, sep)
	if sep == nil then
		sep = "%s"
	end
	t={} ; i=1
	for str in string.gmatch(inputstr, "([^"..sep.."]+)") do
		t[i] = str
		i = i + 1
	end
	return t
end

function M.table.pack(...)
	return {...}
end

function M.math.round( val )
	return math.floor(val + 0.5)
end

-- Functions

function M.center_in_canvas( w, h )
	local canvas_w, canvas_h = canvas.size()
	local x = (canvas_w - w) / 2
	local y = (canvas_h - h) / 2
	return x, y
end

function M.ensure_call( fnc, object, ... )
	if fnc ~= nil and object ~= nil then
		return fnc(object, ...)
	elseif fnc then
		return fnc(...)
	end

	return false
end

function M.call_if_condition( callback, ... )
	local args = {...}
	return function( condition )
		if condition then callback(unpack(args)) end
	end
end

return M
