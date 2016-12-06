{{#if .show}}
	<section class="volume-background">
		<label>{{.volume}}</label>
		<section class="volume-container {{# isMuted}}muted{{/}}">
			<section class="volume-level" style="height:{{ 100 - .volume}}%;"></section>
		</section>
		<section class="volume-image">
			<img src="{{.icon}}">
		</section>
	</section>
{{/if}}
