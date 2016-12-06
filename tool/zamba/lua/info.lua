local banish = require "banish_mixin"
local log = require "log"
require "basewidget"

------------------------------------------------------------
-- ******************** ZAPPING INFO ******************** --
------------------------------------------------------------
local infoColorTable = {
			bgColor = Color.Blue,
			bgLabelColor = Color.Blue,
			textColor = Color.White,
			selectedTextColor = Color.Black
			}

local extInfoColorTable = {
			bgColor = Color.DarkBlue,
			textColor = Color.White,
			selectedTextColor = Color.Black
			}

local ZappingInfo            =  {}
ZappingInfo.X                =  85
ZappingInfo.Y                = 403
ZappingInfo.W                = 550
ZappingInfo.H                = 115
ZappingInfo.BORDER_W         =   4

--	Date Time
ZappingInfo.DATE_TIME_X =  16
ZappingInfo.DATE_TIME_Y =  10
ZappingInfo.DATE_TIME_W = 516
ZappingInfo.DATE_TIME_H =  26
ZappingInfo.dateTime    = label:new(ZappingInfo.DATE_TIME_X
								  , ZappingInfo.DATE_TIME_Y
								  , ZappingInfo.DATE_TIME_W
								  , ZappingInfo.DATE_TIME_H
				  , menuColorTable
								  , ""
								  , 15 -- Font Size
--                                , Alignment.vCenter_hRight)
								  , Alignment.vTop_hRight)
--	Logo
ZappingInfo.LOGO_WINDOW_X = 17
ZappingInfo.LOGO_WINDOW_Y = 39
ZappingInfo.LOGO_WINDOW_W = 60
ZappingInfo.LOGO_WINDOW_H = 55
ZappingInfo.logoWindow   = window:new(ZappingInfo.LOGO_WINDOW_X
									, ZappingInfo.LOGO_WINDOW_Y
									, ZappingInfo.LOGO_WINDOW_W
									, ZappingInfo.LOGO_WINDOW_H
					, widgetZIndex
									, infoColorTable)
ZappingInfo.LOGO_X    =  8
ZappingInfo.LOGO_Y    =  5
ZappingInfo.LOGO_W    = 45
ZappingInfo.LOGO_H    = 45
ZappingInfo.LOGO_PATH = "INFO_logoCanal.png"
ZappingInfo.logo      = image:new(ZappingInfo.LOGO_X
								, ZappingInfo.LOGO_Y
								, ZappingInfo.LOGO_W
								, ZappingInfo.LOGO_H
				, widgetZIndex
				, infoColorTable
								, ZappingInfo.LOGO_PATH)

--	Full Name
ZappingInfo.FULL_NAME_X =  80
ZappingInfo.FULL_NAME_Y =  39
ZappingInfo.FULL_NAME_W = 261
ZappingInfo.FULL_NAME_H =  30
ZappingInfo.fullName    = textarea:new(ZappingInfo.FULL_NAME_X
									 , ZappingInfo.FULL_NAME_Y
									 , ZappingInfo.FULL_NAME_W
									 , ZappingInfo.FULL_NAME_H
									 , infoColorTable
									 , ""
									 , 17 -- Font Size
									 , 1  -- Lines
									 , 4) -- Margin
--	Flags
ZappingInfo.FLAG_WINDOW_X    = 344
ZappingInfo.FLAG_WINDOW_Y    =  39
ZappingInfo.FLAG_WINDOW_W    =  45
ZappingInfo.FLAG_WINDOW_H    =  30
ZappingInfo.favoriteWindow   = window:new(ZappingInfo.FLAG_WINDOW_X
										, ZappingInfo.FLAG_WINDOW_Y
										, ZappingInfo.FLAG_WINDOW_W
										, ZappingInfo.FLAG_WINDOW_H
					, widgetZIndex
										, infoColorTable)
ZappingInfo.favoriteImage    = image:new(10, 3, 24, 20, widgetZIndex, infoColorTable, "Canales_Favorito_Blanco.png")
ZappingInfo.blockedWindow    = window:new(ZappingInfo.FLAG_WINDOW_X + ZappingInfo.FLAG_WINDOW_W + 3
										, ZappingInfo.FLAG_WINDOW_Y
										, ZappingInfo.FLAG_WINDOW_W
										, ZappingInfo.FLAG_WINDOW_H
					, widgetZIndex
										, infoColorTable)
ZappingInfo.blockedImage     = image:new(10, 3, 24, 20, widgetZIndex, infoColorTable, "Canales_Bloquear_Blanco.png")

--	Parental Control
ZappingInfo.PARENTAL_WINDOW_X = ZappingInfo.FLAG_WINDOW_X + (ZappingInfo.FLAG_WINDOW_W + 3) * 2
ZappingInfo.PARENTAL_WINDOW_Y = 39
ZappingInfo.PARENTAL_WINDOW_W = 45
ZappingInfo.PARENTAL_WINDOW_H = 30
ZappingInfo.parentalContent   = label:new(ZappingInfo.PARENTAL_WINDOW_X
					, ZappingInfo.PARENTAL_WINDOW_Y
					, ZappingInfo.PARENTAL_WINDOW_W
					, ZappingInfo.PARENTAL_WINDOW_H
					, infoColorTable
					, ""
					, 14
--                                      , Alignment.vCenter_hCenter
					, Alignment.vTop_hCenter)
