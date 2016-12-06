<section id="InputDialog">
	{{#if .error}}
		<label class="ErrorMessage">{{.error}}</label>
	{{/if}}
	{{#each inputs}}
		<section class="InputDialog_input">
			<label>{{.label}}</label>
			<input id="{{.name}}" type="{{.type}}" value="{{.value}}" on-keydown="inputKeyHandler:{{@index}}"/>
		</section>
	{{/each}}
</section>
