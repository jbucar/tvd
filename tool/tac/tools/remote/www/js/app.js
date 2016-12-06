// We use an "Immediate Function" to initialize the application to avoid leaving anything behind in the global scope
(function () {
    /* --------------------------------- Handlebars helpers -------------------------------- */
    var onDeviceReady = function() {
        FastClick.attach(document.body);
        $('body').html(new ConectarView().render().$el);

        if( ! window.cordova) {
            // Show server-input
            $('body').find('#server-input').show();

            // Hide server-list
            $('body').find('#server-list').hide();
        }

        var tac_host = '10.0.0.106';
        var tac_port = 2000;
        var system_info = {
            id:"00B4302EAC4A",
            system: {
                api:1,
                version:"1.0.5268",
                commit:"g0ff97cd"},
                platform:{
                    name:"aml_isdbt_1l",
                    version:"1.0.460",
                    commit:"gddcac47\n"
            }
        };
//         $('body').html(new HomeView(system_info, tac_host, tac_port).render().$el);

        if (navigator.notification) { // Override default HTML alert with native dialog
            window.alert = function (message) {
                navigator.notification.alert(
                    message,              // message
                    null,                 // callback
                    "TAC Control Remoto", // title
                    'OK'                  // buttonName
                );
            };
        }
    };

    /* ---------------------------------- Local Variables ---------------------------------- */
    HomeView.prototype.template = Handlebars.compile($("#home-tpl").html());
    ConectarView.prototype.template = Handlebars.compile($("#servers-tpl").html());

    
    /* --------------------------------- Event Registration -------------------------------- */
    if (window.cordova) {
        document.addEventListener("deviceready", onDeviceReady, false);
    } else {
        // Mock discovery
        discovery = {
            startDiscovery: function() {},
            stopDiscovery: function() {}
        };
        onDeviceReady();
    }

    /* ---------------------------------- Local Functions ---------------------------------- */

}());