ZappingInfo.parentalAge       = label:new(ZappingInfo.PARENTAL_WINDOW_X + ZappingInfo.PARENTAL_WINDOW_W + 3
										, ZappingInfo.PARENTAL_WINDOW_Y
										, ZappingInfo.PARENTAL_WINDOW_W
										, ZappingInfo.PARENTAL_WINDOW_H
					, infoColorTable
										, ""
										, 14
--                                      , Alignment.vCenter_hCenter
									, Alignment.vTop_hCenter)

-- Present Name
ZappingInfo.PRESENT_NAME_X = ZappingInfo.LOGO_WINDOW_X + ZappingInfo.LOGO_WINDOW_W + 3
ZappingInfo.PRESENT_NAME_Y = ZappingInfo.FULL_NAME_Y + ZappingInfo.FULL_NAME_H + 3
ZappingInfo.PRESENT_NAME_W = 453
ZappingInfo.PRESENT_NAME_H = 22
ZappingInfo.presentName    = textarea:new( ZappingInfo.PRESENT_NAME_X
					 , ZappingInfo.PRESENT_NAME_Y
					 , ZappingInfo.PRESENT_NAME_W
					 , ZappingInfo.PRESENT_NAME_H
					 , infoColorTable
					 , ""
					 , 14
					 , 1
					 , 4
					 , Alignment.vBottom_hLeft)
--	Zapping Info
local zappingInfo = window:new(ZappingInfo.X
							 , ZappingInfo.Y
							 , ZappingInfo.W
							 , ZappingInfo.H
				 , widgetZIndex
				 , menuColorTable
							 , ZappingInfo.BORDER_W)
window:addChild(zappingInfo, ZappingInfo.dateTime)
window:addChild(zappingInfo, ZappingInfo.logoWindow)
window:addChild(ZappingInfo.logoWindow, ZappingInfo.logo)
window:addChild(zappingInfo, ZappingInfo.fullName)
window:addChild(zappingInfo, ZappingInfo.favoriteWindow)
window:addChild(ZappingInfo.favoriteWindow, ZappingInfo.favoriteImage)
window:addChild(zappingInfo, ZappingInfo.blockedWindow)
window:addChild(ZappingInfo.blockedWindow, ZappingInfo.blockedImage)
window:addChild(zappingInfo, ZappingInfo.parentalContent)
window:addChild(zappingInfo, ZappingInfo.parentalAge)
window:addChild(zappingInfo, ZappingInfo.presentName)
wgt:setVisible(zappingInfo, FALSE)
wgt:fixOffsets(zappingInfo)

------------------------------------------------------------
-- ******************** NORMAL  INFO ******************** --
------------------------------------------------------------
local NormalInfo            =  {}
NormalInfo.X                =  85
NormalInfo.Y                = 378
NormalInfo.W                = 550
NormalInfo.H                = 140
NormalInfo.BORDER_W         =   4

--	Date Time
NormalInfo.DATE_TIME_X =  16
NormalInfo.DATE_TIME_Y =  10
NormalInfo.DATE_TIME_W = 516
NormalInfo.DATE_TIME_H =  26
NormalInfo.dateTime    = label:new(NormalInfo.DATE_TIME_X
								  , NormalInfo.DATE_TIME_Y
								  , NormalInfo.DATE_TIME_W
								  , NormalInfo.DATE_TIME_H
				  , menuColorTable
								  , ""
								  , 15 -- Font Size
--                                   , Alignment.vCenter_hRight)
								  , Alignment.vTop_hRight)
--	Logo
NormalInfo.LOGO_WINDOW_X = 17
NormalInfo.LOGO_WINDOW_Y = 39
NormalInfo.LOGO_WINDOW_W = 60
NormalInfo.LOGO_WINDOW_H = 55
NormalInfo.logoWindow   = window:new(NormalInfo.LOGO_WINDOW_X
									, NormalInfo.LOGO_WINDOW_Y
									, NormalInfo.LOGO_WINDOW_W
									, NormalInfo.LOGO_WINDOW_H
					, widgetZIndex
									, infoColorTable)
NormalInfo.LOGO_X    =  8
NormalInfo.LOGO_Y    =  5
NormalInfo.LOGO_W    = 45
NormalInfo.LOGO_H    = 45
NormalInfo.LOGO_PATH = "INFO_logoCanal.png"
NormalInfo.logo      = image:new(NormalInfo.LOGO_X
								, NormalInfo.LOGO_Y
								, NormalInfo.LOGO_W
								, NormalInfo.LOGO_H
				, widgetZIndex
				, infoColorTable
								, NormalInfo.LOGO_PATH)

