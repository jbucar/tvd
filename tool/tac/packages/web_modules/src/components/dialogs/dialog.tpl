<style type="text/css">
	#Dialog > .DialogBox {
		background-color: {{theme.bgColor}};
	}

	#Dialog > .DialogBox > .DialogTitle {
		color: {{dialogTitleColor}};
	}

	#Dialog > .DialogBox > .DialogButtons > button {
		background-color: {{theme.fgColor}};
	}

	#Dialog > .DialogBox > .DialogButtons > button:focus {
		background-color: {{theme.focusColor}};
	}
</style>

<section id="Dialog" class="DialogBackground">
	<section class="DialogBox">
		<label class="DialogTitle">{{title}}</label>
		{{>content}}
		{{#if .hasButtons}}
			<section id="buttons" class="DialogButtons">
				{{#if positive!==''}}
					<button id="positive" tabindex="0" on-keydown="btnKeyHandler:1,true">{{positive}}</button>
				{{/if}}
				{{#if negative!==''}}
					<button id="negative" tabindex="0" on-keydown="btnKeyHandler:0,false">{{negative}}</button>
				{{/if}}
			</section>
		{{/if}}
	</section>
</section>
