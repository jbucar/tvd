<section id="tvview_container" tabindex="0" on-keydown="keyDownHandler" style='z-index:0; position: absolute; width:100vw; height:100vh; background-color:black; outline:none; '>
{{# showVideo}}
    <video id="tvvideo" autoplay style='width:100%; height:100%;'>
		<source src={{.videoURL}}>
    </video>
{{/}}
<TechnicalDetails/>
<InfoBox/>
</section>
