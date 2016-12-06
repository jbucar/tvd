{{#if dialog}}
	<Dialog positive={{dialog.positive}} negative={{dialog.negative}} on-result="onDialogResult" >
		<section id="QueuedMovies">
			<section id="QMLogoContainer">
				<img class="logo" src="{{dialog.logo}}"/>
			</section>
			<section id="QMDetails">
				<section id="QMDetailsTitle">
					<label>{{dialog.title}}</label>
					<img id="QMDetailsRanking" src="{{dialog.ratingImg}}">
					<label>{{dialog.rating}}</label>
				</section>
				<label class="QMDetailsLabel">{{dialog.genre}}</label>
				<label class="QMDetailsLabel">{{dialog.director}}</label>
				<label class="QMDetailsLabel">{{dialog.actors}}</label>
				<label class="QMDetailsLabel">{{dialog.runtime}}</label>
				<label id="QMSummary">{{dialog.summary}}</label>
			</section>
		</section>
	</Dialog>
{{/if}}
