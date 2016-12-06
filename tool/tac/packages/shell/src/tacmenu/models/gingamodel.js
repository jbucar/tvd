(function (scope) {
   "use strict";

   enyo.kind({
      name: 'GingaAppModel',
      kind: 'enyo.Model',
      attributes: {
          id: undefined,
          name: undefined
      }
   });

   enyo.kind({
      name: 'GingaDownloadingModel',
      mixins: [enyo.BindingSupport, enyo.ComputedSupport, enyo.EventEmitter],
      step: 0,
      total: 0,
      appId: undefined,
      downloaded: false,
      computed: [
          { method: "percentage", path: ["step", "total"] }
      ],
      percentage: function() {
          return this.get('total') === 0 ? 0 : this.get('step') * 100 / this.get('total');
      },
      update: function(appId, step, total) {
          log.info('GingaDownloadingModel', 'update', 'step=%d, total=%d', step, total);
          var downloadEmit;
          if (this.step === this.total) {
              downloadEmit = true;
              this.set('downloaded', false);
          } else if( step === this.total ) {
              downloadEmit = false;
              this.set('downloaded', true);
          }
          this.set('step', step);
          this.set('total', total);
          this.set('appId', appId);
          if (downloadEmit !== undefined) {
              this.emit('downloadGinga', { downloading: downloadEmit});
          }
      },
      reset: function() {
          this.set('downloaded', false);
          this.emit('downloadGinga', { downloading: false});
          this.set('step', 0);
          this.set('total', 0);
      }
   });

   enyo.kind({
       name: 'GingaAppsCollection',
       kind: 'enyo.Collection',
       model: 'GingaAppModel',
       getApp: function(id) {
           return this.find(function(app) {
               return app.get('id') === id;
           });
       }
   });

   enyo.kind({
       name: "GingaModel",
       mixins: [enyo.BindingSupport, enyo.EventEmitter],
       published: {
           enabled: false,
           applications: undefined,
           download: undefined,
           running: false
       },
       bindings: [
           { from: '.applications.length', to: '.hasInteractivity', transform: function(length) {
               return length > 0;
           }}
       ],
       components: [
           { kind: 'Signals', onTVMode: 'onTvMode' }
       ],
       init: function() {
           var self = this;
           this.set('applications', new GingaAppsCollection());
           this.set('download', new GingaDownloadingModel());
           tvd.middleware.disableApplications(true);
		   tvd.middleware.isMiddlewareEnabled(function(err,result) {
			   if (!err && result) {
				   self.set('enabled', true);
			   }
		   });

           tvd.on('middleware', function(evt){
               if (evt.type === 'added') {
                   self.addApp(evt);
               } else if(evt.type === 'removed') {
                   self.removeApp(evt);
               } else if(evt.type === 'download') {
                   self.updateProgress(evt);
               } else if(evt.type === 'start') {
                   self.set('running', true);
               } else if(evt.type === 'stop') {
                   self.set('running', false);
               }
           });
           return this;
       },
       onTvMode: function(oSender, oEvent) {
           if (this.get('enabled')) {
               var disable = oEvent.mode !== 'fullscreen';
               log.info('GingaModel', 'onTvMode', 'Disable ginga applications ' + disable);
               tvd.middleware.disableApplications(disable);
           }
       },
       hasInteractivityChanged: function(was, is) {
           if (is) {
               notification.emit('gingaAvailable', true);
           }
       },
       runningChanged: function(was, is) {
            this.emit('runningGinga', { running: is});
       },
       enable: function(enabled) {
           log.info('GingaModel', 'enable', 'Enable ginga = %d', enabled);
           this.set('enabled', enabled);
           tvd.middleware.enableMiddleware(enabled);
           notification.emit('gingaEnabled', enabled);
       },
       addApp: function(evt) {
           log.info('GingaModel', 'addApp', 'Add app = %s', evt.app.name);
           var app = new GingaAppModel();
           app.set('id', evt.id);
           app.set('name', evt.app.name);
           this.get('applications').add(app);
       },
       removeApp: function(evt){
           log.info('GingaModel', 'removeApp', 'Remove app with id=%s', evt.id);
           var app = this.get('applications').getApp(evt.id);
           this.get('applications').remove(app);
           if(this.get('download').get('appId') === evt.id) {
               this.get('download').reset();
           }
       },
       updateProgress: function(evt){
           this.get('download').update(evt.id, evt.progress.step, evt.progress.total);
       }
   });
})(this);
