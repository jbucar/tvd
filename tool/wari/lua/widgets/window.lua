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
-- Variables
-----------------------------------------------------------------------------------

local CurrentWindow = nil
local KeysLocked = 0

---------------------------------------------------------------------------------
-- Key Locking ------------
-----------------------------------------------------------------------------------

function lockKeys()
	logMethod( TRACE, 'lockKeys', KeysLocked )
	KeysLocked = KeysLocked + 1
	if KeysLocked > 1 then
		mainWindow.lockKeys(1)
	end
end

function unlockKeys()
	logMethod( TRACE, 'unlockKeys', KeysLocked )
	KeysLocked = KeysLocked - 1
	if KeysLocked <= 1 then
		mainWindow.lockKeys(0)
	end
end

---------------------------------------------------------------------------------
-- Window methods
-----------------------------------------------------------------------------------

function wndCurrentWindow()
	return CurrentWindow
end

function setCurrentWindow( wnd )
	logMethod( TRACE, 'setCurrentWindow', wnd['name'] )
	CurrentWindow = wnd;
end

function wndNew( name, parent, x, y )
	win = wdtNew( name, parent, x, y, 'Window' )
	win.variables = {}
	win.widgets = {}
	win.w = 0
	win.h = 0
	win.actions.onShow = winOnShow
	win.actions.onHide = winOnHide
	win.actions.onKey = winOnKey
	win.actions.onButton = winOnButton
	return win
end

function wndResize( win, w, h )
	logMethod( TRACE, 'wndResize', win['name'], w, h )
	win.w = w
	win.h = h
end

function wndAddWidget( win, wdt )
	wdtCreate( wdt )
	wdtSetParent( wdt, win )
	table.insert( win.widgets, wdt )
end

function wndShowWidgets( win, show )
	for i=1, #win.widgets do
		wdtSetVisible( win.widgets[i], show )
	end
end

-- Implementation
function winOnShow( win )
	logMethod( TRACE, "winOnShow", win['name'] )
	lockKeys()
	setCurrentWindow( win )
	wndShowWidgets( win, true )
end

function winOnHide( win )
	logMethod( TRACE, "winOnHide", win['name'] )
	wndShowWidgets( win, false )
	runAction( win, 'onClose' )
	setCurrentWindow( win.parent )
	unlockKeys()
end

function winOnKey( win, key, isRelease )
	logMethod( TRACE, "winOnKey", win['name'], key, isRelease )
	for i=1, #win.widgets do
		if wdtHandleKey( win.widgets[i], key, isRelease ) then
			runAction( win.widgets[i], 'onKey', key, isRelease )
			return
		end
	end
	wdtOnKey( win, key, isRelease )
end

function winOnButton( win, button, isPress, x, y )
	logMethod( TRACE, "winOnButton", win['name'], button, isPress, x, y )

	if wdtPointInRect( win, x, y ) then
		for i=1, #win.widgets do
			if wdtHandleButton( win.widgets[i], button, isPress, x, y ) then
				runAction( win.widgets[i], 'onButton', button, isPress, x, y )
				return
			end
		end
	elseif isPress then
		wdtSetVisible( win, false )

		local cur=win.parent
		while not wdtPointInRect( cur, x, y ) do
			local tmp = cur
			cur = cur.parent
			wdtSetVisible( tmp, false )
		end
	end
end

function toogleFullScreen( btn )
	logMethod( TRACE, "menuOnFullScreen", mainWindow.isFullscreen() )
	mainWindow.fullscreen( not mainWindow.isFullscreen() );
end

