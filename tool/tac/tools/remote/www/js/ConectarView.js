var discovery;

var ConectarView = function () {
    var self = this;

    self.servers = {};

    self.successCallback = function( server ) {
        server = JSON.parse( server );
        if( ( !( server.host in self.servers)) || ( server !== self.servers[server.host] ) ) {
            console.log('ConectarView', 'found new server', server.host, server.port);
            self.servers[server.host] = server;
            self.render();
        }
    };
 
    self.initialize = function () {
        self.servers = {};
        self.$el = $('<div/>');
        self.$el.on('click', '.server', self.conectarFromList);
        self.$el.on('click', '#server-connect', self.conectarFromInput);
        document.addEventListener("pause", self.pause, false);
        document.addEventListener("resume", self.resume, false);
        discovery.startDiscovery(self.successCallback);
    };

    self.validate_tac = function ( tac_ip, tac_port ) {
        var params = {
                'serviceID': 'ar.edu.unlp.info.lifia.tvd.system',
                'method':'getInfo',
        };

        var settings = {
            method: 'POST',
            url: 'http://'+tac_ip+':'+tac_port+'/api',
            data:  JSON.stringify(params),
        };
        $.ajax(settings)
        .done(function( system_info ) {
            if(('id' in system_info) && ('system' in system_info) && ('platform' in system_info)) {
                document.removeEventListener("pause", self.pause, false);
                document.removeEventListener("resume", self.resume, false);
                discovery.stopDiscovery();
                $('body').html(new HomeView(system_info, tac_ip, tac_port).render().$el);
            } else {
                alert("Fallo en la conexion!");
            }
        })
        .fail(function() {
            alert("Fallo en la conexion!");
        });
    };

    self.conectarFromList = function(event) {
        var server = self.servers[event.target.id];
        self.validate_tac( server.host, server.port );
    };

    self.conectarFromInput = function(event) {
        var server = {};
        server.host = $('#server-addr').val();
        server.port = parseInt($('#server-port').val());
        self.validate_tac( server.host, server.port );
    };

    self.pause = function () {
        console.log('ConectarView app paused, stopping discovery');
        discovery.stopDiscovery();
    };

    self.resume = function () {
        console.log('ConectarView app resumed, restarting discovery');
        discovery.startDiscovery(self.successCallback);
    };

    self.render = function () {
        var measures = {
            '.tac-logo': { l: 311, w: 156, h: 65, p:  0 },
        };

        self.$el.html(self.template(self.servers));
        utils.position_left( self.$el, measures);

        return self;
    };

    self.initialize();

};
