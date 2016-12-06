<section class="carrousel_list {{animated}}" on-keydown="keyhandler" style="transform: translateY({{transform}}px);">
{{# carrousels}}
	<Carrousel title={{.name}} items={{.items}} tpl="{{{.tpl}}}" on-itemsChanged="refreshCarrousels" on-itemFocused="focusChange" />
{{/}}
</section>
