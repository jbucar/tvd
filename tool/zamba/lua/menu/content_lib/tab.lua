local config = require "menu.config_page"
local media_file = require 'models.media_file'
local l_util = require "l_util"
local log = require "log"
local disable_support = require 'menu.util.disable_support'

local _M = {} -- API Module
local Tab = {} -- Class

_M.mt = { __index = Tab }

-- Vars
local _visible_rows = 9

-- Internals

local function create_content_view( desc )
	local content_view = list:new(0, 0, config.page.width, config.page.list.height, 3, colors_extend(disable_support.enable_color_table, {textColor = Color.Black}))
	list:setSelected(content_view, -1)
	list:setVisibleRows(content_view, _visible_rows);
	list:setCellWidths(content_view, desc.columns_sizes);
	list:setCellHeaders(content_view, desc.headers);
	list:scrollImages(content_view, "arrow_up.png", "arrow_down.png")
	wgt:setVisible(content_view, FALSE)

	return content_view
end

function _M.new( description, name, icon, help, action_help, enable )
	local inst = {
		_description = description,
		_values = {},
		_disabled = false,
		-- Public vars
		content = create_content_view(description),
		title = name,
		help = help,
		action_help = action_help,
		icon = icon,
		enable = enable and TRUE or FALSE
	}

	wgt:setName(inst.content, string.format("Tab(%s)", name))

	media_file.observe('mount_point_changed', function(mountPoint, isMounted)
		inst:update()
	end)

	return inst
end

function Tab:update( force )
	local actual_values = self:values() or {}
	log.info("Tab", "update equals?=" .. tostring(l_util.table.deep_equals(self._values, actual_values)))
	if (not l_util.table.deep_equals(self._values, actual_values)) or force then
		log.info("Tab", "update disabled?=" .. tostring(self._disabled))
		self._values = actual_values

		list:setValues(self.content, self._description.visible_columns, self._description.index_key, self._values)
		if #self._values > 0 and not self._disabled then
			list:setSelected(self.content, 0)
		else
			list:setSelected(self.content, -1)
		end
	end
 end

function Tab:values()
	return {}
end

function Tab:selected_item()
	return self._values[list:selected(self.content) + 1], list:getCurrentValue(self.content)
end

function Tab:update_row( index, item )
	list:updateRow(self.content, index, self._description.visible_columns, self._description.index_key, item)
end

function Tab:select()
	self:open(TRUE)
	return l_util.ensure_call(self.onSelected, self)
end

function Tab:disable_view()
	self._disabled = true
	disable_support.disable(self.content, disable_support.disabled_list)
	list:setSelected(self.content, -1)
end

function Tab:enable_view()
	self._disabled = false
	disable_support.enable(self.content, {textColor = Color.Black})
	self:update(true)
end

function Tab:open( isOpen )
	wgt:setVisible(self.content, isOpen)
	self:update(isOpen==TRUE)
end

function Tab:unselect()
	self:open(FALSE)
	return l_util.ensure_call(self.onUnselected, self)
end

function Tab:handle_key( key )
	local res = wgt:processKey(self.content, key)
	l_util.ensure_call(self.onKey, self)
	return res
end

function Tab:on_enter()
	if list:getRowCount(self.content) > 0 then
		self:onOk()
	end
end

return _M
