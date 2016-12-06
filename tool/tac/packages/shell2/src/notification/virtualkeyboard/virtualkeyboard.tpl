{{#if .show}}
	<section class="container full_background" style="background-image: url({{.bgImage}})">
		<label class="title">{{.title}}</label>
		<section class="keyboard" style="background-color: {{.bgColor}};">
			<section class="input">
				<input id="user_input_text" type="{{.type}}" on-keydown="keyHandler" />
			</section>
		</section>
	</section>
{{/if}}
