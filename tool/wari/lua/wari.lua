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

-- Libs
require "util"                -- Aux functions
require "group"               -- Grouping functions
require "widgets/widget"      -- Widget functions
require "widgets/button"      -- Button functions
require "widgets/window"      -- Window functions
require "widgets/label"       -- Label functions
require "widgets/progressbar" -- ProgressBar functions
require "widgets/background"  -- Background functions
require "widgets/list"        -- List functions
require "widgets/slider"      -- Slider functions
require "widgets/scrollbar"   -- Scrollbar functions
require "widgets/text"         -- Text widget for show description
require "widgets/schedule"     -- Schedule functions, implementation of schedule table in future EPG (Electronic program guide)
require "widgets/logocontainer" -- Widget for containing and changing images. Initially thought for Channel logos.
require "widgets/notification" -- Module for displaying small notifications to the user that fade with time
-- Implementation
require "root"                 -- root window
require "menu"                 -- menu window
require "scan"                 -- scan window
require "channellist"          -- channel list window
require "applist"              -- available applications list window
require "downloadapplication"  -- download application widget
require "about"                -- about window
require "cc"                   -- cc widget
require "epg"                  -- Inclusion of future EPG
require "audio"                -- audio track management (only includes changing so far)

require "model" -- model methods


--------------------------------------------------------------------------------------------------
-- Main
--------------------------------------------------------------------------------------------------

log( INFO, "init" )

-- Create windows
wdtCreate( RootWindow )
wdtCreate( MenuWindow )
wdtCreate( ChannelList )
wdtCreate( ApplicationsList )
wdtCreate( ScanWindow )
wdtCreate( DownloadAppWidget )
wdtCreate( AboutWindow )
wdtCreate( EPGWindow )

setAttribute( setZIndex, canvas.zapperZIndex() )

-- Show root window (transparent)
wdtToggleVisible( RootWindow )

-- If no channels, show splash
if (channels.count() == 0) then
	log( INFO, "Sin canales!" )
	mainWindow.showBackground(1)
	wdtToggleVisible( MenuWindow )
	canvas.flush()
end

-- Run main loop
mainWindow.run()

log( INFO, "exit" )