--	Full Name
NormalInfo.FULL_NAME_X =  80
NormalInfo.FULL_NAME_Y =  39
NormalInfo.FULL_NAME_W = 251
NormalInfo.FULL_NAME_H =  55
NormalInfo.fullName    = textarea:new(NormalInfo.FULL_NAME_X
									 , NormalInfo.FULL_NAME_Y
									 , NormalInfo.FULL_NAME_W
									 , NormalInfo.FULL_NAME_H
									 , infoColorTable
									 , ""
									 , 17 -- Font Size
									 , 1  -- Lines
									 , 4 -- Margin
--                                     , Alignment.vCenter_hLeft)
									 , Alignment.vTop_hLeft)


--	Flags
NormalInfo.FLAG_WINDOW_X    = 334
NormalInfo.FLAG_WINDOW_Y    =  39
NormalInfo.FLAG_WINDOW_W    =  27
NormalInfo.FLAG_WINDOW_H    =  26

NormalInfo.CC = label:new(NormalInfo.FLAG_WINDOW_X
						, NormalInfo.FLAG_WINDOW_Y
						, NormalInfo.FLAG_WINDOW_W
						, NormalInfo.FLAG_WINDOW_H
			, infoColorTable
						, ""
						, 15 -- Font Size
--                      , Alignment.vCenter_hCenter
						, Alignment.vTop_hCenter)



NormalInfo.favoriteWindow   = window:new(NormalInfo.FLAG_WINDOW_X + NormalInfo.FLAG_WINDOW_W + 3
										, NormalInfo.FLAG_WINDOW_Y
										, NormalInfo.FLAG_WINDOW_W
										, NormalInfo.FLAG_WINDOW_H
					, widgetZIndex
										, infoColorTable)
NormalInfo.favoriteImage    = image:new(1, 3, 24, 20, widgetZIndex, infoColorTable, "Canales_Favorito_Blanco.png")
NormalInfo.blockedWindow    = window:new(NormalInfo.FLAG_WINDOW_X + (NormalInfo.FLAG_WINDOW_W + 3) * 2
										, NormalInfo.FLAG_WINDOW_Y
										, NormalInfo.FLAG_WINDOW_W
										, NormalInfo.FLAG_WINDOW_H
					, widgetZIndex
										, infoColorTable)
NormalInfo.blockedImage     = image:new(1, 3, 24, 20, widgetZIndex, infoColorTable, "Canales_Bloquear_Blanco.png")

--	Parental Control
NormalInfo.PARENTAL_WINDOW_X = NormalInfo.FLAG_WINDOW_X + (NormalInfo.FLAG_WINDOW_W + 3) * 3
NormalInfo.PARENTAL_WINDOW_Y = 39
NormalInfo.PARENTAL_WINDOW_W = 31
NormalInfo.PARENTAL_WINDOW_H = 26
NormalInfo.parentalContent   = label:new(NormalInfo.PARENTAL_WINDOW_X
										, NormalInfo.PARENTAL_WINDOW_Y
										, NormalInfo.PARENTAL_WINDOW_W
										, NormalInfo.PARENTAL_WINDOW_H
					, infoColorTable
										, ""
										, 14
--                                      , Alignment.vCenter_hCenter
										, Alignment.vTop_hCenter)
NormalInfo.parentalAge       = label:new(NormalInfo.PARENTAL_WINDOW_X + NormalInfo.PARENTAL_WINDOW_W + 3
										, NormalInfo.PARENTAL_WINDOW_Y
										, NormalInfo.PARENTAL_WINDOW_W
										, NormalInfo.PARENTAL_WINDOW_H
					, infoColorTable
										, ""
										, 14
--                                      , Alignment.vCenter_hCenter
										, Alignment.vTop_hCenter)
--	Signal
NormalInfo.SIGNAL_WINDOW_X = 492
NormalInfo.SIGNAL_WINDOW_Y =  39
NormalInfo.SIGNAL_WINDOW_W =  41
NormalInfo.SIGNAL_WINDOW_H =  26
NormalInfo.signalWindow    = window:new(NormalInfo.SIGNAL_WINDOW_X
									  , NormalInfo.SIGNAL_WINDOW_Y
									  , NormalInfo.SIGNAL_WINDOW_W
									  , NormalInfo.SIGNAL_WINDOW_H
					  , widgetZIndex
									  , infoColorTable)
NormalInfo.signalImage     = image:new(2,4, 37,17, widgetZIndex, infoColorTable, "INFO_antenita2.png")

--	Interactivity
NormalInfo.I_WINDOW_X    = 334
NormalInfo.I_WINDOW_Y    =  68
NormalInfo.I_WINDOW_W    =  27
NormalInfo.I_WINDOW_H    =  26

NormalInfo.interactivityWindow   = window:new(NormalInfo.I_WINDOW_X
											, NormalInfo.I_WINDOW_Y
											, NormalInfo.I_WINDOW_W
											, NormalInfo.I_WINDOW_H
						, widgetZIndex
											, infoColorTable)
NormalInfo.interactivityImage    = image:new(3, 3, 21, 21, widgetZIndex, infoColorTable, "i.png" )

