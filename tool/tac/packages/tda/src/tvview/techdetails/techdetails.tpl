{{#if .show}}
<Dialog title={{techTitle}} dialogTitleColor="rgb(255,117,0)" dialogBGColor="white" hasButtons=false>
  <section class="tech_section_base">
	<section class="tech_section">
	  <label>{{techAudio}}</label>
	  <section>
		<label>{{techPIDLabel}}:</label>
		<label>{{techAudioPID}}</label>
	  </section>
	  <section>
		<label>{{techChannelsLabel}}:</label>
		<label>{{techChannels}}</label>
	  </section>
	  <section>
		<label>{{techRateLabel}}:</label>
		<label>{{techAudioRate}}</label>
	  </section>
	</section>

	<section class="tech_section">
	  <label>{{techVideo}}</label>
	  <section>
		<label>{{techPIDLabel}}:</label>
		<label>{{techVideoPID}}</label>
	  </section>
	  <section>
		<label>{{techResolutionLabel}}:</label>
		<label>{{techVideoResolution}}</label>
	  </section>
	  <section>
		<label>{{techRateLabel}}:</label>
		<label>{{techVideoRate}}</label>
	  </section>
	</section>

	<section class="tech_section">
	  <label>{{techSignalLabel}}</label>
	  <section>
		<label>{{techSignalLevel}}:</label>
		<label>{{techSignal}}</label>
	  </section>
	  <section>
		<label>{{techSignalQualityLabel}}:</label>
		<label>{{techSignalQuality}}</label>
	  </section>
	  <section>
		<label>{{techFrequencyLabel}}:</label>
		<label>{{techFrequency}}</label>
	  </section>
	</section>

	<section class="tech_section">
	  <label>{{techChannelLabel}}</label>
	  <section>
		<label>{{techChannelIDLabel}}:</label>
		<label>{{techChannelID}}</label>
	  </section>
	  <section>
		<label>{{techChannelNumberLabel}}:</label>
		<label>{{techChannelNumber}}</label>
	  </section>
	</section>
  </section>
</Dialog>
{{/if}}

