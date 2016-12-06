var ControlView = function (tac_ip, tac_port) {

    var self = this;
    var keyboard;

    self.register = function (element, teclado) {
        keyboard = teclado;

        var down = 'mousedown';
        var up = 'mouseup';
        if (window.cordova) {
            down = 'touchstart';
            up = 'touchend';
            element.on('touchmove', '.button-round-glow', self.unglow);
            element.on('touchmove', '.flechas-item', self.unglow);
            element.on('touchmove', '.vol-item', self.unglow);
            element.on('touchmove', '.ch-item', self.unglow);
            element.on('touchmove', '.colors-item', self.unglow);
        }

        element.on(down, '.button-round-glow', self.round_glow);
        element.on(down, '.menu-item', self.menu_click);
        element.on(down, '.salir-item', self.salir_click);
        element.on(down, '.flechas-item', self.flechas_click);
        element.on(down, '.power-item', self.power_click);
        element.on(down, '.mute-item', self.mute_click);
        element.on(down, '.info-item', self.info_click);
        element.on(down, '.lista-item', self.lista_click);
        element.on(down, '.vol-item', self.vol_click);
        element.on(down, '.ch-item', self.ch_click);
        element.on(down, '.colors-item', self.colors_click);

        element.on(up, '.button-round-glow', self.unglow);
        element.on(up, '.flechas-item', self.unglow);
        element.on(up, '.vol-item', self.unglow);
        element.on(up, '.ch-item', self.unglow);
        element.on(up, '.colors-item', self.unglow);
    };

    self.unregister = function (element) {
        var down = 'mousedown';
        var up = 'mouseup';
        if (window.cordova) {
            down = 'touchstart';
            up = 'touchend';
            element.off('touchmove', '.button-round-glow', self.unglow);
            element.off('touchmove', '.flechas-item', self.unglow);
            element.off('touchmove', '.vol-item', self.unglow);
            element.off('touchmove', '.ch-item', self.unglow);
            element.off('touchmove', '.colors-item', self.unglow);
        }

        element.off(down, '.button-round-glow', self.round_glow);
        element.off(down, '.menu-item', self.menu_click);
        element.off(down, '.salir-item', self.salir_click);
        element.off(down, '.flechas-item', self.flechas_click);
        element.off(down, '.power-item', self.power_click);
        element.off(down, '.mute-item', self.mute_click);
        element.off(down, '.info-item', self.info_click);
        element.off(down, '.lista-item', self.lista_click);
        element.off(down, '.vol-item', self.vol_click);
        element.off(down, '.ch-item', self.ch_click);
        element.off(down, '.colors-item', self.colors_click);

        element.off(up, '.button-round-glow', self.unglow);
        element.off(up, '.flechas-item', self.unglow);
        element.off(up, '.vol-item', self.unglow);
        element.off(up, '.ch-item', self.unglow);
        element.off(up, '.colors-item', self.unglow);

        element.find(".round-glow-back").removeClass("round-unglow");
        element.find(".round-glow-back").removeClass("round-glow");
        element.find(".round-glow-back").css('background', 'rgba(255, 255, 255, 0.0)');
    };

    self.activate = function (element) {
        var padding = 20;

        var measures_left = {
            '.power-item':     { t:  37,         l:  53, w: 122, h: 112, p: padding },
            '.menu-item':      { t: -36-padding, l:  68, w: 105, h: 105, p: padding },
            '.vol-item':       { t:  51-padding, l: 150, w: 104, h: 254, p: padding },
            '.info-lista-row': { t:  51-padding, l:  0,  w: 210, h: 254, p: padding }
        };

        var measures_right = {
            '.mute-item':  { t:  37,         l:  53, w: 122, h: 112, e:   0        , p: padding },
            '.salir-item': { t: -36-padding, l:  68, w: 105, h: 105, e:   0        , p: padding },
            '.ch-item':    { t:  51-padding, l: 150, w: 104, h: 254, e: 210+padding, p: padding }
        };

        var measures_center = {
            '.flechas-item': { t: -49-padding, l: 165, w: 404, h: 405, e:   0,           p: padding },
            '.colors-item':  { t:  51-padding, l:  50, w: 621, h:  86, e:   0,           p: padding },
            '.info-item':    { t:   0,         l:   0, w: 105, h: 105, e: 510+2*padding, p: padding },
            '.lista-item':   { t:   2*padding, l:   0, w: 105, h: 105, e: 510+2*padding, p: padding }
        };
        utils.position_left(element, measures_left);
        utils.position_right(element, measures_right);
        utils.position_center(element, measures_center);
    };

    self.send_key_press_release = function (key_code) {
        console.log('send_key_press_release', key_code);
        utils.send_input_msg( tac_ip, tac_port, 'sendKeys', [
            { keyCode: key_code, isUp: false },
            { keyCode: key_code, isUp: true },
        ] );
    };

    self.send_postional_key = function (event, pos_to_code) {
        keyboard.hide();
        var bar_h = $('.bar').height();
        var click = event;
        if( event.originalEvent.constructor.name === 'TouchEvent' ) {
            click = event.originalEvent.touches[0];
        }
        var point = {x: (click.clientX - event.currentTarget.offsetLeft)/event.currentTarget.offsetWidth, y: (click.clientY - event.currentTarget.offsetTop-bar_h)/event.currentTarget.offsetHeight};

        pos_to_code.forEach( function( pos ) {
            if( self.pointRectangleIntersection(point, pos) ) {
                self.send_key_press_release(pos.code);
                self.glow(event.currentTarget, pos);
            }
        });
    };
    self.glow = function (element, bounds) {
        var round_glow_back, d, w, h, x, y, img;
        if($(element).find(".round-glow-back").length === 0){
            $(element).append("<span class='round-glow-back'></span>");
        }
        round_glow_back = $(element).find(".round-glow-back");
        round_glow_back.removeClass("round-glow").width(); // reading width() forces reflow
        round_glow_back.removeClass("round-unglow").width(); // reading width() forces reflow

        img = $(element).find('img')[0];
        d = Math.min(img.width*(bounds.x2-bounds.x1), img.height*(bounds.y2-bounds.y1));
        w = img.width*(bounds.x2-bounds.x1);
        h = img.height*(bounds.y2-bounds.y1);
        x = img.offsetLeft +  img.width * bounds.x1;
        y = img.offsetTop + img.height * bounds.y1;
        round_glow_back.css({top: y+'px', left: x+'px', height: h, width: w, 'border-radius': d+'px' });

        round_glow_back.addClass("round-glow");
        round_glow_back.one('animationend webkitAnimationEnd oAnimationEnd', function(event) {
            $(this).css('background', 'rgba(255, 255, 255, 0.5)');
        });
    };

    self.unglow = function () {
        var round_glow_back = $(this).find(".round-glow-back");
        round_glow_back.removeClass("round-glow");
        round_glow_back.removeClass("round-unglow");
        round_glow_back.addClass("round-unglow");
        round_glow_back.one('animationend webkitAnimationEnd oAnimationEnd', function(event) {
            $(this).css('background', 'rgba(255, 255, 255, 0.0)');
        });
    };

    self.round_glow = function () {
        var bounds = { x1:0.00, x2:1.00, y1:0.00, y2:1.00 };
        self.glow( $(this), bounds);
    };

    self.menu_click = function () {
        self.send_key_press_release('VK_HOME');
    };

    self.salir_click = function () {
        self.send_key_press_release('VK_EXIT');
    };

    self.power_click = function () {
        self.send_key_press_release('VK_POWER');
    };

    self.mute_click = function () {
        self.send_key_press_release('VK_MUTE');
    };

    self.info_click = function () {
        self.send_key_press_release('VK_INFO');
    };

    self.lista_click = function () {
        self.send_key_press_release('VK_LIST');
    };

    self.flechas_click = function (event) {
        var w = 404;
        var h = 405;
        var pos_to_code = [
            { x1:119/w, x2:285/w, y1:  0/h, y2:113/h, code:'VK_UP' },
            { x1:119/w, x2:285/w, y1:291/h, y2:405/h, code:'VK_DOWN' },
            { x1:  0/w, x2:113/w, y1:119/h, y2:285/h, code:'VK_LEFT' },
            { x1:291/w, x2:404/w, y1:119/h, y2:285/h, code:'VK_RIGHT' },
            { x1:119/w, x2:285/w, y1:119/h, y2:285/h, code:'VK_ENTER' },
        ];
        self.send_postional_key(event, pos_to_code);
    };

    self.vol_click = function (event) {
        var w = 104;
        var h = 254;
        var pos_to_code = [
            { x1:  0/w, x2:104/w, y1:  0/h, y2: 90/h, code:'VK_VOLUME_UP' },
            { x1:  0/w, x2:104/w, y1:163/h, y2:254/h, code:'VK_VOLUME_DOWN' },
        ];
        self.send_postional_key(event, pos_to_code);
    };

    self.ch_click = function (event) {
        var w = 104;
        var h = 254;
        var pos_to_code = [
            { x1:  0/w, x2:104/w, y1:  0/h, y2: 90/h, code:'VK_CHANNEL_UP' },
            { x1:  0/w, x2:104/w, y1:163/h, y2:254/h, code:'VK_CHANNEL_DOWN' },

        ];
        self.send_postional_key(event, pos_to_code);
    };

    self.colors_click = function (event) {
        var w = 621;
        var h =  86;
        var pos_to_code = [
            { x1: 21/w, x2:156/w, y1:0/h, y2: 86/h, code:'VK_RED' },
            { x1:168/w, x2:303/w, y1:0/h, y2: 86/h, code:'VK_GREEN' },
            { x1:317/w, x2:452/w, y1:0/h, y2: 86/h, code:'VK_YELLOW' },
            { x1:464/w, x2:599/w, y1:0/h, y2: 86/h, code:'VK_BLUE' },
        ];
        self.send_postional_key(event, pos_to_code);
    };

    self.pointRectangleIntersection = function(p, r) {
        return p.x > r.x1 && p.x < r.x2 && p.y > r.y1 && p.y < r.y2;
    };

};
