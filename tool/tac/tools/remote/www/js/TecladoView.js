function sprintf() {
    var args = arguments,
    string = args[0],
    i = 1;
    return string.replace(/%((%)|s|d)/g, function (m) {
        // m is the matched format, e.g. %s, %d
        var val = null;
        if (m[2]) {
            val = m[2];
        } else {
            val = args[i].toString();
            // A switch statement so that the formatter can be extended. Default is %s
            switch (m) {
                case '%d':
                    val = parseFloat(val);
                    if (isNaN(val)) {
                        val = 0;
                    }
                    break;
            }
            i++;
        }
        return val;
    });
}

var NullKeyboard = function() {
    var self = this;
    var visible = false;

    self.show = function() {
        $('body').find('#virtual-keyboard').show();
        visible = true;
    };
    self.close = function() {
        $('body').find('#virtual-keyboard').hide();
        visible = false;
    };
    self.isVisible = function() {
        return visible;
    };
};

var SoftKeyboard = function() {
    var self = this;
    var visible = false;
    var Keyboard = cordova.plugins.Keyboard;

    self.show = function() {
        Keyboard.show();
        visible = true;
    };
    self.close = function() {
        Keyboard.close();
        visible = false;
    };
    self.isVisible = function() {
        return visible;
    };
};

var TecladoView = function (element, tac_ip, tac_port, system_info) {
    var self = this;
    var Keyboard;
    var delete_key;
    var BACKSPACE = 8;
    var BACKSPACE_ENABLED_VERSION = '1.0.5352';

    var ascii2names = {
        // Numbers
        48: ['VK_0'],
        49: ['VK_1'],
        50: ['VK_2'],
        51: ['VK_3'],
        52: ['VK_4'],
        53: ['VK_5'],
        54: ['VK_6'],
        55: ['VK_7'],
        56: ['VK_8'],
        57: ['VK_9'],

        // Uppercase alphabet
        65: ['VK_SHIFT', 'VK_A'],
        66: ['VK_SHIFT', 'VK_B'],
        67: ['VK_SHIFT', 'VK_C'],
        68: ['VK_SHIFT', 'VK_D'],
        69: ['VK_SHIFT', 'VK_E'],
        70: ['VK_SHIFT', 'VK_F'],
        71: ['VK_SHIFT', 'VK_G'],
        72: ['VK_SHIFT', 'VK_H'],
        73: ['VK_SHIFT', 'VK_I'],
        74: ['VK_SHIFT', 'VK_J'],
        75: ['VK_SHIFT', 'VK_K'],
        76: ['VK_SHIFT', 'VK_L'],
        77: ['VK_SHIFT', 'VK_M'],
        78: ['VK_SHIFT', 'VK_N'],
        79: ['VK_SHIFT', 'VK_O'],
        80: ['VK_SHIFT', 'VK_P'],
        81: ['VK_SHIFT', 'VK_Q'],
        82: ['VK_SHIFT', 'VK_R'],
        83: ['VK_SHIFT', 'VK_S'],
        84: ['VK_SHIFT', 'VK_T'],
        85: ['VK_SHIFT', 'VK_U'],
        86: ['VK_SHIFT', 'VK_V'],
        87: ['VK_SHIFT', 'VK_W'],
        88: ['VK_SHIFT', 'VK_X'],
        89: ['VK_SHIFT', 'VK_Y'],
        90: ['VK_SHIFT', 'VK_Z'],

        // Lowercase alphabet
        97: ['VK_A'],
        98: ['VK_B'],
        99: ['VK_C'],
        100: ['VK_D'],
        101: ['VK_E'],
        102: ['VK_F'],
        103: ['VK_G'],
        104: ['VK_H'],
        105: ['VK_I'],
        106: ['VK_J'],
        107: ['VK_K'],
        108: ['VK_L'],
        109: ['VK_M'],
        110: ['VK_N'],
        111: ['VK_O'],
        112: ['VK_P'],
        113: ['VK_Q'],
        114: ['VK_R'],
        115: ['VK_S'],
        116: ['VK_T'],
        117: ['VK_U'],
        118: ['VK_V'],
        119: ['VK_W'],
        120: ['VK_X'],
        121: ['VK_Y'],
        122: ['VK_Z'],

        // Simbols
        13: ['VK_ENTER'],
        32: ['VK_SPACE'],
        39: ['VK_APOSTROPHE'],
        44: ['VK_COMMA'],
        46: ['VK_PERIOD'],
        63: ['VK_SHIFT', 'VK_SLASH'], // Question Mark
        64: ['VK_SHIFT', 'VK_2'], // @
        35: ['VK_SHIFT', 'VK_3'], // #
        95: ['VK_SHIFT', 'VK_MINUS'], // _
        37: ['VK_SHIFT', 'VK_5'], // %
        40: ['VK_SHIFT', 'VK_9'], // (
        41: ['VK_SHIFT', 'VK_0'], // )
        38: ['VK_SHIFT', 'VK_7'], // &
        36: ['VK_SHIFT', 'VK_4'], // $
        43: ['VK_SHIFT', 'VK_EQUALS'], // +
        58: ['VK_SHIFT', 'VK_SEMICOLON'], // :
        59: ['VK_SEMICOLON'], // ;
        34: ['VK_SHIFT', 'VK_APOSTROPHE'], // "
        // open exclamation
        33: ['VK_SHIFT', 'VK_1'], // !
        47: ['VK_SLASH'], // /
        // open question
        45: ['VK_MINUS'], // -
        42: ['VK_SHIFT', 'VK_8'], // *
    };

    self.initialize = function() {
        delete_key = 'VK_BACK';
        if( system_info.system.version >= BACKSPACE_ENABLED_VERSION ) {
            delete_key = 'VK_BACKSPACE';
        }
        Keyboard = new NullKeyboard();
        try {
            Keyboard = new SoftKeyboard();
        } catch(e) {}
        element.on('blur', '#teclado-area', Keyboard.close);
        element.on('keypress', '#teclado-area', self.keypress);
        element.on('keyup', '#teclado-area', self.keyup);
    };

    self.keypress = function( event ) {
       var charCode = event.keyCode;
       console.log(sprintf('[keypress] code: %d, value: "%s", shiftKey: %s', charCode, String.fromCharCode(charCode), event.shiftKey));
       if (charCode in ascii2names) {
           var keys = [];
           ascii2names[charCode].forEach( function( key_name ) {
               keys.push({ keyCode: key_name, isUp: false });
           });
           ascii2names[charCode].slice().reverse().forEach( function( key_name ) {
               keys.push({ keyCode: key_name, isUp: true });
           });
           utils.send_input_msg(tac_ip, tac_port, 'sendKeys', keys);
       }
    };

    self.keyup = function( event ) {
        var charCode = event.keyCode;
        if( charCode === BACKSPACE ) { // Backspace
            console.log(sprintf('[keyup] code: %d (Backspace)', charCode));
            var keys = [ { keyCode: delete_key, isUp: false }, { keyCode: delete_key, isUp: true } ];
            utils.send_input_msg(tac_ip, tac_port, 'sendKeys', keys);
        }
    };

    self.toggle = function() {
        if( ! Keyboard.isVisible() ) {
            self.show();
        } else {
            self.hide();
        }
    };

    self.show = function() {
        Keyboard.show();
        $('body').find('#teclado-area').focus();
    };

    self.hide = function() {
        Keyboard.close();
	$('body').find('#teclado-area').blur();
    };

    self.initialize();
};