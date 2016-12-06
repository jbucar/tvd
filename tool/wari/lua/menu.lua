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

----------------------------------------------------------------------------------
-- Menu Window
-----------------------------------------------------------------------------------

MenuWindow = wndNew( 'Menu', RootWindow, 102, 361 )


function menuOnChannelSelected( btn )
	logMethod( TRACE, "menuOnChannelSelected" )
	wdtSetParent( ChannelList, MenuWindow )
	wdtAddAction( ChannelList, 'onClose', function() menuOnChannelClosed(btn) end )
	wdtToggleVisible( ChannelList )
end

function menuOnChannelClosed( btn )
	logMethod( TRACE, "menuOnChannelClosed" )
	btnOnRelease( btn, false )
end

function menuOnAppListSelected( btn )
	logMethod( TRACE, "menuOnAppListSelected" )
	wdtSetParent( ApplicationsList, MenuWindow )
	wdtAddAction( ApplicationsList, 'onClose', function() menuOnAppListClosed(btn) end )
	wdtToggleVisible( ApplicationsList )
end

function menuOnAppListClosed( btn )
	logMethod( TRACE, "menuOnAppListClosed" )
	btnOnRelease( btn, false )
end

function menuOnEPGSelected( btn )
	logMethod( TRACE, "menuOnEPGSelected" )
	wdtToggleVisible( MenuWindow )
	RootWindow.actions.onToggleEPGWindow()
end

function menuOnScan( btn )
	startScan( MenuWindow, function () menuOnScanClosed(btn) end )
end

function menuOnScanClosed( btn )
	btnOnRelease( btn, false )
	ChannelList.variables.needsListUpdate = true
end

function menuOnMinimize( btn )
	logMethod( TRACE, "menuOnMinimize" )
	mainWindow.iconify( true );
end

function menuOnAbout( wdt )
	logMethod( TRACE, "menuOnAbout" )
	wdtToggleVisible(AboutWindow)
end

function menuOnMute( btn )
	logMethod( TRACE, "menuOnMute" )
	mixer.toggleMute()
	menuUpdateVolume( MenuWindow )
end

function menuOnVolDown( slider )
	runAction( RootWindow, 'onVolDown' )
	menuUpdateVolume( MenuWindow )
end

function menuOnVolUp( slider )
	runAction( RootWindow, 'onVolUp' )
	menuUpdateVolume( MenuWindow )
end

function menuOnVolumeClicked( slider, percentage )
	logMethod( TRACE, 'menuOnVolumeClicked', slider['name'], percentage )
	mixer.setVolume( percentage * mixer.maxVolume() / 100 )
	menuUpdateVolume( MenuWindow )
end

function menuOnShow( menu )
	winOnShow( menu )
	menuUpdateVolume( menu )
end

