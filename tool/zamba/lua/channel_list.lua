local chanelListColorTable = {
	bgColor = Color.Gray4,
	textColor = Color.Black,
	selectedTextColor = Color.Black,
	cellColor = Color.Gray3,
	selectedCellColor = Color.Orange,
	headerColor = Color.Gray6,
	textHeaderColor = Color.Orange,
	scrollColor = Color.Gray3,
	scrollBgColor = Color.Gray6,
}

local row_sep = 3

function chListCreate( listName, x, y, w, h, showCount, colSizes )
	chList = {
		name=listName
		, visible_columns={"name", "channel"}
		, index_key="channelID"
		, headers={'Nombre del canal','Nro.'}
		, alignments={Alignment.vCenter_hLeft, Alignment.vCenter_hCenter, Alignment.vCenter_hCenter, Alignment.vCenter_hCenter}
		, columns_sizes = colSizes
		, show_count=showCount
		, lastChannelSelected=0

		, list = list:new( x, y, w, h, row_sep, chanelListColorTable )
	}

	if #colSizes == 4 then
		table.insert(chList.headers, 'Fav.')
		table.insert(chList.headers, 'Bloq.')

		table.insert(chList.visible_columns, 'favorite')
		table.insert(chList.visible_columns, 'blocked')

		list:mapColumn(chList.list, 2, "1", "img:channels_list_fav.png", "img:channels_list_fav.png")
		list:mapColumn(chList.list, 3, "1", "img:channels_list_bloq.png", "img:channels_list_bloq.png")
	end

	list:setSelected( chList.list, 0)
	list:setVisibleRows(chList.list, chList.show_count)

	for i=1, #chList.columns_sizes - 1 do
		chList.columns_sizes[i] = chList.columns_sizes[i] - row_sep
	end

	list:setCellWidths( chList.list, chList.columns_sizes)
	list:setCellHeaders(chList.list, chList.headers);
	list:setCellAlignments(chList.list, chList.alignments)
	list:scrollImages(chList.list, "arrow_up.png", "arrow_down.png")

	return chList
end

function chListSelectChannel( chlist, channel )
	list:setSelected( chlist.list, channel)
end

function chListSetChannels( chlist, chs )
	list:setValues( chlist.list, chlist.visible_columns, chlist.index_key, chs )
end

function chListRefresh( chlist )
	list:updateScrollbar( chlist.list, TRUE )
end

function chListAddChannel( chlist, ch )
	list:addRow( chlist.list, chlist.visible_columns, chlist.index_key, ch )
end

function chListUpdateChannel( chlist, ch, row )
	local row = row or list:selected( chlist.list )
	list:updateRow( chlist.list, row, chlist.visible_columns, chlist.index_key, ch )
end

function chListCurrentChannel( chlist )
	return list:getCurrentValue( chlist.list )
end

function chListSelectedIndex( chlist )
	return list:selected( chlist.list )
end

function chListRowCount( chList )
	return list:getRowCount( chList.list )
end