-- Audio
NormalInfo.AUDIO_WINDOW_X = NormalInfo.I_WINDOW_X + NormalInfo.I_WINDOW_W + 3
NormalInfo.AUDIO_WINDOW_Y =  68
NormalInfo.AUDIO_WINDOW_W =  57
NormalInfo.AUDIO_WINDOW_H =  26
NormalInfo.audio          = label:new(NormalInfo.AUDIO_WINDOW_X
									, NormalInfo.AUDIO_WINDOW_Y
					, NormalInfo.AUDIO_WINDOW_W
									, NormalInfo.AUDIO_WINDOW_H
					, infoColorTable
									, ""
									, 14
--                                  , Alignment.vCenter_hCenter
									, Alignment.vTop_hCenter)

-- Aspect
NormalInfo.ASPECT_WINDOW_W =  50
NormalInfo.aspect         = label:new(NormalInfo.AUDIO_WINDOW_X + NormalInfo.AUDIO_WINDOW_W + 3
									, NormalInfo.AUDIO_WINDOW_Y
									, NormalInfo.ASPECT_WINDOW_W
									, NormalInfo.AUDIO_WINDOW_H
					, infoColorTable
									, ""
									, 14
--                                  , Alignment.vCenter_hCenter
									, Alignment.vTop_hCenter)

-- Video
NormalInfo.VIDEO_WINDOW_W =  57
NormalInfo.resolution     = label:new(NormalInfo.AUDIO_WINDOW_X + NormalInfo.AUDIO_WINDOW_W + 3 + NormalInfo.ASPECT_WINDOW_W + 3
									, NormalInfo.AUDIO_WINDOW_Y
									, NormalInfo.VIDEO_WINDOW_W
									, NormalInfo.AUDIO_WINDOW_H
					, infoColorTable
									, ""
									, 14
--                                  , Alignment.vCenter_hCenter
									, Alignment.vTop_hCenter)

-- Present Name
NormalInfo.PRESENT_NAME_X = 17
NormalInfo.PRESENT_NAME_Y = 98
NormalInfo.PRESENT_NAME_W = 517
NormalInfo.PRESENT_NAME_H = 22
NormalInfo.presentName    = textarea:new(NormalInfo.PRESENT_NAME_X
					 , NormalInfo.PRESENT_NAME_Y
					 , NormalInfo.PRESENT_NAME_W
					 , NormalInfo.PRESENT_NAME_H
					 , infoColorTable
					 , ""
					 , 14
					 , 1
					 , 4
					 , Alignment.vCenter_hLeft)
--	Normal Info

local normalInfo = window:new(NormalInfo.X
							 , NormalInfo.Y
							 , NormalInfo.W
							 , NormalInfo.H
				 , widgetZIndex
				 , menuColorTable
							 , NormalInfo.BORDER_W)
window:addChild(normalInfo, NormalInfo.dateTime)
window:addChild(normalInfo, NormalInfo.logoWindow)
window:addChild(NormalInfo.logoWindow, NormalInfo.logo)
window:addChild(normalInfo, NormalInfo.fullName)

window:addChild(normalInfo, NormalInfo.interactivityWindow)
window:addChild(NormalInfo.interactivityWindow, NormalInfo.interactivityImage)
window:addChild(normalInfo, NormalInfo.CC)
window:addChild(normalInfo, NormalInfo.favoriteWindow)
window:addChild(NormalInfo.favoriteWindow, NormalInfo.favoriteImage)
window:addChild(normalInfo, NormalInfo.blockedWindow)
window:addChild(NormalInfo.blockedWindow, NormalInfo.blockedImage)
window:addChild(normalInfo, NormalInfo.signalWindow)
window:addChild(NormalInfo.signalWindow, NormalInfo.signalImage)
window:addChild(normalInfo, NormalInfo.parentalContent)
window:addChild(normalInfo, NormalInfo.parentalAge)
window:addChild(normalInfo, NormalInfo.audio)
window:addChild(normalInfo, NormalInfo.aspect)
window:addChild(normalInfo, NormalInfo.resolution)
window:addChild(normalInfo, NormalInfo.presentName)
wgt:setVisible(normalInfo, FALSE)
wgt:fixOffsets(normalInfo)

------------------------------------------------------------
-- ******************* EXTENDED  INFO ******************* --
------------------------------------------------------------
local ExtendedInfo            =  {}
ExtendedInfo.X                =  85
ExtendedInfo.Y                = 301
ExtendedInfo.W                = 550
ExtendedInfo.H                = 217
ExtendedInfo.BORDER_W         =   4

--	Date Time
ExtendedInfo.DATE_TIME_X =  16
ExtendedInfo.DATE_TIME_Y =  10
ExtendedInfo.DATE_TIME_W = 516
ExtendedInfo.DATE_TIME_H =  26
ExtendedInfo.dateTime    = label:new(ExtendedInfo.DATE_TIME_X
								  , ExtendedInfo.DATE_TIME_Y
								  , ExtendedInfo.DATE_TIME_W
								  , ExtendedInfo.DATE_TIME_H
				  , menuColorTable
								  , ""
								  , 15 -- Font Size
--                                , Alignment.vCenter_hRight)
								  , Alignment.vTop_hRight)
