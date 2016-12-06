{{#if .show}}
	<section id="InfoBox">
		<!-- Poster image-->
		<img id="InfoBoxLogo" src="{{.logo}}">

		<!-- Title and clock -->
		<section id="InfoBoxHeader" class="InfoBoxRow">
			<label id="InfoBoxHeaderTitle" class="oneline_text">{{.name}}</label>
			<img src="{{.clockImg}}" style="width: 3%; -webkit-filter: invert(40%);">
			<label style="margin-left: 1%;">{{.timeNow}}</label>
		</section>

		<!-- Progress bar -->
		<section id="InfoBoxProgress" class="InfoBoxRow">
			<label style="width: 7%;">{{.beginTime}}</label>
			<section id="InfoBoxProgressBar">
				<section id="InfoBoxProgressBarFill" style="width: {{.progressPercent}}%"></section>
			</section>
			<label>{{.duration}}</label>
		</section>

		<!-- Player control buttons -->
		<section id="InfoBoxControls" class="InfoBoxRow">
			<section id="InfoBoxButtons">
				{{#each .buttons}}
					<img id="InfoBoxBtn_{{.name}}" src="{{.src}}" tabindex="0" on-keydown="keyHandler:{{.name}}">
				{{/each}}
			</section>
			<label id="InfoBoxEndTime" style="align-self: flex-start;">{{.endTime}}</label>
		</section>
	</section>
{{/if}}
