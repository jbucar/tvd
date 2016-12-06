-- Framework libs
-- load Core
require 'src.core.Object'
require 'src.core.FiniteStateMachine'
-- load Events
require 'src.config.KeysMap'
require 'src.core.EventManager'
-- load Widgets
require 'src.widgets.NavigationBar'
require 'src.widgets.RemoteImage'
require 'src.widgets.Client'
require 'src.widgets.Tools'
_G.client = Client:new('http://api.prod.cda.dcarsat.com.ar/servicios/api/','3b215471706c796964217d6172705d')
_G.jsonParser = require 'src.widgets.JSON'
_G.jsonParser.assert = Tools.assert
-- Stages
require 'src.stages.MainStage'
require 'src.stages.ItemStage'