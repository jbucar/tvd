<section id="carrousel"
         class="carrousel {{animated}} {{# !visible}}hidden{{/}}"
         tabindex="{{visible ? 0 : -1 }}"
         on-keydown='keyhandler'
         on-focus="focusHandler:true"
         on-blur="focusHandler:false">

	{{# items.length}}
		<label style="color: {{title_color}}; font-family: {{title_font}}; font-size: {{title_size}};">
			{{title}}
		</label>
	{{/}}

	<div id="carrousel_view"
		class="carrousel_view {{animated}} {{# items.length}}margin{{/}}">

		{{#each items : idx}}
			{{>item}}
		{{/each}}
	</div>
</section>