--	Logo
ExtendedInfo.LOGO_WINDOW_X = 17
ExtendedInfo.LOGO_WINDOW_Y = 39
ExtendedInfo.LOGO_WINDOW_W = 60
ExtendedInfo.LOGO_WINDOW_H = 55
ExtendedInfo.logoWindow   = window:new(ExtendedInfo.LOGO_WINDOW_X
									, ExtendedInfo.LOGO_WINDOW_Y
									, ExtendedInfo.LOGO_WINDOW_W
									, ExtendedInfo.LOGO_WINDOW_H
					, widgetZIndex
									, infoColorTable)
ExtendedInfo.LOGO_X    =  8
ExtendedInfo.LOGO_Y    =  5
ExtendedInfo.LOGO_W    = 45
ExtendedInfo.LOGO_H    = 45
ExtendedInfo.LOGO_PATH = "INFO_logoCanal.png"
ExtendedInfo.logo      = image:new(ExtendedInfo.LOGO_X
								, ExtendedInfo.LOGO_Y
								, ExtendedInfo.LOGO_W
								, ExtendedInfo.LOGO_H
				, widgetZIndex
				, infoColorTable
								, ExtendedInfo.LOGO_PATH)

--	Full Name
ExtendedInfo.FULL_NAME_X =  80
ExtendedInfo.FULL_NAME_Y =  39
ExtendedInfo.FULL_NAME_W = 251
ExtendedInfo.FULL_NAME_H =  55
ExtendedInfo.fullName    = textarea:new(ExtendedInfo.FULL_NAME_X
									 , ExtendedInfo.FULL_NAME_Y
									 , ExtendedInfo.FULL_NAME_W
									 , ExtendedInfo.FULL_NAME_H
									 , infoColorTable
									 , ""
									 , 17 -- Font Size
									 , 1  -- Lines
									 , 4 -- Margin
--                                     , Alignment.vCenter_hLeft)
									, Alignment.vTop_hLeft)

--	Flags
ExtendedInfo.FLAG_WINDOW_X    = 334
ExtendedInfo.FLAG_WINDOW_Y    =  39
ExtendedInfo.FLAG_WINDOW_W    =  27
ExtendedInfo.FLAG_WINDOW_H    =  26

ExtendedInfo.CC = label:new(ExtendedInfo.FLAG_WINDOW_X
						  , ExtendedInfo.FLAG_WINDOW_Y
						  , ExtendedInfo.FLAG_WINDOW_W
						  , ExtendedInfo.FLAG_WINDOW_H
			  , infoColorTable
						  , ""
						  , 15 -- Font Size
--                        , Alignment.vCenter_hCenter
						  , Alignment.vTop_hCenter)

ExtendedInfo.favoriteWindow   = window:new(ExtendedInfo.FLAG_WINDOW_X + ExtendedInfo.FLAG_WINDOW_W + 3
										, ExtendedInfo.FLAG_WINDOW_Y
										, ExtendedInfo.FLAG_WINDOW_W
										, ExtendedInfo.FLAG_WINDOW_H
					, widgetZIndex
										, infoColorTable)
ExtendedInfo.favoriteImage    = image:new(1, 3, 24, 20, widgetZIndex, infoColorTable, "Canales_Favorito_Blanco.png")
ExtendedInfo.blockedWindow    = window:new(ExtendedInfo.FLAG_WINDOW_X + (ExtendedInfo.FLAG_WINDOW_W + 3) * 2
										, ExtendedInfo.FLAG_WINDOW_Y
										, ExtendedInfo.FLAG_WINDOW_W
										, ExtendedInfo.FLAG_WINDOW_H
					, widgetZIndex
										, infoColorTable)
ExtendedInfo.blockedImage     = image:new(1, 3, 24, 20, widgetZIndex, infoColorTable, "Canales_Bloquear_Blanco.png")

--	Parental Control
ExtendedInfo.PARENTAL_WINDOW_X = ExtendedInfo.FLAG_WINDOW_X + (ExtendedInfo.FLAG_WINDOW_W + 3) * 3
ExtendedInfo.PARENTAL_WINDOW_Y = 39
ExtendedInfo.PARENTAL_WINDOW_W = 31
ExtendedInfo.PARENTAL_WINDOW_H = 26
ExtendedInfo.parentalContent   = label:new(ExtendedInfo.PARENTAL_WINDOW_X
										, ExtendedInfo.PARENTAL_WINDOW_Y
										, ExtendedInfo.PARENTAL_WINDOW_W
										, ExtendedInfo.PARENTAL_WINDOW_H
					, infoColorTable
										, ""
										, 14
--                                      , Alignment.vCenter_hCenter
										, Alignment.vTop_hCenter)
ExtendedInfo.parentalAge       = label:new(ExtendedInfo.PARENTAL_WINDOW_X + ExtendedInfo.PARENTAL_WINDOW_W + 3
										, ExtendedInfo.PARENTAL_WINDOW_Y
										, ExtendedInfo.PARENTAL_WINDOW_W
										, ExtendedInfo.PARENTAL_WINDOW_H
					, infoColorTable
										, ""
										, 14
--                                      , Alignment.vCenter_hCenter
									, Alignment.vTop_hCenter)
