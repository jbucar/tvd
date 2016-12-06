<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="textTest" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg_bg" width="100%" height="100%" left="0%" top="0%" />
			<region id="reg1" width="50%" height="33%" left="25%" top="10%" />
			<region id="reg2" width="50%" height="33%" left="0%" top="33%" />
			<region id="reg3" width="50%" height="33%" left="50%" top="33%" />
			<region id="reg4" width="50%" height="34%" left="0%" top="66%" />
			<region id="reg5" width="50%" height="34%" left="50%" top="66%" />
 		</regionBase>
		
		<descriptorBase>
			<descriptor id="desc_bg" region="reg_bg"/>
			<descriptor id="desc1" region="reg1" />
 			<descriptor id="desc2" region="reg2" />
			<descriptor id="desc3" region="reg3" />
			<descriptor id="desc4" region="reg4" />
			<descriptor id="desc5" region="reg5" />
 		</descriptorBase>
		
	</head>
	<body>
		<port id="pBg" component="bg" />
		<media id="bg" src="../resources/images/background.celeste.png" descriptor="desc_bg" />
		<port component="text_normal_normal" id="entry_normal_normal"/>
		<media descriptor="desc1" id="text_normal_normal" src="../resources/text/text_normal_normal.txt" >
			<property name="fontFamily" value="Tiresias" />
			<property name="fontStyle" value="normal" />
			<property name="fontWeight" value="normal" />
			<property name="fontSize" value="36" />
		</media>
		<port component="text_normal_bold" id="entry_normal_bold"/>
		<media descriptor="desc2" id="text_normal_bold" src="../resources/text/text_normal_bold.txt" >
			<property name="fontFamily" value="Tiresias" />
			<property name="fontStyle" value="normal" />
			<property name="fontWeight" value="bold" />
			<property name="fontSize" value="36" />
		</media>
		<port component="text_italic_normal" id="entry_italic_normal"/>
		<media descriptor="desc3" id="text_italic_normal" src="../resources/text/text_italic_normal.txt" >
			<property name="fontFamily" value="Tiresias" />
			<property name="fontStyle" value="italic" />
			<property name="fontWeight" value="normal" />
			<property name="fontSize" value="36" />
		</media>

		<!-- SMALL CAPS -->
		<port component="text_normal_normal_sc" id="entry_normal_normal_sc"/>
		<media descriptor="desc4" id="text_normal_normal_sc" src="../resources/text/text_normal_normal_sc.txt" >
			<property name="fontFamily" value="Tiresias" />
			<property name="fontStyle" value="normal" />
			<property name="fontWeight" value="normal" />
			<property name="fontSize" value="36" />
			<property name="fontVariant" value="small-caps" />
		</media>
		<port component="text_italic_normal_sc" id="entry_italic_normal_sc"/>
		<media descriptor="desc5" id="text_italic_normal_sc" src="../resources/text/text_italic_normal_sc.txt" >
			<property name="fontFamily" value="Tiresias" />
			<property name="fontStyle" value="italic" />
			<property name="fontWeight" value="normal" />
			<property name="fontSize" value="36" />
			<property name="fontVariant" value="small-caps" />
		</media>


	</body>
</ncl>
