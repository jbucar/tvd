(function() {
    'use strict';

    // TODO [nbaglivo]: Parametrize font-size
    enyo.kind({
        name: 'components.ProgressLabel',
        classes: 'progress-label',
        published: {
            progress: 0
        },
        bindings: [
            { from: '.content', to: '.$.label.content' },
            { from: '.barClasses', to: '.$.progress.barClasses' },
            { from: '.progress', to: '.$.progress.progress', oneWay: false }
        ],
        components: [
            {
                classes: 'bar-wrapper',
                components: [
                    {
                        name: 'progress',
                        kind: 'moon.ProgressBar',
                        classes: 'bar',
                        progress: 0
                    },
                ]
            },
            {
                classes: 'label-wrapper',
                style: 'font-size: 20px;',
                components: [
                    { name: 'label' }
                ]
            }
        ],
        animateProgressTo: function() {
            return this.$.progress.animateProgressTo.apply(this.$.progress, arguments);
        },
        stopAnim: function() {
            return this.$.progress.$.progressAnimator.stop();
        }
    });

    // TODO [nbaglivo]: Remove inline JS style and move it to CSS classes
    enyo.kind({
        name: 'components.Progress',
        style: 'width: 100%',
        bgColor: undefined,
        published: {
            progressing: false,
        },
        bindings: [
            { from: '.content', to: '.$.content.content' }
        ],
        components: [
            { name: 'bg', style: 'position: absolute; left: 0; top: 90%; width: 0%; height: 10%;' },
            { name: 'content' },
            { kind: 'Animator', duration: 600, onStep: 'animStep', onEnd: 'animEnd', onStop: 'animStopped' }
        ],
        create: enyo.inherit(function(sup) {
            return function() {
                sup.apply(this, arguments);
                if (this.bgColor) {
                    this.$.bg.applyStyle('background-color', this.bgColor);
                }
            }
        }),
        animStep: function(sender) {
            this.$.bg.applyStyle('width', sender.value + '%');
        },
        animStopped: function() {
            if (this.$.bg) {
                this.$.bg.applyStyle('width', '0%');
            }
        },
        animEnd: function() {
            if (this.get('progressing')) {
                this.$.animator.play({
                    startValue: 0,
                    endValue: 100,
                    node: this.hasNode()
                });
            }
            else {
                this.$.bg.applyStyle('width', '0%');
            }
        },
        progressingChanged: function(was, is) {
            if (is) {
                this.$.animator.play({
                    startValue: 0,
                    endValue: 100,
                    node: this.hasNode()
                });
            }
            else {
                this.$.animator.stop();
            }
        }
    });

})();
