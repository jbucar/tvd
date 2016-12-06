<section class="popupbox_container {{containerClass}}">
	<section class="popup {{boxClass}} {{.orientation}} {{# .visible}}visible{{/}}"
	         style="transition-duration: {{.delay}}ms;
	                background-color: {{.boxBgColor}};
	                {{#.orientation=='ltr'}}right:{{else}}left:{{/}} {{(100 - .boxInitialPos)}}%;">
		{{yield}}
	</section>
</section>
