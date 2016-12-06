var utils = {
    base_w: 720,
    base_h: 1280,

    send_msg: function (tac_ip, tac_port, service, method) {
        var args = Array.prototype.slice.call(arguments);
        var params = {
                'serviceID': service,
                'method':method,
                'params': args.slice(4),
        };

        var settings = {
            method: 'POST',
            url: 'http://'+tac_ip+':'+tac_port+'/api',
            data:  JSON.stringify(params),
        };
        $.ajax(settings);
    },

    send_input_msg: function (tac_ip, tac_port, method) {
        var args = Array.prototype.slice.call(arguments);
        args.splice(2, 0, 'ar.edu.unlp.info.lifia.tvd.input');
        this.send_msg.apply(this, args);
    },

    position_generic: function ( ele, measure ) {
        var w = $( document.body ).width();
        var h = $( document.body ).height();
        if ('w' in measure) {
            ele.style.width = ""+ ((measure.w+2*measure.p)/this.base_w*w)+"px";
        }
        if ('h' in measure) {
            ele.style.height = ""+((measure.h+2*measure.p)/this.base_h*h)+"px";
        }
        if ('t' in measure) {
            ele.style['margin-top'] = ""+((measure.t-measure.p)/this.base_h*h)+"px";
        }

        ele.style['padding-top'] = ""+(measure.p/this.base_h*h)+"px";
        ele.style['padding-bottom'] = ""+(measure.p/this.base_h*h)+"px";
        ele.style['padding-left'] = ""+(measure.p/this.base_w*w)+"px";
        ele.style['padding-right'] = ""+(measure.p/this.base_w*w)+"px";

    },

    position_left: function ( element, measures ) {
        var w = $( document.body ).width();

        for (var measure_name in measures) {
            var measure = measures[measure_name];
            var ele = element.find(measure_name)[0];
            this.position_generic( ele, measure );
            if ('l' in measure) {
                ele.style['margin-left'] = ""+((measure.l-measure.p)/this.base_w*w)+"px";
            }
        }
    },

    position_right: function ( element, measures ) {
        var w = $( document.body ).width();

        for (var measure_name in measures) {
            var measure = measures[measure_name];
            var ele = element.find(measure_name)[0];
            this.position_generic( ele, measure );
            if ('l' in measure) {
                ele.style['margin-left'] = ""+((this.base_w-2*(measure.l+measure.w+measure.p)-measure.e)/this.base_w*w)+"px";
            }
        }
    },

    position_center: function ( element, measures ) {
        var w = $( document.body ).width();

        for (var measure_name in measures) {
            var measure = measures[measure_name];
            var ele = element.find(measure_name)[0];
            this.position_generic( ele, measure );
            if ('l' in measure) {
                ele.style['margin-left'] = ""+((this.base_w-measure.w-measure.e-2*measure.p)/this.base_w*w/2)+"px";
            }
        }
    },
};