function MenuWindow.actions.onCreate( win )
	log( TRACE, 'MenuWindow::onCreate' )

	wdtAddAction( win, 'onShow', menuOnShow )

	local wdt
	local top = 29
	--	Background
	wdt=bgNew(
		'background',
		win.x,
		win.y+top,
		'FondoPlayer.png'
	)
	local y = win.y+top+23
	bgAddText(wdt, 'Canales', win.x+15, y, 16)
	bgAddText(wdt, 'TVi', win.x+185, y, 16)
	bgAddText(wdt, 'Volumen', win.x+235, y, 16)
	bgAddText(wdt, 'Pantalla', win.x+415, y, 16)
	wndAddWidget( win, wdt )

	--	Resize window to background image
	local ww, wh = bgSize(wdt)
	wndResize( win, ww, wh+top )

	local x = win.x+15
	local y = win.y+top+31

	--	Scan
	wdt=btnNew( 
		'btnScan',
		x, y,
		'BtnCanalesScan.png',
		'BtnCanalesScan-selec.png'
	)
	btnAddShortcut( wdt, KEY_SMALL_R )
	btnAddAction( wdt, menuOnScan )
	btnReleaseOnSelected( wdt, false )
	wndAddWidget( win, wdt )

	--	List of Channels
	x = x+30
	wdt=btnNew( 
		'btnChannels',
		x, y,
		'BtnCanalesLista.png',
		'BtnCanalesLista-selec.png'
	)
	btnAddShortcut( wdt, KEY_SMALL_L )
	btnAddAction( wdt, menuOnChannelSelected )
	btnReleaseOnSelected( wdt, false )
	wndAddWidget( win, wdt )

	--	EPG
	x = x+30
	wdt=btnNew(
		'btnEPG',
		x, y,
		'BtnCanalesGuia.png',
		'BtnCanalesGuia-selec.png'
	)
	btnAddShortcut( wdt, KEY_SMALL_E )
	btnAddAction( wdt, menuOnEPGSelected )
	btnReleaseOnSelected( wdt, false )
	wndAddWidget( win, wdt )

	--	ChannelDown
	x = x+30
	wdt=btnNew( 
		'btnChannelDown',
		x, y,
		'BtnCanalesAnt.png',
		'BtnCanalesAnt-selec.png'
	)
	btnAddShortcut( wdt, KEY_CHANNEL_DOWN )
	btnAddShortcut( wdt, KEY_SMALL_Z )
	btnAddShortcut( wdt, KEY_CURSOR_LEFT )
	wdtForwardAction( wdt, 'onSelected', RootWindow, 'onChannelDown' )
	wndAddWidget( win, wdt )

	--	ChannelUP
	x = x+30
	wdt=btnNew( 
		'btnChannelUp',
		x, y,
		'BtnCanalesSig.png',
		'BtnCanalesSig-selec.png'
	)
	btnAddShortcut( wdt, KEY_CHANNEL_UP )
	btnAddShortcut( wdt, KEY_SMALL_Q )
	btnAddShortcut( wdt, KEY_CURSOR_RIGHT )
	wdtForwardAction( wdt, 'onSelected', RootWindow, 'onChannelUp' )
	wndAddWidget( win, wdt )

	--	Apps
	x = x+50
	wdt=btnNew(
		'btnApps',
		x, y,
		'BtnApps.png',
		'BtnApps-selec.png'
	)

	btnAddShortcut( wdt, KEY_SMALL_G )
	btnAddAction( wdt, menuOnAppListSelected )
	btnReleaseOnSelected( wdt, false )
	wndAddWidget( win, wdt )



	--	Mute
	x = x+50
	win.variables.mute = btnNew( 
		'btnMute',
		x, y,
		'BtnVolumen.png',
		'BtnVolumen-selec.png',
		'BtnVolumenMute.png',
		'BtnVolumenMute-selec.png'
	)
	btnAddShortcut( win.variables.mute, KEY_MUTE )
	btnAddShortcut( win.variables.mute, KEY_SMALL_M )
	btnAddAction( win.variables.mute, menuOnMute )
	wndAddWidget( win, win.variables.mute )

	--	Volume

	-- Images for the slider
	local bg = bgNew('sliderBackground', 0, 0, 'BarraVolumen.png')
	win.variables.imageMutedSlider = bgNew('imageMutedSlider', 0, 0, 'VolumenMutedSlider.png')
	win.variables.imageSlider = bgNew('imageSlider', 0, 0, 'VolumenSlider.png')

	x = x+30
	win.variables.volumeSlider = slNew(
		'volumeSlider', x, y+3,
		bg, win.variables.imageSlider,
		mixer.getVolume() * 100 / mixer.maxVolume()
	)
	wdtAddAction( win.variables.volumeSlider, 'onVolUp', menuOnVolUp )
	wdtAddAction( win.variables.volumeSlider, 'onVolDown', menuOnVolDown )
	wdtAddAction( win.variables.volumeSlider, 'onSliderPosChanged', menuOnVolumeClicked )
	wdtAddKey( win.variables.volumeSlider, KEY_VOLUME_UP, 'Press', 'onVolUp' )
	wdtAddKey( win.variables.volumeSlider, KEY_SMALL_W, 'Press', 'onVolUp' )
	wdtAddKey( win.variables.volumeSlider, KEY_VOLUME_DOWN, 'Press', 'onVolDown' )
	wdtAddKey( win.variables.volumeSlider, KEY_SMALL_X, 'Press', 'onVolDown' )
	wndAddWidget( win, win.variables.volumeSlider )

	--	CC
	x = x+100
	wdt=btnNew(
		'btnCC',
		x, y,
		'BtnCC.png',
		'BtnCC-selec.png'
	)

	btnAddShortcut( wdt, KEY_SUBTITLE )
	btnAddShortcut( wdt, KEY_SMALL_S )
	wdtForwardAction( wdt, 'onSelected', RootWindow, 'onToggleSub' )
	wndAddWidget( win, wdt )

	--	Full Screen
	x = x+50
	wdt=btnNew( 
		'btnFullScreen',
		x, y,
		'BtnPantallaCompleta.png',
		'BtnPantallaCompleta-selec.png'
	)
	btnAddShortcut( wdt, KEY_SMALL_F )
	wdtForwardAction( wdt, 'onSelected', RootWindow, 'onToggleFullScreen' )
	wndAddWidget( win, wdt )

	--	Minimize
	x = x+30
	wdt=btnNew( 
		'btnMinimize',
		x, y,
		'BtnPantallaMinimizar.png',
		'BtnPantallaMinimizar-selec.png'
	)
	btnAddAction( wdt, menuOnMinimize )
	wndAddWidget( win, wdt )

	--	Close
	x = x+30
	wdt=btnNew( 
		'btnClose',
		x, y,
		'BtnPantallaCerrar.png',
		'BtnPantallaCerrar-selec.png'
	)
	
	btnAddShortcut( wdt, KEY_EXIT )
	wdtForwardAction( wdt, 'onSelected', RootWindow, 'onExit' )
	wndAddWidget( win, wdt )

	-- Setup about window
	wdt = bgNew('logo', 528, 371, 'LogoWari.png')
	win.actions['onAbout'] = menuOnAbout
	wdtAddKey( wdt, KEY_SMALL_I, 'Release', 'onAbout' )
	wdtAddButtonRegion( wdt, 1, true, 528, 371, 75, 26 )
	wdtAddAction( wdt, 'onBtnPress', menuOnAbout )
	wndAddWidget( win, wdt )

	--	Setup keys: Forward toggle menu actions
	win.keys.Press = {
		  [KEY_MENU]    = 'onToggleMenu'
		, [KEY_SMALL_C] = 'onToggleMenu'
	}
end

-- Private functions
function menuUpdateVolume( menu )
	local muted = (mixer.isMuted() == 1)
	btnSetSelected( menu.variables.mute, muted, 'up' )
	if (muted) then
		slSliderImage( menu.variables.volumeSlider, menu.variables.imageMutedSlider )
	else
		slSliderImage( menu.variables.volumeSlider, menu.variables.imageSlider )
	end

	local volPercentage = mixer.getVolume() * 100 / mixer.maxVolume()
	slUpdate( menu.variables.volumeSlider, volPercentage )
	if ( (not menu.variables.mute.selected) and (volPercentage==0)) then
		menuOnMute( menu.variables.mute )
	end
end
