{{#if scanProgressShow}}
<section class="scan_progress">
	<label class="scan_progress_label">{{scanProgressText}}</label>
	<section class="scan_progress_bar">
		<div style="width: {{scanProgressPercentage}}%"></div>
	</section>
    <section class="scan_progress_channels">
		{{#each scanProgressChannels}}
			<section class="scan_progress_channel">
				<label class="scan_progress_label">{{.name}}</label>
			</section>
		{{/each}}
    </section>
</section>
{{/if}}

