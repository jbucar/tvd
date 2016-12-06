{{#if .show}}
	<section id="infobox_container" class="InfoBoxContainer">
		<section>
			<img src="{{.icon}}">
		</section>
		<section class="InfoBoxContent">
			<label>{{.channel}}</label>
			<section>
				{{#each .sections}}
					<section>
						<label>{{@key}}</label>
						<label>{{.}}</label>
					</section>
				{{/each}}
			</section>
		</section>
	</section>
{{/if}}
