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
-- Widget ProgressBar
--------------------------------------

function pbOnShow( pb )
	logMethod( TRACE, 'pbOnShow', pb['name'] )
	bgOnShow( pb.background )

	pb.progressSurface:setVisible(true)
	pbDrawPercentage( pb )

	lblOnShow( pb.label )
end

function pbOnHide( pb )
	logMethod( TRACE, 'pbOnHide', pb['name'] )
	bgOnHide( pb.background )
	lblOnHide( pb.label )
	pb.progressSurface:setVisible(false)
end

-- 0 <= percentage <= 100
function pbUpdate( pb, percentage, label )
	logMethod( TRACE, 'pbUpdate', pb['name'], percentage, label )
	tmp = percentage / 100.0

	if (tmp < pb.value) then
		pb.progressSurface:clear()
	end

	pb.value = tmp

	if (label ~= nil) then
		lblText(pb.label, label)
	end

	pbDrawPercentage( pb )
end

function pbNew( name, x, y, backImg,
		label, labelSize, labelPosX, labelPosY,
		progressRectX, progressRectY, progressRectW, progressRectH )

	local pb = wdtNew( name, nil, x, y, 'ProgressBar' )
	pb.value = 0

	pb.background = bgNew( name .. '_bg', x, y, backImg )

	pb.label = lblNew("pbLabel", x + labelPosX, y + labelPosY, label, labelSize)

	pb.progressSurface = createSurface(x + progressRectX, y + progressRectY, progressRectW, progressRectH)
	pb.progressSurface:autoFlush(true)
	pb.progressSurface:setVisible(false)

	pb.progressRect = {
		['rect'] = {
			['w'] = progressRectW,
			['h'] = progressRectH
		},

		['color'] = {
			['r'] = 0,
			['g'] = 127,
			['b'] = 255,
			['alpha'] = 255
		}
	}

	pb.actions.onShow = pbOnShow
	pb.actions.onHide = pbOnHide
	pb.actions.onCreate = emptyMethod

	return pb
end

-- Private functions
function pbDrawPercentage( pb )
	pb.progressSurface:setColor( pb.progressRect.color.r, pb.progressRect.color.g, pb.progressRect.color.b, pb.progressRect.color.alpha )

	local width = pb.progressRect.rect.w * pb.value
	if width < 1 then
		width = 1
	end

	pb.progressSurface:fillRect(
		0,
		0,
		width,
		pb.progressRect.rect.h
	)
end
