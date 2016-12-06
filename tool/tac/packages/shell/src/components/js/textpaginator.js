
(function() {

enyo.kind({
    name: 'TextPaginator',
    spotlight: true,
    published: {
        text: '',
        linesPerPage: 9,
    },
    handlers: {
        onSpotlightLeft: 'scrollToLeft',
        onSpotlightRight: 'scrollToRight',
    },
    classes: 'text-paginator',
    components: [
        {
            classes: 'arrow-bar',
            components: [{ kind: 'Shell.LeftArrow', name: 'leftarrow', style: 'opacity: 0' }]
        },
        { name: 'cvs', tag: 'canvas', style: 'width: 0; height: 0' },
        {
			name: 'anim', kind: "Animator", duration: 300, easingFunction: enyo.easing.linear,
			onStep: 'animStep'
		},
        {
            name: 'container',
            classes: 'container',
            components: [
                { name: 'wrapper', classes: 'wrapper' }
            ]
        },
        {
            classes: 'arrow-bar end',
            components: [{ kind: 'Shell.RightArrow', name: 'rightarrow', style: 'opacity: 0' }]
        },
    ],
    _idx: 0,
    _len: 0,
    textChanged: function(was, is) {
        var pages = _.chunk(this._splitLines(is), this.linesPerPage);
        this._len = pages.length - 1;
        this._idx = 0;

        this.$.wrapper.destroyClientControls();
        this.$.wrapper.createComponents(pages.map(function(page) {
            return { content: page.join('') };
        }), {owner: this}).forEach(function(c) {
            c.render();
        });

        this.$.wrapper.applyStyle('transform', 'translateX(0px)');
        this._adjustArrows();
    },
    scrollToRight: function() {
        if (this._idx < this._len) {
            this._move(1);
        }
    },
    scrollToLeft: function() {
        if (this._idx > 0) {
            this._move(-1);
        }
    },
    _move: function(factor) {
        var width = (parseInt(this.$.wrapper.getComputedStyleValue('width'), 10));
        this.$.anim.play({
            startValue: (width * this._idx),
            endValue: width * (this._idx + factor),
            node: this.hasNode()
        });
        this._idx += factor;
        this._adjustArrows();
    },
    animStep: function(oSender) {
        this.$.wrapper.applyStyle('transform', 'translateX(-' + oSender.value + 'px)');
    },
    _adjustArrows: function() {
        this.$.rightarrow.applyStyle('opacity', this._idx !== this._len? 1 : 0);
        this.$.leftarrow.applyStyle('opacity', this._idx !== 0? 1 : 0);
    },
    _measureWidth: function(text) {
        if (!this.font) {
            this.font = this.getComputedStyleValue('font-size') + ' ' + this.getComputedStyleValue('font-family');
            this.$.cvs.hasNode().getContext("2d").font = this.font;
        }
        return this.$.cvs.hasNode().getContext("2d").measureText(text).width;
    },
    _splitLines: function(text) {
        this.maxTextWidth = this.maxTextWidth || (parseInt(this.getComputedStyleValue('width'), 10));
		// Split text into words by spaces
		var words = text.split(' ');
		var lastWord = words[words.length - 1];
		var lineWidth = 0;
		var wordWidth = 0;
		var thisLine = '';
		var allLines = new Array();

        function addToAllLines(text) {
            allLines.push(text);
            thisLine = '';
            lineWidth = 0;
        }

		// For every element in the array of words
		for (var i = 0; i < words.length; i++) {
			var word = words[i];
			// Add current word to current line
			thisLine = thisLine.concat(word + ' ');
			// Get width of the entire current line
			lineWidth = this._measureWidth(thisLine);
			// If word is not the last element in the array
			if (word !== lastWord) {
				// Find out what the next upcoming word is
				var nextWord = words[i + 1];

				// Check if the current line + the next word would go over width limit
				if (lineWidth + this._measureWidth(nextWord) >= this.maxTextWidth) {
					// If so, add the current line to the allLines array
					// without adding the next word
					addToAllLines(thisLine);
				}

				// '~' indicates inserting a blank line, if required
				else if (word === '~') {
					addToAllLines(' ');
				}

				// If the next word is a line break, end line now
				else if (nextWord === '~') {
					addToAllLines(thisLine);
				}
			}

			// If this IS the last word in the array
			else {
				// Add this entire line to the array and return allLines
				addToAllLines(thisLine);
				return allLines;
			}
		}
	}
});

})();
