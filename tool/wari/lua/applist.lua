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

------------------------------------------------------------------------------
-- Application model
------------------------------------------------------------------------------

ApplicationClass = {}   -- Clase que modela una aplicación
ApplicationClass.__index = function (t,k) return ApplicationClass[k] end
function ApplicationClass.new(id, name)
  local self = setmetatable({}, ApplicationClass)
  self.name = name
  self.id = id
  return self
end

function ApplicationClass.buildFromPair( aPair )
	return ApplicationClass.new(aPair.id, aPair.name)
end

function ApplicationClass:run()
	middleware.runApplication(self.id)
end

function ApplicationClass:tostring()
	return tostring(self.id) .. ": " .. self.name
end

AvailableApplications = {}

function updateAvailableApplications()
	AvailableApplications = {}
	for k,v in pairs(middleware.getApplications()) do
		AvailableApplications[#AvailableApplications+1] = ApplicationClass.buildFromPair(v)
	end
end

----------------------------------------------------------------------------------
-- Background widget
-----------------------------------------------------------------------------------

ApplicationsList = wndNew( 'ApplicationsList', RootWindow, 102, 83 )

function ApplicationsList.actions.onCreate( win )
	logMethod( TRACE, 'onCreate', win['name'] )

	win.actions.onShow = applicationsListOnShow

	local bg = 'FondoListaCanales.png'

	--	Background
	local background = bgNew(
		'background',
		win.x,
		win.y,
		bg
	)
	bgAddText(background, "Aplicaciones", win.x+15, win.y+23, 16)
	wndAddWidget( win, background )

	wndResize( win, bgSize(background) ) 

	local tmpImg = createSurfaceFromPath( bg )
	local w, h = tmpImg:getSize()

	local listWidth = w - 30
	local list = listNew('list', win.x+15, win.y+30, listWidth, h - 55, 20)
	listSetTopPadding( list, 8 )

	wndAddWidget( win, list )
	wdtAddAction( list, 'onElementSelected', applicationsListOnSelectedChanged )
	win.variables.apps = list
	win.variables.needsListUpdate = true

	imgs = {
		['barTop'] = "ScrollBarritaArriba.png",
		['barMid'] = "ScrollBarritaMedio.png",
		['barBottom'] = "ScrollBarritaAbajo.png",
		['buttonUp'] = 'ScrollArriba.png',
		['buttonUpSel'] = 'ScrollArriba.png',
		['buttonDown'] = 'ScrollAbajo.png',
		['buttonDownSel'] = 'ScrollAbajo.png'
	}

	local scrollbarHeight = 14
	local scrollbar = scrollbarNew("scrollbar", scrollbarHeight, list, imgs)
	--	scrollbar for apps list
	wndAddWidget(
		win,
		scrollbar
	)

	listSetSelectedColor(list, 22, 182, 241, 255)
	listSetSelectedWidth(list, listWidth - scrollbarHeight -6)

	--	Forward keys
	--	Setup keys: Forward toggle list actions
	win.keys.Release = {
		  [KEY_SMALL_G] = 'onMenuClose',
		  [KEY_EXIT]    = 'onMenuClose',
		  [KEY_ESCAPE]  = 'onMenuClose'
	}
end

function ApplicationsList.actions.onMenuClose( wdt )
	wdtSetVisible( wdt, false )
end

function applicationsListOnShow( applist )
	if applist.variables.needsListUpdate then
		applistUpdate( applist )
	end
	winOnShow( applist )
end

function applistUpdate( applist )
	logMethod( TRACE, 'AppsListUpdate', applist.name )
	applist.variables.needsListUpdate = false
	updateAvailableApplications()
	listContent( applist.variables.apps, AvailableApplications, 1 )
end

function applicationsListOnSelectedChanged( list, selected )
	logMethod( TRACE, 'AppsListOnSelectedChanged', selected:tostring() )
	selected:run()
	wdtSetVisible( ApplicationsList, false)
--    wdtToggleVisible( MenuWindow )
end

function toggleApplicationsList()
	ApplicationsList.actions['onClose'] = nil
	wdtSetParent( ApplicationsList, RootWindow )
	wdtToggleVisible( ApplicationsList )
end
