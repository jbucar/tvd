require 'colors'

local sep = 23
local fontSize = 14
local label_h = 22

local BAR_H = 4

local _M = {}

local function prepare_opts( opts )
	local res = {}
	res.alignment = opts.alignment or Alignment.vCenter_hLeft

	return res
end

function _M.new( win, x, y, horizontal_separation )
	local container = {
		curr_x = x,
		curr_y = y,
		_x = x,
		_y = y,
		_win = win,
		_h_separation = horizontal_separation or 0,
	}

	function container:add_field( name, width, lines, opts )
		opts = opts or {}
		local content_opts = prepare_opts(opts.content or {})
		local header_opts = prepare_opts(opts.header or {})

		local header = label:new(self.curr_x, self._y, width, label_h, labelsColorTable, name, fontSize, header_opts.alignment)
		local content = nil


		if lines then
			content = textarea:new(self.curr_x, self._y + sep, width, label_h * lines, menuColorTable, "", fontSize, lines, 0, content_opts.alignment)
		else
			content = label:new(self.curr_x, self._y + sep, width, label_h, menuColorTable, "", fontSize, content_opts.alignment)
		end

		window:addChild(self._win, header)
		window:addChild(self._win, content)

		self.curr_x = self.curr_x + width + self._h_separation
		self._h_bar = nil

		local tmp = label_h * (lines or 1) + label_h
		if tmp > (self.curr_y - self._y) then
			self.curr_y = tmp + self._y
		end

		return content
	end

	function container:add_field_h( name, width, width_content, sep, opts )
		sep = sep or 0
		local header = label:new(self.curr_x, self._y, width, label_h, labelsColorTable, name, fontSize, Alignment.vCenter_hLeft)
		local content = label:new(self.curr_x + width + sep, self._y, width_content, label_h, dateColorTable, "", fontSize, Alignment.vCenter_hLeft)

		window:addChild(self._win, header)
		window:addChild(self._win, content)

		self.curr_x = self.curr_x + width + sep + width_content + self._h_separation

		if label_h > (self.curr_y - self._y) then
			self.curr_y = label_h + self._y
		end

		return content
	end

	function container:y()
		return self.curr_y
	end

	function container:x()
		return self.curr_x
	end

	return container
end

return _M
