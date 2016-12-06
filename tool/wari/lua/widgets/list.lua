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
-- List methods
--------------------------------------------------------------------------------------------------

-- 	list API
function listNew( name, x, y, w, h, itemHeight )
	logMethod( TRACE, 'listNew', name )

	local list = wdtNew( name, nil, x, y, 'List' )
	list._surface = createSurface( x, y, w, h )
	list._surface:autoFlush(true)
	list._surface:setVisible(false)

	list.actions.onShow = listOnShow
	list.actions.onHide = listOnHide
	list.actions.onCursorUp = navUp
	list.actions.onCursorDown = navDown
	list.actions.onEnter = listOnEnter
	list.actions.onButton = listOnButton


	wdtAddKey( list, KEY_CURSOR_DOWN, 'Press', 'onCursorDown' )
	wdtAddKey( list, KEY_CURSOR_UP, 'Press', 'onCursorUp' )
	wdtAddKey( list, KEY_ENTER, 'Press', 'onEnter' )

	list.width = w
	list.height = h
	list.padding = { ['top'] = 0, ['right'] = 0, ['bottom'] = 0, ['left'] = 0}
	list.itemHeight = itemHeight
	list.characterSize  =  14

	list.selAttr = {}
	list.selAttr.color = { ['r'] = 0, ['g'] = 0, ['b'] = 0, ['a'] = 0}
	list.selAttr.width = list.width

	list.currAttr = {}
	list.currAttr.color = { ['r'] = 100, ['g'] = 150, ['b'] = 150, ['a'] = 255}
	list.currAttr.width = list.width

	list.fontColor = { ['r'] = 255, ['g'] = 255, ['b'] = 255, ['a'] = 255}

	list.content  =  {}
	list.toShow = {}
	list.index  =  1
	list.scrollOffset = 0

-- 	configureClickRegion(list)

	return list
end

