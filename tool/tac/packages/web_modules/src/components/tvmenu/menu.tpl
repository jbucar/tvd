<style type="text/css">
	.MenuSection:focus {
		background-color: {{.sectionFocusBgColor}};
	}
</style>

<section id="Menu" class="full_background" style="background-image: url({{.menuBgImg}})" on-keydown="keyDown">
	{{#if .shortcuts.length > 0 && .modelReady}}
		<PopupBox orientation="ltr"
		          visible={{.showLeftPopup}}
		          containerClass="MenuPopupContainer"
		          boxClass="MenuPopupBox"
		          boxBgColor="{{.leftPopupBgColor}}"
		          boxInitialPos="22">

			<section id="TVMenu_MenuIcon" class="{{#!showLeftPopup}}alignRight{{/}}">
				<img id="TVMenu_MenuIconImg" src="{{.leftPopupIcon}}">
			</section>
			<section id="MenuSections">
				{{#each .shortcuts}}
					<section class="MenuSection" tabindex="0" on-keydown="shortcutKeyHandler:{{@index}},{{.section.index}},{{.opts}}">
						<section class="MenuSectionIcon {{#!showLeftPopup}}alignRight{{/}}">
							<img src="{{.icon}}" class="{{#@index != currentShortcut}}blured{{/}}">
						</section>
						{{#if showLeftPopup}}
							<label class="MenuSectionLabel">{{.title}}</label>
						{{/if}}
					</section>
				{{/each}}
			</section>
		</PopupBox>
	{{/if}}

	{{#with .dialog}}
		{{>dialog}}
	{{/with}}

	{{#with .header}}
		<section id="MenuHeader">{{>header}}</section>
	{{/with}}

	<section id="MenuBody" tabindex="0" on-keydown="menukeyHandler">
		{{#if .modelReady}}
			{{#with .section}}
				{{>section}}
			{{/with}}
		{{/if}}
	</section>
</section>
