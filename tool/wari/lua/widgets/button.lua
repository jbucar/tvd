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
-- Button methods
--------------------------------------------------------------------------------------------------

function btnAddAction( btn, action )
	wdtAddAction( btn, 'onSelected', action )
end

function btnAddShortcut( btn, key )
	wdtAddKey( btn, key, 'Press', 'onBtnPress' )
	wdtAddKey( btn, key, 'Release', 'onKeyRelease' )
end

function btnToggleSelected( btn )
	btn.selected = not btn.selected
	btnShow( btn, 'down' )
end

function btnSetSelected( btn, sel, state )
	logMethod( TRACE, 'btnSetSelected', btn['name'], sel, state )
	btn.selected = sel
	btnShow( btn, state )
end

function btnReleaseOnSelected( btn, release )
	btn.releaseOnSelected = release
end

function btnGetWidth( btn )
	return btn.w
end

function btnGetHeight( btn )
	return btn.h
end

function btnNew( name, x, y, upBtn, downBtn, upBtnSel, downBtnSel )
	local btn = wdtNew( name, nil, x, y, 'Button' )

	-- Default init
	btn.selected = false

	-- Images
	btn.images = {}
	btn.images[true] = {}
	btn.images[false] = {}
	btn.images[false].up = auxcreateSurfaceFromPath(upBtn, x, y)
	btn.images[false].down = auxcreateSurfaceFromPath(downBtn, x , y)

	
	-- Fix if sel is empty
	if (upBtnSel == nil) then
		btn.images[true].up = btn.images[false].up
	else
		btn.images[true].up = auxcreateSurfaceFromPath(upBtnSel, x, y)
	end

	if (downBtnSel == nil) then
		btn.images[true].down = btn.images[false].down
	else
		btn.images[true].down = auxcreateSurfaceFromPath(downBtnSel, x, y)
	end

	-- Current surface
	btn.currSurface = btn.images[true].up

	--	w,h
	btn.w, btn.h =  btn.images[false].down:getSize()

	-- Button regions
	wdtAddButtonRegion( btn, 1, true, btn.x, btn.y, btn.w, btn.h )
	wdtAddButtonRegion( btn, 1, false, btn.x, btn.y, btn.w, btn.h )

	-- Actions
	btn.actions.onShow = btnOnShow
	btn.actions.onHide = btnOnHide
	btn.actions.onCreate = emptyMethod
	btn.actions.onKeyRelease = btnOnKeyRelease
	btn.actions.onBtnPress = btnOnPress
	btn.actions.onBtnRelease = btnOnRelease

	btn.releaseOnSelected = true

	return btn
end

-- Implementation
function btnOnShow( btn )
	btnShow( btn, 'up' )
end

function btnShow( btn, state )
	logMethod( TRACE, 'btnShow', btn['name'], state )
	
	btn.currSurface:setVisible(false)
	btn.currSurface = btn.images[btn.selected][state]
	btn.currSurface:setVisible(true)
end

function btnOnHide( btn )
	logMethod( TRACE, 'btnOnHide', btn['name'] )
	btn.currSurface:setVisible(false)
end

function btnOnPress( btn )
	logMethod( TRACE, 'btnOnPress', btn['name'] );
	btnShow( btn, 'down' )
end

function btnOnRelease( btn, overBtn )
	logMethod( TRACE, 'btnOnRelease', btn['name'], overBtn );
	local refresh = true
	if (overBtn) then
		runAction( btn, 'onSelected' )
		refresh = btn.releaseOnSelected
	end
	if (refresh) then
		btnShow( btn, 'up' )
	end
end

function btnOnKeyRelease( btn )
	btnOnRelease( btn, true )
end

--	Aux method
function auxcreateSurfaceFromPath(img, x, y)
	surface = createSurfaceFromPath(img)
	surface:setVisible(false)
	surface:autoFlush(true)

	surface:setLocation(x, y)

	return surface
end