--	contentArray elements must define tostring() method
--	the selected element in the list is optional
function listContent( list, contentArray, selected )
	logMethod( TRACE, 'listContent', list['name'] )
	list.content = contentArray
	local size

	if (#list.content < (list.height / list.itemHeight)) then
		size = #list.content
	else
		size = math.floor(list.height / list.itemHeight)
	end

	if selected == nil then
		selected = 1
	end

	local offset = 0
	if selected > size then
		offset = selected - size
	end

	list.toShow = {}
	for i=1 , size do
		list.toShow[i]  =  i + offset
	end

	list.selected = selected
	list.index  = selected
	list.scrollOffset = offset

	if (list.scrollbar) then
		scrollbarReset(list.scrollbar)
		scrollbarDisplaceDown(list.scrollbar, offset)
	end

	drawList( list )
end

function listScrollDown( list )
	if list.toShow[#list.toShow] < #list.content then
		table.remove(list.toShow,1)
		table.insert(list.toShow, list.toShow[#list.toShow]+1)
		drawList(list)
		if (list.scrollbar) then
			scrollbarDisplaceDown(list.scrollbar)
			if (list.scrollOffset < #list.content) then
				list.scrollOffset = list.scrollOffset + 1
			end
		end
	end
end

function listScrollUp( list )
	if list.toShow[1] > 1 then
		table.remove(list.toShow)
		table.insert(list.toShow, 1, list.toShow[1]-1)
		drawList(list)
		if (list.scrollbar) then
			scrollbarDisplaceUp(list.scrollbar)
			if (list.scrollOffset > 0 ) then
				list.scrollOffset = list.scrollOffset - 1
			end
		end
	end
end

--	Set the scrollbar and configure it to be adapted to the list
function listSetVerticalScrollbar( list, scrollbar )
	list.scrollbar = scrollbar
	list.width = list.width - scrollbarWidth(scrollbar)
	configureClickRegion(list)

	scrollbarSetX(scrollbar, list.x + list.width)
	scrollbarSetY(scrollbar, list.y)
	scrollbarSetHeight(scrollbar, list.height)
end

function listScrolledPercentage( list )
	local count = listCount(list)
	local percentage = 1

	if count > 0 then
		percentage = (listShowedCount(list) / count)
	end

	return percentage
end

--	Setters

function listSetSelectedColor( list, r, g, b, a )
	list.selAttr.color.r = r
	list.selAttr.color.g = g
	list.selAttr.color.b = b
	list.selAttr.color.a = a
end

function listSetSelectedWidth( list, w )
	list.selAttr.width = w
end

function listSetTopPadding( list, tPadding )
	list.padding.top = tPadding
end

--	Getters
function listItemHeight( list )
	return list.itemHeight
end

function listItemWidth( list )
	return list.width
end

function listShowedCount( list )
	return #list.toShow
end

function listCount( list )
	return #list.content
end

--	Internal methods
function navDown( list )
	logMethod( TRACE, 'navDown', list['name'] )
	if list.index < #list.content then
		list.index = list.index + 1
		if needScrool(list) then
			listScrollDown(list)
		else
			drawList(list)
		end
	end
end

function navUp( list )
	logMethod( TRACE, 'navUp', list['name'] )
	if list.index > 1 then
		list.index = list.index - 1
		if needScrool(list) then
			listScrollUp(list)
		else
			drawList(list)
		end
	end
end

function configureClickRegion( list )
	wdtAddButtonRegion( list, 1, true, list.x, list.y, list.width, list.height )
end

function showPosition(list, y)
	local result = nil
	local relativeY = y - list.y - list.padding.top

	if (relativeY <= (#list.toShow * list.itemHeight)) and (relativeY > 0) then
		result = math.floor(relativeY / list.itemHeight) + 1
	end

	return result
end

function needScrool( list )
	return (list.index < list.toShow[1] or list.index > list.toShow[#list.toShow])
end

function drawText(list, index, y )
	local txt = list.content[list.toShow[index]]:tostring()
	local _, h = list._surface:measureText("A")
	local baseY = y + list.itemHeight * (index-1)
	baseY = baseY - (list._surface:fontDescent() / 3)
	list._surface:drawText( 5, baseY + (list.itemHeight - h) / 2, listItemWidth(list), h, txt )
end

function drawList( list )
	logMethod( TRACE, 'drawList', list.name )
	local y = list.padding.top

--	Clean canvas and draw black background for the list
	list._surface:clear()
	list._surface:setColor(0, 0, 0, 255)
	list._surface:fillRect(0, 0, list.width, list.height)

	if (#list.toShow > 0) then
		
		list._surface:setFont( getFont(), list.characterSize )

		list._surface:setColor(list.fontColor.r, list.fontColor.g, list.fontColor.b, list.fontColor.a)
		for i=1, #list.toShow do
			if  (list.toShow[i] == list.index) then
				list._surface:setColor(list.selAttr.color.r, list.selAttr.color.g, list.selAttr.color.b, list.selAttr.color.a)
				list._surface:fillRect(0, y + list.itemHeight * (i-1), list.selAttr.width, list.itemHeight)
				list._surface:setColor(list.fontColor.r, list.fontColor.g, list.fontColor.b, list.fontColor.a)
			end

			if (list.toShow[i] == list.selected ) then
				list._surface:setColor(list.currAttr.color.r, list.currAttr.color.g, list.currAttr.color.b, list.currAttr.color.a)
				drawText(list, i, y)
				list._surface:setColor(list.fontColor.r, list.fontColor.g, list.fontColor.b, list.fontColor.a)
			else
				drawText(list, i, y)
			end
		end
		
	end
end

function listOnEnter( list )
	list.selected = list.index
	drawList( list )
	runAction( list, 'onElementSelected', list.content[list.index] )
end

--	Hook methods
function listOnShow( list )
	list._surface:setVisible(true)
	logMethod( TRACE, 'listOnShow', list['name'] )
end

function listOnHide( list )
	list._surface:setVisible(false)
	logMethod( TRACE, 'listOnHide', list['name'] )
end

function listOnButton( list, button, isPress, x, y )
	pos = showPosition(list, y)
	if pos ~= nil then
		runAction(list, 'onElementSelected', list.content[list.toShow[pos]])
		list.index = pos + list.scrollOffset
		list.selected = list.index
		drawList(list)
	end
end