--	Signal
ExtendedInfo.SIGNAL_WINDOW_X = 492
ExtendedInfo.SIGNAL_WINDOW_Y =  39
ExtendedInfo.SIGNAL_WINDOW_W =  41
ExtendedInfo.SIGNAL_WINDOW_H =  26
ExtendedInfo.signalWindow    = window:new(ExtendedInfo.SIGNAL_WINDOW_X
									  , ExtendedInfo.SIGNAL_WINDOW_Y
									  , ExtendedInfo.SIGNAL_WINDOW_W
									  , ExtendedInfo.SIGNAL_WINDOW_H
					  , widgetZIndex
									  , infoColorTable)
ExtendedInfo.signalImage     = image:new(2,4, 37,17, widgetZIndex, infoColorTable, "INFO_antenita2.png")

--	Interactivity
ExtendedInfo.I_WINDOW_X    = 334
ExtendedInfo.I_WINDOW_Y    =  68
ExtendedInfo.I_WINDOW_W    =  27
ExtendedInfo.I_WINDOW_H    =  26
ExtendedInfo.interactivityWindow   = window:new(ExtendedInfo.I_WINDOW_X
											  , ExtendedInfo.I_WINDOW_Y
											  , ExtendedInfo.I_WINDOW_W
											  , ExtendedInfo.I_WINDOW_H
						  , widgetZIndex
											  , infoColorTable)
ExtendedInfo.interactivityImage    = image:new(3, 3, 21, 21, widgetZIndex, infoColorTable, "i.png")

-- Audio
ExtendedInfo.AUDIO_WINDOW_X = ExtendedInfo.I_WINDOW_X + ExtendedInfo.I_WINDOW_W + 3
ExtendedInfo.AUDIO_WINDOW_Y =  68
ExtendedInfo.AUDIO_WINDOW_W =  57
ExtendedInfo.AUDIO_WINDOW_H =  26
ExtendedInfo.audio          = label:new(ExtendedInfo.AUDIO_WINDOW_X
									, ExtendedInfo.AUDIO_WINDOW_Y
					, ExtendedInfo.AUDIO_WINDOW_W
									, ExtendedInfo.AUDIO_WINDOW_H
					, infoColorTable
									, ""
									, 14
--                                  , Alignment.vCenter_hCenter
									, Alignment.vTop_hCenter)

-- Aspect
ExtendedInfo.ASPECT_WINDOW_W =  50
ExtendedInfo.aspect         = label:new(ExtendedInfo.AUDIO_WINDOW_X + ExtendedInfo.AUDIO_WINDOW_W + 3
									, ExtendedInfo.AUDIO_WINDOW_Y
					, ExtendedInfo.ASPECT_WINDOW_W
									, ExtendedInfo.AUDIO_WINDOW_H
					, infoColorTable
									, ""
									, 14
--                                  , Alignment.vCenter_hCenter
									, Alignment.vTop_hCenter)

-- Resolution
ExtendedInfo.VIDEO_WINDOW_W =  57
ExtendedInfo.resolution     = label:new(ExtendedInfo.AUDIO_WINDOW_X + ExtendedInfo.AUDIO_WINDOW_W + 3 + ExtendedInfo.ASPECT_WINDOW_W + 3
									, ExtendedInfo.AUDIO_WINDOW_Y
					, ExtendedInfo.VIDEO_WINDOW_W
									, ExtendedInfo.AUDIO_WINDOW_H
					, infoColorTable
									, ""
									, 14
--                                  , Alignment.vCenter_hCenter
									, Alignment.vTop_hCenter)

-- Present Name
ExtendedInfo.PRESENT_NAME_X = 17
ExtendedInfo.PRESENT_NAME_Y = 97
ExtendedInfo.PRESENT_NAME_W = 517
ExtendedInfo.PRESENT_NAME_H = 22
ExtendedInfo.presentName    = textarea:new(ExtendedInfo.PRESENT_NAME_X
					 , ExtendedInfo.PRESENT_NAME_Y
					 , ExtendedInfo.PRESENT_NAME_W
					 , ExtendedInfo.PRESENT_NAME_H
					 , extInfoColorTable
					 , ""
					 , 14
					 , 1
					 , 4
					 , Alignment.vCenter_hLeft )
--	Present Description
ExtendedInfo.PRESENT_DESC_X =  17
ExtendedInfo.PRESENT_DESC_Y = 122
ExtendedInfo.PRESENT_DESC_W = 517
ExtendedInfo.PRESENT_DESC_H = 44
ExtendedInfo.presentDesc    = textarea:new(ExtendedInfo.PRESENT_DESC_X
										 , ExtendedInfo.PRESENT_DESC_Y
										 , ExtendedInfo.PRESENT_DESC_W
										 , ExtendedInfo.PRESENT_DESC_H
										 , extInfoColorTable
										 , ""
										 , 14 -- Font Size
										 , 2  -- Lines
										 , 4 -- Margin
--                                       , Alignment.vCenter_hLeft)
									 , Alignment.vTop_hLeft)

