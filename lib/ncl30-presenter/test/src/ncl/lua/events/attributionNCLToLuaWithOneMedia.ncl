<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="nclClicks" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">

<head>
	<regionBase>
		<region id="appReg"/>
	</regionBase>

	<descriptorBase>
		<descriptor id="appDesc" region="appReg"  focusIndex="1"/>
	</descriptorBase>

	<connectorBase>
      <causalConnector id="onKeySelectionSet">
        <connectorParam name="keyCode"/> 
        <connectorParam name="var"/>
        <simpleCondition role="onSelection" key="$keyCode"/> 
          <simpleAction role="set" value="$var"/>
      </causalConnector>
	</connectorBase>
</head>

<body>
   
	<port id="entryPoint" component="app"/>

	<media id="app" src="attributionNCLToLua.lua" descriptor="appDesc">
		<property name="luaColor"/>
	</media>

	<media id="settings" type="application/x-ginga-settings" >
		<property name="service.currentKeyMaster" value="1"/>
	</media>
	

    <link xconnector="onKeySelectionSet">
      <bind role="onSelection" component="app"> 
      <bindParam name="keyCode" value="RED"/>
      </bind>
      <bind role="set" component="app" interface="luaColor">
        <bindParam name="var" value="red"/>
      </bind>
    </link>

    <link xconnector="onKeySelectionSet">
      <bind role="onSelection" component="app"> 
      <bindParam name="keyCode" value="GREEN"/>
      </bind>
      <bind role="set" component="app" interface="luaColor">
        <bindParam name="var" value="green"/>
      </bind>
    </link>


    <link xconnector="onKeySelectionSet">
      <bind role="onSelection" component="app"> 
      <bindParam name="keyCode" value="YELLOW"/>
      </bind>
      <bind role="set" component="app" interface="luaColor">
        <bindParam name="var" value="yellow"/>
      </bind>
    </link>

    <link xconnector="onKeySelectionSet">
      <bind role="onSelection" component="app"> 
      <bindParam name="keyCode" value="BLUE"/>
      </bind>
      <bind role="set" component="app" interface="luaColor">
        <bindParam name="var" value="blue"/>
      </bind>
    </link>

</body>

</ncl>
