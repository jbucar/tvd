<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="textTest" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg_bg" width="100%" height="100%" left="0%" top="0%" />
			<region id="reg1"  width="40%" height="12%" left="10%" top="6%" />
			<region id="reg2"  width="40%" height="12%" left="10%" top="12%" />
			<region id="reg3"  width="40%" height="12%" left="10%" top="24%" />
			<region id="reg4"  width="40%" height="12%" left="10%" top="36%" />
			<region id="reg5"  width="40%" height="12%" left="10%" top="48%" />
			<region id="reg6"  width="40%" height="12%" left="10%" top="60%" />
			<region id="reg7"  width="40%" height="12%" left="10%" top="72%" />
			<region id="reg8"  width="40%" height="12%" left="10%" top="84%" />

			<region id="reg9"  width="40%" height="12%" left="50%" top="6%" />
			<region id="reg10" width="40%" height="12%" left="50%" top="12%" />
			<region id="reg11" width="40%" height="12%" left="50%" top="24%" />
			<region id="reg12" width="40%" height="12%" left="50%" top="36%" />
			<region id="reg13" width="40%" height="12%" left="50%" top="48%" />
			<region id="reg14" width="40%" height="12%" left="50%" top="60%" />
			<region id="reg15" width="40%" height="12%" left="50%" top="72%" />
			<region id="reg16" width="40%" height="12%" left="50%" top="84%" />
 		</regionBase>
		
		<descriptorBase>
			<descriptor id="desc_bg" region="reg_bg"/>
			<descriptor id="desc1" region="reg1" />
 			<descriptor id="desc2" region="reg2" />
			<descriptor id="desc3" region="reg3" />
			<descriptor id="desc4" region="reg4" />
			<descriptor id="desc5" region="reg5" />
			<descriptor id="desc6" region="reg6" />
			<descriptor id="desc7" region="reg7" />
			<descriptor id="desc8" region="reg8" />
			<descriptor id="desc9" region="reg9" />
			<descriptor id="desc10" region="reg10" />
			<descriptor id="desc11" region="reg11" />
			<descriptor id="desc12" region="reg12" />
			<descriptor id="desc13" region="reg13" />
			<descriptor id="desc14" region="reg14" />
			<descriptor id="desc15" region="reg15" />
			<descriptor id="desc16" region="reg16" />
 		</descriptorBase>
		
	</head>
	<body>
		<port id="pBg" component="bg" />
		<media id="bg" src="../resources/images/background.celeste.png" descriptor="desc_bg" />

		<port component="text_white" id="entry_white"/>
		<media descriptor="desc1" id="text_white" src="../resources/text/text_white.txt" >
			<property name="fontColor" value="white" />
			<property name="fontSize" value="12" />
		</media>
 		<port component="text_black" id="entry_black"/>
		<media descriptor="desc2" id="text_black" src="../resources/text/text_black.txt" >
			<property name="fontColor" value="black" />
			<property name="fontSize" value="14" />
		</media>
		<port component="text_silver" id="entry_silver"/>
		<media descriptor="desc3" id="text_silver" src="../resources/text/text_silver.txt" >
			<property name="fontColor" value="silver" />
			<property name="fontSize" value="16" />
		</media>
		<port component="text_gray" id="entry_gray"/>
		<media descriptor="desc4" id="text_gray" src="../resources/text/text_gray.txt" >
			<property name="fontColor" value="gray" />
			<property name="fontSize" value="18" />
		</media>
		<port component="text_red" id="entry_red"/>
		<media descriptor="desc5" id="text_red" src="../resources/text/text_red.txt" >
			<property name="fontColor" value="red" />
			<property name="fontSize" value="20" />
		</media>
		<port component="text_maroon" id="entry_maroon"/>
		<media descriptor="desc6" id="text_maroon" src="../resources/text/text_maroon.txt" >
			<property name="fontColor" value="maroon" />
			<property name="fontSize" value="22" />
		</media>
		<port component="text_fuchsia" id="entry_fuchsia"/>
		<media descriptor="desc7" id="text_fuchsia" src="../resources/text/text_fuchsia.txt" >
			<property name="fontColor" value="fuchsia" />
			<property name="fontSize" value="24" />
		</media>
		<port component="text_purple" id="entry_purple"/>
		<media descriptor="desc8" id="text_purple" src="../resources/text/text_purple.txt" >
			<property name="fontColor" value="purple" />
			<property name="fontSize" value="26" />
		</media>
		<port component="text_lime" id="entry_lime"/>
		<media descriptor="desc9" id="text_lime" src="../resources/text/text_lime.txt" >
			<property name="fontColor" value="lime" />
			<property name="fontSize" value="12" />
		</media>
		<port component="text_green" id="entry_green"/>
		<media descriptor="desc10" id="text_green" src="../resources/text/text_green.txt" >
			<property name="fontColor" value="green" />
			<property name="fontSize" value="14" />
		</media>
		<port component="text_yellow" id="entry_yellow"/>
		<media descriptor="desc11" id="text_yellow" src="../resources/text/text_yellow.txt" >
			<property name="fontColor" value="yellow" />
			<property name="fontSize" value="16" />
		</media>
		<port component="text_olive" id="entry_olive"/>
		<media descriptor="desc12" id="text_olive" src="../resources/text/text_olive.txt" >
			<property name="fontColor" value="olive" />
			<property name="fontSize" value="18" />
		</media>
		<port component="text_blue" id="entry_blue"/>
		<media descriptor="desc13" id="text_blue" src="../resources/text/text_blue.txt" >
			<property name="fontColor" value="blue" />
			<property name="fontSize" value="20" />
		</media>
		<port component="text_navy" id="entry_navy"/>
		<media descriptor="desc14" id="text_navy" src="../resources/text/text_navy.txt" >
			<property name="fontColor" value="navy" />
			<property name="fontSize" value="22" />
		</media>
		<port component="text_aqua" id="entry_aqua"/>
		<media descriptor="desc15" id="text_aqua" src="../resources/text/text_aqua.txt" >
			<property name="fontColor" value="aqua" />
			<property name="fontSize" value="24" />
		</media>
		<port component="text_teal" id="entry_teal"/>
		<media descriptor="desc16" id="text_teal" src="../resources/text/text_teal.txt" >
			<property name="fontColor" value="teal" />
			<property name="fontSize" value="26" />
		</media> 
	</body>
</ncl>
