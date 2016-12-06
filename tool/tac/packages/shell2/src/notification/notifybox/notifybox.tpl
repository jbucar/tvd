<PopupBox orientation="rtl"
          visible={{.showNotify}}
          delay={{.notifyAnimDelay}}
          containerClass="notifyBoxContainer"
          boxClass="notifyBox"
          boxBgColor="rgb(33,33,33)">

	<img src="{{notifyIcon}}">
	<label class="oneline_text">{{notifyContent}}</label>
</PopupBox>
