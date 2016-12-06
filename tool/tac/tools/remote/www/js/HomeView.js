var HomeView = function (system_info,tac_ip, tac_port) {

    var self = this;
    var MOUSE_ENABLED_VERSION = '1.0.5204';
    var teclado;

    var panes = {
        control: new ControlView(tac_ip, tac_port),
        cursor: new CursorView(tac_ip, tac_port)
    };

    var panes_direction = {
        control: {
            to: {
                'cursor': 'right',
            },
        },
        cursor: {
            previous: 'control',
            to: {
                'control': 'left',
            }
        },
    };

    var current_pane = 'control';

    self.initialize = function () {
        self.$el = $('<div/>');
        self.$el.on('click', '.control-item', self.change_pane);
        self.$el.on('click', '.kbd-icon', self.keyboard_toggle);
        panes[current_pane].register(self.$el);
    };

    self.reverse = function( direction ) {
        return direction === 'left' ? 'right' : 'left';
    };

    self.change_pane = function(event) {
        var pane = event.target.id;
        if(current_pane != pane) {
            var direction = panes_direction[current_pane].to[pane];
            self.render_pane(pane, direction);
        }
    };

    self.swipeone = function(event, object) {
        if(object.description.indexOf('left') > -1) {
            self.swipeleft(event, object);
        } else if(object.description.indexOf('right') > -1) {
            self.swiperight(event, object);
        }
    };

    self.swipeleft = function() {
        var pane = panes_direction[current_pane].next;
        if( pane ) {
            self.render_pane(pane, 'right');
        }
    };

    self.swiperight = function() {
        var pane = panes_direction[current_pane].previous;
        if( pane ) {
            self.render_pane(pane, 'left');
        }
    };

    self.render = function () {
        var measures = {
            '.tac-logo': { l: 311, w: 156, h: 65, p:  0 },
            '.kbd-icon': { l: 140, w:  67, h: 41, p: 20 },
        };

        self.$el.html(self.template());
        teclado = new TecladoView(self.$el, tac_ip, tac_port, system_info );
        if( system_info.system.version >= MOUSE_ENABLED_VERSION ) {
            panes_direction.control.next = 'cursor';
            $('#cursor', self.$el).css( 'display', 'table-cell');
        }

        utils.position_left( self.$el, measures);

        var content = $('.content', self.$el);
        content.bind('swipeone',self.swipeone);
        content.bind('swipemove',self.swipeone);
        self.render_pane(current_pane, 'left');
        return self;
    };

    self.render_pane = function (pane, direction) {
        teclado.hide();
        panes[current_pane].unregister(self.$el);
        self.$el.find('#'+current_pane).removeClass('active');

        // Swipe out old pane
        var current_pane_content = self.$el.find('#'+current_pane+'-content');
        current_pane_content.attr('class', 'page transition ' + self.reverse(direction) );
        current_pane_content.one('webkitTransitionEnd', function(e) {
            // After swipe, hide panel if not current
            if (current_pane+'-content' !== $(e.target)[0].id) {
                $(e.target).css('display', 'none');
            }
        });

        current_pane = pane;
        panes[current_pane].register(self.$el, teclado);
        self.$el.find('#'+current_pane).addClass('active');

        // Swipe in new pane
        current_pane_content = self.$el.find('#'+current_pane+'-content');
        current_pane_content.css('display', '');
        panes[current_pane].activate(self.$el);
        current_pane_content.attr('class', 'page '+ direction);
        current_pane_content[0].offsetWidth; // Force reflow. More information here: http://www.phpied.com/rendering-repaint-reflowrelayout-restyle/
        current_pane_content.attr('class', 'page transition center');

        return self;
    };

    self.keyboard_toggle = function() {
        teclado.toggle();
    };

    self.initialize();

};
