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

--------------------------------------
-- Widget Slider
--------------------------------------

-- 0 <= percentage <= 100
function slUpdate( sl, percentage )
	logMethod( TRACE, 'slUpdate', sl['name'], percentage )
	sl.position.x = sl.position.minX + (percentage * (sl.position.maxX - sl.position.minX) / 100.0);
	bgSetLocation(sl.image, sl.position.x, sl.position.y, sl.visible)
end

function slSliderImage( sl, img )
	-- Hide the old image
	bgOnHide( sl.image )

	--	Setup the new one
	slSetupSliderImage(sl, img)
end

-- 0 <= percentage <= 100 || percentage == nil
function slNew( name, x, y, sliderBackground, sliderImg, percentage )

	local sl = wdtNew( name, nil, x, y, 'Slider' )

	sl.background = sliderBackground
	bgSetLocation(sl.background, x, y, false)

	slSetupSliderImage( sl, sliderImg, false )
	if (percentage ~= nil) then
		slUpdate( sl, percentage )
	end

	local sw, sh = bgSize( sl.image )
	wdtAddButtonRegion( sl, 1, true, sl.position.minX, sl.position.y, sl.position.maxX-sl.position.minX+sw, sh )
	wdtAddButtonRegion( sl, 1, false, sl.position.minX, sl.position.y, sl.position.maxX-sl.position.minX+sw, sh )

	sl.actions.onShow = slOnShow
	sl.actions.onHide = slOnHide
	sl.actions.onBtnRelease = slOnButtonRelease
	sl.actions.onCreate = emptyMethod

	return sl
end

-- Implementation

function slOnShow( sl )
	logMethod( TRACE, 'slOnShow', sl['name'] )
	bgOnShow( sl.background )
	bgOnShow( sl.image )
end

function slOnHide( sl )
	logMethod( TRACE, 'slOnHide', sl['name'] )
	bgOnHide( sl.background )
	bgOnHide( sl.image )
end

function slOnButtonRelease( sl, overSlider, x, y )
	logMethod( TRACE, 'slOnButton', sl['name'], overSlider )
	if (overSlider) then
		local sw, sh = bgSize( sl.image )
		sl.position.x = x - ( sw / 2 )
		if (sl.position.x > sl.position.maxX) then
			sl.position.x = sl.position.maxX
		elseif (sl.position.x < sl.position.minX) then
			sl.position.x = sl.position.minX
		end
		runAction( sl, 'onSliderPosChanged', (sl.position.x - sl.position.minX) * 100 / (sl.position.maxX - sl.position.minX) )
	end
end

function slSetupSliderImage( sl, img )
	sl.image = img

	local bw, bh = bgSize( sl.background )
	local sw, sh = bgSize( sl.image )

	sl.position = {
		['x'] = sl.x,
		['minX'] = sl.x,
		['maxX'] = sl.x + bw - sw,
		['y'] = sl.y + ((bh + sh) / 2) - sh
	}

	bgSetLocation(sl.image, sl.position.x, sl.position.y, sl.visible)
end