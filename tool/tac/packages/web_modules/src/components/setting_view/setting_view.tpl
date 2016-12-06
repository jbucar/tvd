<section id="SettingView" class="full_background" style="background-image: url({{.bgImage}})">
	<style type="text/css">
		#right_section > .buttons > .button:focus {
			background-color: {{theme.fgColor}};
		}
	</style>

	{{#with headerData}}
		{{>header}}
	{{/with}}

	{{#if currentWidget.showDialog}}
		{{#if currentWidget.dialogType=='input'}}
			<InputDialog title={{currentWidget.dialogTitle}} positive={{currentWidget.dialogYes}} inputs={{currentWidget.dialogContent}} validator={{currentWidget.dialogInputValidator}} on-result="onDialogResult" />
		{{elseif currentWidget.dialogType=='confirm'}}
			<ConfirmDialog title={{currentWidget.dialogTitle}} description={{currentWidget.dialogContent}} on-result="onDialogResult"/>
		{{/if}}
	{{/if}}

	<section class="left_section_base">
		<section id="left_section" class="left_section">
			<section class="image_container {{#if .infoPanel.fullIconImg}}full_size{{/if}}">
				<img  class="image" src="{{.leftPanelIcon}}">
			</section>
			<section class="text">
				<section class="title">
					<p class="oneline_text">{{.leftPanelTitle}}</p>
					{{#if .leftPanelSubTitle}}
						<p class="oneline_text subtitle">{{.leftPanelSubTitle}}</p>
					{{/if}}
				</section>
				{{#each .leftPanelDescriptions}}
					<section>
						{{#each .}}
							<p class="default_paragraph">{{.}}</p>
						{{/each}}
					</section>
				{{/each}}
			</section>
		</section>
		{{>extraContent}}
	</section>

	<section id="right_section" class="right_section" style="background-color: {{theme.bgColor}};">
		<section id="buttons" class="buttons">
			{{#each topPanel.widgets}}
				{{#if .visible}}
					<button class="button"
							tabindex="0"
							on-keydown="btnKeyHandler:{{this}},{{@index}}"
							on-focus="btnFocusHandler:{{this}},{{@index}}">
						<section class="hbox_center">
							<section class="button_left hbox_center">
								<img src="{{.icon}}">
								<label class="oneline_text">{{.name}}<label>
							</section>
							<section class="button_right hbox_center">
								{{#if .action_icon && .progress<0}}
									<img src="{{.action_icon}}">
								{{elseif .rightLabel}}
									<label>{{.rightLabel}}</label>
								{{/if}}
							</section>
						</section>
						{{#if .progress >= 0}}
							<section class="progress_bar"><section style="width: {{.progress}}%"></section></section>
						{{/if}}
					</button>
				{{/if}}
			{{/each}}
		</section>
	</section>
</section>