--	STB Config
--	Text
ExtendedInfo.CONFIG_TEXT_X = ExtendedInfo.PRESENT_NAME_X
ExtendedInfo.CONFIG_TEXT_Y = 169
ExtendedInfo.CONFIG_TEXT_W = 314 + 30
ExtendedInfo.CONFIG_TEXT_H =  26
ExtendedInfo.configText = label:new(ExtendedInfo.CONFIG_TEXT_X
								  , ExtendedInfo.CONFIG_TEXT_Y
								  , ExtendedInfo.CONFIG_TEXT_W
								  , ExtendedInfo.CONFIG_TEXT_H
				  , infoColorTable
								  , "Config. Set Top Box (MENU = Modificar)"
								  , 14
								  , Alignment.vCenter_hLeft
--                                , Alignment.vBottom_hLeft
								  , 4)

-- STB Audio
ExtendedInfo.STB_AUDIO_WINDOW_X = 334 + 30
ExtendedInfo.STB_AUDIO_WINDOW_Y = ExtendedInfo.AUDIO_WINDOW_Y + ExtendedInfo.AUDIO_WINDOW_H + 3 + ExtendedInfo.PRESENT_NAME_H + 3 + ExtendedInfo.PRESENT_DESC_H + 3
ExtendedInfo.STB_AUDIO_WINDOW_W =  ExtendedInfo.AUDIO_WINDOW_W
ExtendedInfo.STB_AUDIO_WINDOW_H =  ExtendedInfo.AUDIO_WINDOW_H
ExtendedInfo.stbAudio           = label:new(ExtendedInfo.STB_AUDIO_WINDOW_X
										  , ExtendedInfo.STB_AUDIO_WINDOW_Y
										  , ExtendedInfo.STB_AUDIO_WINDOW_W
										  , ExtendedInfo.STB_AUDIO_WINDOW_H
					  , infoColorTable
										  , ""
										  , 14
--                                        , Alignment.vCenter_hCenter
									  , Alignment.vTop_hCenter)

-- STB Aspect
-- ExtendedInfo.STB_ASPECT_WINDOW_W = 57
ExtendedInfo.stbAspect = label:new(ExtendedInfo.STB_AUDIO_WINDOW_X + ExtendedInfo.AUDIO_WINDOW_W + 3
								 , ExtendedInfo.STB_AUDIO_WINDOW_Y
								 , ExtendedInfo.ASPECT_WINDOW_W
								 , ExtendedInfo.STB_AUDIO_WINDOW_H
				 , infoColorTable
								 , ""
								 , 14
--                               , Alignment.vCenter_hCenter
								 , Alignment.vTop_hCenter)

-- Resolution
-- ExtendedInfo.STB_VIDEO_WINDOW_W =  57
ExtendedInfo.stbResolution = label:new(ExtendedInfo.STB_AUDIO_WINDOW_X + ExtendedInfo.STB_AUDIO_WINDOW_W + 3 + ExtendedInfo.ASPECT_WINDOW_W + 3
									 , ExtendedInfo.STB_AUDIO_WINDOW_Y
									 , ExtendedInfo.VIDEO_WINDOW_W
									 , ExtendedInfo.STB_AUDIO_WINDOW_H
					 , infoColorTable
									 , ""
									 , 14
--                                   , Alignment.vCenter_hCenter
									 , Alignment.vTop_hCenter)


--	Extended Info
local extendedInfo = window:new(ExtendedInfo.X
							  , ExtendedInfo.Y
							  , ExtendedInfo.W
							  , ExtendedInfo.H
				  , widgetZIndex
				  , menuColorTable
							  , ExtendedInfo.BORDER_W)
window:addChild(extendedInfo, ExtendedInfo.dateTime)
window:addChild(extendedInfo, ExtendedInfo.logoWindow)
window:addChild(ExtendedInfo.logoWindow, ExtendedInfo.logo)
window:addChild(extendedInfo, ExtendedInfo.fullName)

window:addChild(extendedInfo, ExtendedInfo.interactivityWindow)
window:addChild(ExtendedInfo.interactivityWindow, ExtendedInfo.interactivityImage)
window:addChild(extendedInfo, ExtendedInfo.CC)

window:addChild(extendedInfo, ExtendedInfo.favoriteWindow)
window:addChild(ExtendedInfo.favoriteWindow, ExtendedInfo.favoriteImage)
window:addChild(extendedInfo, ExtendedInfo.blockedWindow)
window:addChild(ExtendedInfo.blockedWindow, ExtendedInfo.blockedImage)
window:addChild(extendedInfo, ExtendedInfo.signalWindow)
window:addChild(ExtendedInfo.signalWindow, ExtendedInfo.signalImage)
window:addChild(extendedInfo, ExtendedInfo.parentalContent)
window:addChild(extendedInfo, ExtendedInfo.parentalAge)
window:addChild(extendedInfo, ExtendedInfo.audio)
window:addChild(extendedInfo, ExtendedInfo.aspect)
window:addChild(extendedInfo, ExtendedInfo.resolution)
window:addChild(extendedInfo, ExtendedInfo.presentName)
window:addChild(extendedInfo, ExtendedInfo.presentDesc)
window:addChild(extendedInfo, ExtendedInfo.configText)
window:addChild(extendedInfo, ExtendedInfo.stbAudio)
window:addChild(extendedInfo, ExtendedInfo.stbAspect)
window:addChild(extendedInfo, ExtendedInfo.stbResolution)
wgt:setVisible(extendedInfo, FALSE)
wgt:fixOffsets(extendedInfo)

