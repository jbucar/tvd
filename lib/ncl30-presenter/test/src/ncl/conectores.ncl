<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="conectores" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<connectorBase>
			<causalConnector id="onRedSelectionSet">
				<connectorParam name="value"/>
				<simpleCondition key="RED" role="onSelection"/>
				<simpleAction role="set" value="$value" max="unbounded" qualifier="seq"/>
			</causalConnector>
		</connectorBase>
	</head>
</ncl>
