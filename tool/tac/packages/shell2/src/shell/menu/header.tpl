<div class="logo">
	<img  src="{{.logo}}"/>
</div>
<div class="datetime">
	{{#if .inet}}<img src="{{.inet}}"/>{{/if}}
	<img src="{{.calendar}}"/>
	<label>{{.date}}</label>
	<img src="{{.clock}}"/>
	<label>{{.time}}</label>
</div>