function getSignalImage(value)
	-- print( "[lua] getSignalImage, Value=" .. value )
	if (value == 1) then
		return "INFO_antenita_mala.png" -- baja
	elseif (value == 2) then
		return "INFO_antenita_media.png" -- media
	elseif (value == 3) then
		return "INFO_antenita_buena.png" -- alta
	else
		return "INFO_antenita2.png" -- nada
	end
end

function getLogo( str )
	if (str == "") then
		return "INFO_logoCanal.png"
	end
	return str
end

function fixUnknown( str )
	if (str == "Unknown") then
		str = ""
	end
	return str
end

function fixResolution( str )
	-- 720x576i@60hz
	str = fixUnknown(str)
	pos1=str:find("x")
	pos2=str:find("@")
	if (pos1 ~= nil and pos2 ~= nil) then
		str=str:sub(pos1+1,pos2-2)
	end
	return str
end

local ChannelInfo_Zapping  = 0
local ChannelInfo_Normal   = 1
local ChannelInfo_Extended = 2

Info = {
	level = ChannelInfo_Zapping
	, renderingChannel = nil
}

Info = BaseWidget:new( Info, "Info", false)

function renderZappingInfo(ix)
	Info.renderInfo(ChannelInfo_Zapping, ix)
end

function Info.renderChannelInfo()
	local ix = Info.renderingChannel
	if ix >= 0 then

		-- data
		local info = channels.getInfo(ix)
		local chFullName = info['channel'] .. ' - ' .. info['name']
		local b,_,f,_    = chooseFlagsImages( info )
		local iCC        = channel.haveSubtitle()
		local iAudio     = fixUnknown(channel.getAudio())
		local iAspect    = fixUnknown(channel.getAspect())
		local iRes       = fixResolution(channel.getResolution())
		local pAge       = info.parentalAge
		local pContent   = info.parentalContent
		local currentTime = mainWindow.getTime('%d/%m/%Y - %H:%Mhs')

		_,jAudio = mixer.getAudioChannel()
		jAudio=fixUnknown(jAudio)
		_,jAspect = display.getAspectMode()
		jAspect=fixUnknown(jAspect)
		_,jRes = display.getVideoMode()
		jRes=fixResolution(jRes)

		wgt:close(zappingInfo)
		wgt:close(normalInfo)
		wgt:close(extendedInfo)

		local struct = {}
		local widget = -1
		if (Info.level == ChannelInfo_Zapping) then
			struct = ZappingInfo
			widget = zappingInfo
		else
			if (Info.level == ChannelInfo_Normal) then
				struct = NormalInfo
				widget = normalInfo
			else
				struct = ExtendedInfo
				widget = extendedInfo
				textarea:setText(struct.presentDesc, info.showDescription)
				label:setText(struct.stbAudio,      jAudio)
				label:setText(struct.stbAspect,     jAspect)
				label:setText(struct.stbResolution, jRes)
			end
			image:setImage(struct.signalImage, getSignalImage( 3 ) )
-- 			image:setImage(struct.interactivityImage, getInteractiveImage( middleware.haveApplications() ) )
			wgt:setVisible(struct.interactivityImage, middleware.haveApplications())
			label:setText(struct.CC,         iCC)
			label:setText(struct.audio,      iAudio)
			label:setText(struct.aspect,     iAspect)
			label:setText(struct.resolution, iRes)
		end
		image:setImage(struct.logo, getLogo(info.logo) )
		label:setText(struct.dateTime, currentTime)
		textarea:setText(struct.fullName, chFullName)
		wgt:setVisible(struct.favoriteImage, info.favorite)
		wgt:setVisible(struct.blockedImage, info.blocked)
		label:setText(struct.parentalContent, pContent)
		label:setText(struct.parentalAge, pAge)
		textarea:setText(struct.presentName, info.showName)
		wgt:open(widget)
	end
end

function Info.renderInfo(level, ix)
	Info.renderingChannel = ix
	Info.level = level
	if Info.visible then
		Info.renderChannelInfo()
	else
		openWgt( Info )
	end
end

function Info.onShow()
	log.debug( "Info", "onShow")
	Info.renderChannelInfo()
	return true
end

function Info.onHide()
	wgt:close(zappingInfo)
	wgt:close(normalInfo)
	wgt:close(extendedInfo)

	Info.level = 0
	Info.renderingChannel = nil
	ignoreChKeys(false)
	setVirtualChannel(-1)
	checkUpdates()
	return true
end

function Info.onInfo()
	Info:banish_restart()

	local level = Info.level + 1
	if (level > 2) then
		closeWgt(Info)
	else
		Info.renderInfo(level, channel.current())
	end

	return true
end

-- Add banish behaviour
Info:include(banish)
Info:banishOn(banish_timeout)