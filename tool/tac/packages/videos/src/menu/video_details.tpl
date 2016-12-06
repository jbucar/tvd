<section id="DetailsMenu">
	<section id="DetailsMenuTitle">
		<label class="oneline_text">{{focusedItem.title}}</label>
		<img id="DetailsMenuRanking" src="{{ratingIcon}}">
		<label>{{focusedItem.rating}}</label>
	</section>
	{{#each extraProps}}
		<label class="DetailsMenuLabel">{{focusedItem[.]}}</label>
	{{/each}}
	<label id="DetailsMenuSummary" style="-webkit-line-clamp: {{maxSummaryLines}};">
		{{focusedItem.summary}}
	</label>
	<section id="DetailsMenuCarrousel">
		<CarrouselList carrousels="{{carrousels}}" />
	</section>
</section>
