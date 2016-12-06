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
-- Model methods
-----------------------------------------------------------------------------------

---------------------------------------------------------------------------------
-- Channel methods
-----------------------------------------------------------------------------------

function changeChannel(ix)
	if (ix ~= -1) then
		downloadAppStop()
		canvas.flush()
		channel.change(ix)
		ChannelList.variables.needsListUpdate = true
	else
		log( TRACE, 'no channel')
	end
end

function changeFactor( factor )
	local ch = channel.next( channel.current(), factor )
	log( TRACE, 'changeFactor: ch='..ch )
	changeChannel(ch,isVirtual)
end

---------------------------------------------------------------------------------
-- Audio methods
-----------------------------------------------------------------------------------

function toggleMute()
	mixer.toggleMute()
end

function volumenUp()
	mixer.volumeUp()
end

function volumenDown()
	mixer.volumeDown()
end

---------------------------------------------------------------------------------
-- Loop methods
-----------------------------------------------------------------------------------

function exitLoop()
	--	Cancel scan, if necesary
	mainWindow.stop(0)
end

--------------------------------------------------------------------------------------------------
-- Zapper callbacks
--------------------------------------------------------------------------------------------------

function zapperShowUpdated( chID, cur )
	log( TRACE, "Show changed: chID=" .. chID .. ', current='.. tostring(cur) )
end

function zapperCurrentShowBlocked( blocked )
	log( TRACE, "Current show blocked: blocked=" .. blocked )
end

function zapperUpdateReceived( updateInfo )
	log( TRACE, "Zapper update received: updateID=" .. tostring(updateInfo.id) )
end

function zapperUpdateDownloadProgress( step, total )
	local value = (step * 100) / total
	log( TRACE, "Download update progress: step=" .. step .. " total=" .. total .. " value=" .. value )
end

function zapperApplicationLayerEnabled( isEnabled )
	log( TRACE, "Applications running:" .. isEnabled )
	downloadAppStop()
	canvas.flush()
end

function zapperApplicationDownloadProgress( step, total )
	percentage = math.floor( step * 100 / total )
	log( TRACE, "Download application: step=" .. step .. " total=" .. total .. " progress=" .. percentage )
	downloadAppUpdate( percentage )
	canvas.flush()
end

function zapperOnStandby( isOff )
	log( TRACE, "On standby: off=" .. tostring(isOff) )
end

function refreshEPG()
  refreshEPGBackground(EPGWindow)
  EPGWindow.schedule:refresh()
  Timers.epg.id = mainWindow.registerTimer(Timers.epg.timeout)
end

function zapperOnTimeChanged( isValid )
	log( TRACE, "On time changed: valid=" .. tostring(isValid) )
end

function zapperOnSessionChanged( isActive )
	log( TRACE, "On session changed: active=" .. tostring(isActive) )
end

--------------------------------------------------------------------------------------------------
-- Timers
--------------------------------------------------------------------------------------------------

Timers = {
	download     = {id=-1, timeout=5000, action=downloadAppStop},
	notification = {id=-1, timeout=2000, action=nil},
	epg          = {id=-1, timeout=5000, action=refreshEPG},
}

function OnTimerEvent( tID )
	logMethod( TRACE, "OnTimerEvent", "id=" .. tID )
	for _,timer in pairs(Timers) do
		if (tID == timer['id']) and timer.action then
			timer.action()
			break
		end
	end
	canvas.flush()
end
