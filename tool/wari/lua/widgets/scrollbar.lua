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
-- Scrollbar methods
--------------------------------------------------------------------------------------------------

function scrollbarNew( name, w, list, imgs )
	logMethod( TRACE, 'scrollbarNew', name )

	local scrollbar = wdtNew(name, nil, x, y, 'Scrollbar')
	scrollbar.actions.onShow = scrollbarOnShow
	scrollbar.actions.onHide = scrollbarOnHide

	scrollbar.width = w
	
	--	Set the scrollbar to the list
	listSetVerticalScrollbar(list, scrollbar)
	scrollbar.list = list
	
	--	Create the background surface
	scrollbar.background = createSurface( scrollbar.x, scrollbar.y, scrollbar.width, scrollbar.height )
	scrollbar.background:autoFlush(true)
	scrollbar.background:setVisible(false)
	scrollbar.background:setColor(51, 51, 51, 255)
	scrollbar.background:fillRect(0, 0, scrollbar.width, scrollbar.height)

	--	Set the click region
	scrollbar.actions.onButton = scrollbarOnButton
	wdtAddButtonRegion( scrollbar, 1, true, scrollbar.x, scrollbar.y, scrollbar.width, scrollbar.height )

	-- Create the scroll buttons

	scrollbar.buttonUp = btnNew( 
		'btnUp',
		scrollbar.x,
		scrollbar.y,
		imgs['buttonUp'],
		imgs['buttonUpSel']
	)
	btnAddAction(scrollbar.buttonUp, emptyMethod)

	local tmpImg = canvas.createSurfaceFromPath(getImgsPath(imgs['buttonDown']))
	local _, hBottomButton = tmpImg:getSize()
	canvas.destroy(tmpImg)

	scrollbar.buttonDown = btnNew(
		'btnUp',
		scrollbar.x,
		scrollbar.y + scrollbar.height - hBottomButton,
		imgs['buttonDown'],
		imgs['buttonDownSel']
	)
	btnAddAction(scrollbar.buttonDown, emptyMethod)

	-- Bar configuration
	scrollbar.bar = {}
	scrollbar.bar.topSurface = createSurfaceFromPath(imgs['barTop'])
	scrollbar.bar.midSurface = createSurfaceFromPath(imgs['barMid'])
	scrollbar.bar.bottomSurface = createSurfaceFromPath(imgs['barBottom'])

	local hTopButton = btnGetHeight(scrollbar.buttonUp)
	initBar(scrollbar, scrollbar.y + hTopButton, scrollbar.height - hTopButton - hBottomButton)
	--	Bar surface configuration
	scrollbar.bar.surface = createSurface(0, 0, scrollbar.width, scrollbar.bar.height)
	updateBarSurface(scrollbar, false)
	scrollbar.bar.surface:autoFlush(true)
	scrollbar.bar.surface:setVisible(false)
	scrollbar.bar.surface:setLocation(scrollbar.x, scrollbar.bar.position)

	return scrollbar
end

function scrollbarReset( scrollbar )
	oldSize = scrollbar.bar.height
	initBar(scrollbar, scrollbar.bar.top, scrollbar.bar.holdSize)
	updateBarSurface(scrollbar, oldSize ~= scrollbar.bar.height)
end

function scrollbarDisplaceDown( scrollbar, offset )
	if offset == nil then
		offset = 1
	end
	displace(scrollbar, offset)
end

function scrollbarDisplaceUp( scrollbar, offset )
	if offset == nil then
		offset = 1
	end
	displace(scrollbar, -offset)
end

-- 	Getters

function scrollbarWidth( scrollbar )
	return scrollbar.width
end

-- 	Setters

function scrollbarSetX(scrollbar, x)
	scrollbar.x = x
end

function scrollbarSetY(scrollbar, y)
	scrollbar.y = y
end

function scrollbarSetHeight(scrollbar, h)
	scrollbar.height = h
end

--	Implementation

function displace( scrollbar, offset )
	local position = scrollbar.bar.position + (scrollOffset(scrollbar) * offset)
	local inBounds = scrollbar.bar.top <= position and (position + scrollbar.bar.height) <= scrollbar.bar.bottom

	if inBounds then
		scrollbar.bar.position = position
		scrollbar.bar.surface:setLocation( scrollbar.x, scrollbar.bar.position)
	end
end

function scrollOffset( scrollbar )
	local diff = listCount(scrollbar.list) - listShowedCount(scrollbar.list)
	if diff == 0 then
		return 0
	end
	local diffSize = scrollbar.bar.holdSize - scrollbar.bar.height
	return diffSize / diff
end

function inBarBounds( scrollbar, y )
	return scrollbar.bar.position < y and y < scrollbar.bar.position + scrollbar.bar.height
end

function initBar( scrollbar, top, holdSize )
	logMethod( TRACE, 'initBar', scrollbar.name )

	--	the top position of the bar is the scrollbar.y plus the buttonUp height
	scrollbar.bar.top = top
	scrollbar.bar.position = top

	--	The bar height is the scrollbar height minus the buttons height
	scrollbar.bar.holdSize = holdSize

	scrollbar.bar.height = holdSize * listScrolledPercentage(scrollbar.list)

	scrollbar.bar.bottom = top + holdSize

end

function updateBarSurface( scrollbar, resize )
	local base = 0
	local bar = scrollbar.bar.height

	if resize then
		scrollbar.bar.surface:resize(scrollbar.width, scrollbar.bar.height)
	end

	scrollbar.bar.surface:blit( 0, base, scrollbar.bar.topSurface )

	local _, h = scrollbar.bar.topSurface:getSize()

	base = base + h
	bar = bar - h

	local _, h = scrollbar.bar.bottomSurface:getSize()

	bar = bar - h
	scrollbar.bar.surface:blit( 0, base + bar, scrollbar.bar.bottomSurface )

	for i=0, bar do
		scrollbar.bar.surface:blit( 0, base + i, scrollbar.bar.midSurface )
	end
end

--	Hooks methods
function scrollbarOnShow( scrollbar )
	logMethod( TRACE, 'scrollbarOnShow', scrollbar.name )
	scrollbar.background:setVisible(true)
	scrollbar.bar.surface:setVisible(true)

	wdtSetVisible(scrollbar.buttonUp, true)
	wdtSetVisible(scrollbar.buttonDown, true)
end

function scrollbarOnHide( scrollbar )
	logMethod( TRACE, 'scrollbarOnHide', scrollbar.name )
	scrollbar.background:setVisible(false)
	scrollbar.bar.surface:setVisible(false)

	wdtSetVisible(scrollbar.buttonUp, false)
	wdtSetVisible(scrollbar.buttonDown, false)
end

function scrollbarOnButton( scrollbar, button, isPress, x, y )
	if wdtHandleButton(scrollbar.buttonUp, button, isPress, x, y) then
		listScrollUp(scrollbar.list)
	elseif wdtHandleButton(scrollbar.buttonDown, button, isPress, x, y) then
		listScrollDown(scrollbar.list)
	elseif not inBarBounds(scrollbar, y) then
		if (y > scrollbar.bar.position) then
			listScrollDown(scrollbar.list)
		else
			listScrollUp(scrollbar.list)
		end
	end
end
