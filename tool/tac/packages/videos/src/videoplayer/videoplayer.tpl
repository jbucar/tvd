<section id="VideoPlayer" tabindex="0" on-keydown="keyDownHandler">
	<video id="Video" src="{{videoURL}}" autoplay>
		{{#each subtitles}}
			<track kind="subtitles" label="{{.name}}" src="{{.uri}}" srclang="{{.lang}}" {{# @index == 0}}default{{/}} />
		{{/each}}
	</video>
	<FFRWDialog/>
	<InfoBox/>
</section>
