local widget = require 'widget'

network = {}

local function filterAdapters(list, filtered)
	local filteredAdapters = {}
	for i, v in ipairs(list) do
		if v ~= filtered then
			table.insert(filteredAdapters, v)
		end
	end
	return filteredAdapters
end

local function getAdapterId(adapter)
	local name = network.visibleAdapters[adapter]
	for i, v in ipairs(network.adapters) do
		if v == name then
			return i - 1
		end
	end
	return -1
end

local function refreshAdapters()
	network.adapters = net.adapters()
	network.visibleAdapters = filterAdapters(network.adapters, 'lo')
end

function network.create(x, y, w, h)
	local win = window:new( x, y, w, h, menuZIndex, menuColorTable )
	network.verticalSet = verticalset:new(0, 0, w, h, menuColorTable )

	local COMBO_X = 21
	local COMBO_Y = 26
	local COMBO_W = w - 2 * (config.page_border + config.page_margin)
	local COMBO_H = 26

	refreshAdapters()
	network.comboAdapters = widget.makeCombo(COMBO_X, COMBO_Y, COMBO_W, COMBO_H, "Interface", network.visibleAdapters)
	combobox:action( network.comboAdapters, "adapterChanged" )
	network.domain = label:new( COMBO_X, COMBO_Y + (COMBO_H + 5) + 30, 100, 20, menuColorTable, "Dominio: ", 17, Alignment.vTop_hLeft, 0)
	network.dns = label:new( COMBO_X, COMBO_Y + (COMBO_H + 5) + 60, 100, 20, menuColorTable, "Servidor DNS: ", 17, Alignment.vTop_hLeft, 0)
	network.ip = label:new( COMBO_X, COMBO_Y + (COMBO_H + 5) + 112, 100, 20, menuColorTable, "Dirección IP: ", 17, Alignment.vTop_hLeft, 0)
	network.netmask = label:new( COMBO_X, COMBO_Y + (COMBO_H + 5) + 142, 100, 20, menuColorTable, "Mascara de red: ", 17, Alignment.vTop_hLeft, 0)
	network.gateway = label:new( COMBO_X,COMBO_Y + (COMBO_H + 5) + 172, 100, 20, menuColorTable, "Puerta de enlace: " .. net.defaultGW(), 17, Alignment.vTop_hLeft, 0)
	
	verticalset:circular( network.verticalSet, TRUE )
	verticalset:addChild( network.verticalSet, network.comboAdapters )
	verticalset:addChild( network.verticalSet, network.domain, FALSE )
	verticalset:addChild( network.verticalSet, network.ip, FALSE )
	verticalset:addChild( network.verticalSet, network.netmask, FALSE )
	verticalset:addChild( network.verticalSet, network.gateway, FALSE )
	verticalset:addChild( network.verticalSet, network.dns, FALSE )
	verticalset:setActiveItem( network.verticalSet, 0 )

	window:addChild( win, network.verticalSet )
	network.win = win
	wgt:setVisible( network.win, FALSE )

	return network.win
end

function network.onUnselected()
	wgt:processKey(network.verticalSet, KEY_ESCAPE)
	return true
end

function network:onSelected()
	updateHelp("Configuración de red.\nOK = Seleccionar interface  EXIT = Salir del menú    MENU = Menú ppal.")
	refreshAdapters()
	combobox:setItems( network.comboAdapters, network.visibleAdapters )
	verticalset:setActiveItem( network.verticalSet, 0 )
	combobox:setSelected( network.comboAdapters, 0 )
	adapterChanged( TRUE )

	return true
end

function network.handleKey(key)
	local handled = wgt:processKey(network.verticalSet, key)
	if not handled and escKeys()[key] then
		openSubWin(menu)
		handled = true
	end
	return handled
end

function adapterChanged( changed )
	if changed == TRUE then
		local adapter = getAdapterId( combobox:getSelected( network.comboAdapters ) + 1)
		if adapter ~= -1 then --There is at least one adapter
			label:setText( network.domain, "Dominio: " .. net.domain() )
			label:setText( network.ip, "Dirección IP: " .. net.address( adapter ) )
			label:setText( network.netmask, "Mascara de red: " .. net.netmask( adapter ) )
			label:setText( network.gateway, "Puerta de enlace: " .. net.defaultGW() )
			local nameservers = net.nameservers()
			if #nameservers > 0 then
				local dnsStr = "Servidor DNS: " ..  nameservers[1]
				for i=2, #nameservers, 1 do
					dnsStr = dnsStr .. ", " .. nameservers[i]
				end
				label:setText( network.dns, dnsStr )
			end
		end
	end
end
