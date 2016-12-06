var CursorView = function (tac_ip, tac_port) {

    var self = this;
    var prevX;
    var prevY;
    var isClick = false;
    var keyboard;

    self.register = function (element, teclado) {
        keyboard = teclado;

        $(element).find(".cursor-div").on('touchstart', self.touchstart);
        $(element).find(".cursor-div").on('touchend', self.touchend);
        $(element).find(".cursor-div").on('touchmove', self.touchmove);
        $(element).find(".cursor-div").on('mousemove', self.mousemove);
        $(element).find(".cursor-div").on('click', self.click);
    };

    self.unregister = function (element) {
        $(element).find(".cursor-div").off('touchstart', self.touchstart);
        $(element).find(".cursor-div").off('touchend', self.touchend);
        $(element).find(".cursor-div").off('touchmove', self.touchmove);
        $(element).find(".cursor-div").off('mousemove', self.mousemove);
        $(element).find(".cursor-div").off('click', self.click);
    };

    self.activate = function (/*element*/) {
        prevX = -1;
        prevY = -1;
    };

    self.send_key_press_release = function (key_code) {
        self.send_keys( [
            { keyCode: key_code, isUp: false },
            { keyCode: key_code, isUp: true },
        ] );
    };

    self.touchstart = function( event ) {
        keyboard.hide();
        var touches = event.originalEvent.changedTouches;
        prevX = touches[0].clientX;
        prevY = touches[0].clientY;
        event.preventDefault();
        isClick = true;
        return false;
    };

    self.touchend = function( event ) {
        keyboard.hide();
        if( isClick ) {
            self.click( event );
        }
        return false;
    };

    self.touchmove = function( event ) {
        keyboard.hide();
        isClick = false;
        var touches = event.originalEvent.changedTouches;
        var x = touches[0].clientX;
        var y = touches[0].clientY;
        if (prevX > -1 && prevY > -1) {
            utils.send_input_msg(tac_ip, tac_port, 'sendMouseMove', x - prevX, y - prevY);
        }
        prevX = x;
        prevY = y;
        event.preventDefault();
        return false;
    };

    self.mousemove = function( event ) {
        keyboard.hide();
        var x = event.originalEvent.clientX;
        var y = event.originalEvent.clientY;
        if (prevX > -1 && prevY > -1) {
            utils.send_input_msg(tac_ip, tac_port, 'sendMouseMove', x - prevX, y - prevY);
        }
        prevX = x;
        prevY = y;
        event.preventDefault();
        return false;
    };

    self.click = function() {
        keyboard.hide();
        utils.send_input_msg(tac_ip, tac_port, 'sendMouseClick', 'BTN_LEFT', false);
        utils.send_input_msg(tac_ip, tac_port, 'sendMouseClick', 'BTN_LEFT', true);
    };
};
