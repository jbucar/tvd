{{#if .show}}
	<section id="SplashContainer" class="full_background" style="background-image: url({{.bgImage}})">
		<style type="text/css">
			@keyframes pulso {
				from { opacity: 0; }
				to   { opacity: 1; }
			}
		</style>
		{{#each .delayPoints}}
			<div class="SplashLoadingPoint" style="animation: pulso {{delay}}ms ease-out {{.}}ms infinite;"></div>
		{{/each}}
	</section>
{{/if}}
