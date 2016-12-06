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

--------------------------------------------------------------------------------------------------
-- Actions methods
--------------------------------------------------------------------------------------------------

function emptyMethod()
end

function runAction( wdt, actionName, ... )
	local action = wdt['actions'][actionName]
	if action then
		logMethod( TRACE, 'runAction', wdt['name'], actionName, unpack(arg) )
		if #arg > 0 then
			action( wdt, unpack(arg) )
		else
			action( wdt )
		end
	elseif wdt.parent ~= nil then
		runAction( wdt.parent, actionName, unpack(arg) )
	else
		log( TRACE, 'runAction: action not found: wdt=' .. wdt['name'] .. ' action=' .. actionName )
	end
end

--------------------------------------------------------------------------------------------------
-- Widget methods
--------------------------------------------------------------------------------------------------

-- Widget that has been pressed and stil not released
local currentWidget = nil

function wdtCurrentWidget()
	return currentWidget
end

function wdtNew( wdtName, parentObj, posX, posY, cls )
	local wdt = {
		name = wdtName,
		parent = parentObj,
		class = cls,
		visible = false,
		x = posX,
		y = posY,
		actions = {},
		buttons = {
			Press = {},
			Release = {}
		},
		keys = {
			Press = {},
			Release = {}
		}
	}
	wdt.actions.onKeyPress = emptyMethod
	wdt.actions.onKeyRelease = emptyMethod
	wdt.actions.onKey = wdtOnKey
	wdt.actions.onButton = wdtOnButton
	wdt.actions.onBtnPress = emptyMethod
	wdt.actions.onBtnRelease = emptyMethod

	return wdt
end

function wdtCreate( wdt )
	logMethod( TRACE, 'wdtCreate', wdt['name'] )
	runAction( wdt, 'onCreate' )
end

function wdtHandleKey( wdt, key, isUp )
	if wdtGetKeyAction( wdt, key, isUp ) then
		return true
	else
		return false
	end
end

function wdtPointInRect( rect, x, y )
	return x >= rect.x and x <= rect.x+rect.w and
		   y >= rect.y and y <= rect.y+rect.h
end

function wdtAddButtonRegion( wdt, button, isPress, x, y, w, h )
	logMethod( TRACE, 'wdtAddButtonRegion', wdt['name'], button, isPress, x, y, w, h )
	local act = wdtGetKeyEventName( wdt, not isPress )
	local evt = {
		['button'] = button,
		['x'] = x,
		['y'] = y,
		['w'] = w,
		['h'] = h
	}

	table.insert( wdt.buttons[act], evt )
end

function wdtHandleButton( wdt, button, isPress, x, y )
	local act = wdtGetKeyEventName( wdt, not isPress )
	for i=1, #wdt.buttons[act] do
		local buttonEvt = wdt.buttons[act][i]
		logMethod( TRACE, 'wdtHandleButton', wdt['name'], buttonEvt.button, buttonEvt.x, buttonEvt.y, buttonEvt.w, buttonEvt.h )
		if buttonEvt.button == button and wdtPointInRect( buttonEvt, x, y ) then
			return true
		end
	end
	return false
end

function wdtAddAction( wdt, actionName, action )
	wdt.actions[actionName] = action
end

function wdtSetParent( wdt, par )
	logMethod( TRACE, 'wdtSetParent', wdt['name'], par['name'] )
	wdt.parent = par
end

function wdtProcessKey( wdt, key, isRelease )
	logMethod( TRACE, 'wdtProcessKey', wdt['name'], key, isRelease )

	--	Send action if up or down
	local actionName = 'onKey' .. wdtGetKeyEventName( wdt, isRelease )
	runAction( wdt, actionName, key )

	--	Run generic press action
	runAction( wdt, 'onKey', key, isRelease )
end

function wdtProcessButton( wdt, button, isPress, x, y )
	logMethod( TRACE, 'wdtProcessButton', wdt['name'], button, isPress, x, y )
	runAction( wdt, 'onButton', button, isPress, x, y )
end

function wdtSetVisible( wdt, state )
	logMethod( TRACE, 'wdtSetVisible', wdt['name'], state )
	if wdt.visible ~= state then
		wdt.visible = state
		if state == true then
			runAction( wdt, 'onShow' )
		else
			runAction( wdt, 'onHide' )
		end
	end
end

function wdtToggleVisible( wdt )
	logMethod( TRACE, 'wdtToggleVisible', wdt['name'] )
	wdtSetVisible( wdt, not wdt.visible )
end

function wdtAddKey( wdt, key, keyEventName, action )
	wdt.keys[keyEventName][key] = action
end

function wdtForwardAction( srcWdt, srcAction, dstWdt, dstAction )
	wdtAddAction( srcWdt, srcAction, function() runAction( dstWdt, dstAction ) end )
end

function wdtGetKeyEventName( wdt, isRelease )
	if isRelease == nil then
		isRelease = false
	end
	local evtName
	if isRelease then
		evtName = 'Release'
	else
		evtName = 'Press'
	end
	return evtName
end

function wdtGetKeyAction( wdt, key, isRelease )
	local name = wdtGetKeyEventName( wdt, isRelease )
	return wdt.keys[name][key]
end

function wdtOnKey( wdt, key, isRelease )
	logMethod( TRACE, "wdtOnKey", wdt['name'], key, isRelease )
	local actionName = wdtGetKeyAction(wdt,key,isRelease)
	if actionName then
		runAction( wdt, actionName )
	elseif wdt.parent ~= nil and wdt['class'] ~= 'Window' then
		wdtProcessKey( wdt.parent, key, isRelease )
	else
		log( TRACE, 'processKey('.. key ..'), unrecognized' )
	end
end

function wdtOnButton( wdt, button, isPress, x, y )
	logMethod( TRACE, 'wdtOnButton', wdt['name'], button, isPress )
	if isPress then
		wdtOnBtnPress( wdt, x, y )
	else
		wdtOnBtnRelease( wdt, true, x, y )
	end
end

function wdtOnBtnPress( wdt, x, y )
	logMethod( TRACE, 'wdtOnBtnPress', wdt['name'] );
	currentWidget = wdt
	runAction( wdt, 'onBtnPress', x, y )
end

function wdtOnBtnRelease( wdt, overWidget, x, y )
	logMethod( TRACE, 'wdtOnBtnRelease', wdt['name'], overWidget );
	runAction( wdt, 'onBtnRelease', overWidget, x, y )
	currentWidget = nil
end

--------------------------------------------------------------------------------------------------
-- Zapper callbacks
--------------------------------------------------------------------------------------------------

function OnButtonEvent( id, isPress, x, y )
	logMethod( TRACE, 'OnButtonEvent', id, isPress, x, y )
	local isBtnPress = (isPress == 1)
	if (isBtnPress) then
		wdtProcessButton( wndCurrentWindow(), id, isBtnPress, x, y )
	else
		local currWdt = wdtCurrentWidget()
		if (currWdt ~= nil) then
			wdtOnBtnRelease( currWdt, wdtHandleButton(currWdt, id, false, x, y), x, y )
		end
	end
	canvas.flush()
end

function OnKeyEvent( key, isUp )
	logMethod( TRACE, 'OnKeyEvent', key, isUp )
	wdtProcessKey( wndCurrentWindow(), key, isUp == 1 )
	canvas.flush()
end
