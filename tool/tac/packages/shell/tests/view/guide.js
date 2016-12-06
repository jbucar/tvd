'use strict';

(function() {
	var channelsMock = [
			{ id: 0, channel: '12.01', name: 'Tele TEST' },
			{ id: 1, channel: '14.05', name: 'Aqua TEST' },
	];

	var guideDef = {
		name: 'guideDef',
		kind: Grid,
		showing: true,
	};

	var shows = [[]];
	var showsListeners = [];

	function createGuideWindow(def, model) {
		def.dependencies = {'chModel': model};
		def.date = new Date('2015-05-27 20:00:00');
		var view = app.createComponent(def, {owner: app});
		view.render();
		return view;
	}

	function createShow(chID, name, from, to) {
		var show = {
			name: name,
			description: 'A test program',
			startTime: from,
			endTime: to,
			category: 0,
			parentalContent: '',
			parentalAge: 'ATP'
		};
		
		shows[chID].push(show);
	}

	function getChannelComponent(view) {
		return view.$.gridWrapperComponent.$.scrollwrapper.getClientControls();
	}

	describe("GuideWindow", function() {
		var view;
		beforeEach(function() {
			var tvdMocked = {
				on: function(signal, callback) {
					if (signal == 'showsChanged' ) {
						showsListeners.push( callback );
					}
				},
				db: {
					getAll: function(fnc) {
						var ids = channelsMock.map( function( ch ) {
							return ch.id;
						});
						fnc( undefined, ids );
					}
				},
				channel: {
					get: function(chId, fnc) {
						var channel =  channelsMock.find( function( ch ) {
							return ch.id === chId;
						});
						fnc( undefined, channel );
					},
					getShowsBetween: function(chId, first, last, fnc) {
						console.log( 'get shows between');
						var _shows = shows[chId];
						fnc(undefined, _shows ? _shows : []);
					}
				},
				player: {
					change: function(chID, fnc) {
						fnc(undefined, chID);
					}
				}
			};
			var tvdadapter = new TvdAdapter().addAPIs({'tvd': tvdMocked});
			var channelModel = new ChsCollection().addAPIs({'tvd': tvdMocked, 'tvdAdapter': tvdadapter});
			channelModel.initialize();
			view = createGuideWindow(guideDef, channelModel);
			channelModel.populate();
		});

		afterEach(function () {
			view.destroy();
			shows = [[]];
			showsListeners = [];
		});

		describe("View", function() {
			it("should exist", function() {
				should.exist(view.hasNode());
			});

			it("should have a header", function() {
				should.exist(view.$.guideHeader.hasNode());
			});

			it("should have 2 channels", function() {
				var clientControls = getChannelComponent(view);
				should.equal(clientControls.length, 2);
			});

			it("should have 8 time headers", function() {
				should.exist(view.$.guideHeader.$.timeElapse.hasNode());
				var clientControls = view.$.guideHeader.$.timeElapse.getClientControls();
				should.equal(clientControls.length, 8);
			});

			it("should have one small show", function() {
				var channelsRow = getChannelComponent(view);
				should.exist(channelsRow[0]);
				createShow(0, 'test', '2015-05-27 20:00:00', '2015-05-27 20:30:00');
				view.updateShows();
				var shows = channelsRow[0].getClientControls();
				should.equal(shows.length, 2);
				console.log(shows[0]);
				should.equal(shows[0].$.title.content, 'TEST');
				view.onLeave();
			});

			it("should have one large show", function() {
				var channelsRow = getChannelComponent(view);
				should.exist(channelsRow[0]);
				createShow(0, 'test', '2015-05-27 20:00:00', '2015-05-28 00:00:00');
				view.updateShows();
				var shows = channelsRow[0].getClientControls();
				should.equal(shows.length, 1);
				should.equal(shows[0].$.title.content, 'TEST');
			});

			it("should have one started show", function() {
				var channelsRow = getChannelComponent(view);
				should.exist(channelsRow[0]);
				createShow(0, 'test', '2015-05-27 18:00:00', '2015-05-28 00:00:00');
				view.updateShows();
				var shows = channelsRow[0].getClientControls();
				should.equal(shows.length, 1);
				should.equal(shows[0].$.title.content, 'TEST');
			});

			it("should have one show ending after time range", function() {
				var channelsRow = getChannelComponent(view);
				should.exist(channelsRow[0]);
				createShow(0, 'test', '2015-05-27 20:00:00', '2015-05-28 18:00:00');
				view.updateShows();
				var shows = channelsRow[0].getClientControls();
				should.equal(shows.length, 1);
				should.equal(shows[0].$.title.content, 'TEST');
			});

			it("should have three shows", function() {
				var channelsRow = getChannelComponent(view);
				should.exist(channelsRow[0]);
				createShow(0, 'test1', '2015-05-27 19:00:00', '2015-05-27 20:30:00');
				createShow(0, 'test2', '2015-05-27 20:30:00', '2015-05-27 21:00:00');
				createShow(0, 'test3', '2015-05-27 21:00:00', '2015-05-28 00:00:00');
				view.updateShows();
				var shows = channelsRow[0].getClientControls();
				should.equal(shows.length, 3);
				for (var i=1; i<4; i++) {
					should.equal(shows[i-1].$.title.content, 'TEST'+i);
				}
			});

			it("should have one complete show and two empty", function() {
				var channelsRow = getChannelComponent(view);
				should.exist(channelsRow[0]);
				createShow(0, 'test', '2015-05-27 20:15:00', '2015-05-27 20:30:00');
				view.updateShows();
				var shows = channelsRow[0].getClientControls();
				should.equal(shows.length, 3);
				should.equal(shows[0].$.title.content, '');
				should.equal(shows[1].$.title.content, 'TEST');
				should.equal(shows[2].$.title.content, '');
			});

			it("empty slot shouldn't accept spotlight", function() {
				var channelsRow = getChannelComponent(view);
				should.exist(channelsRow[0]);
				createShow(0, 'test', '2015-05-27 20:15:00', '2015-05-27 20:30:00');
				view.updateShows();
				var shows = channelsRow[0].getClientControls();
				should.equal(shows.length, 3);
				should.equal(shows[0].spotlight, false);
			});

			it("second channel shouldn't have shows", function() {
				var channelsRow = getChannelComponent(view);
				should.exist(channelsRow[1]);
				createShow(0, 'test', '2015-05-27 20:15:00', '2015-05-27 20:30:00');
				view.updateShows();
				var shows = channelsRow[1].getClientControls();
				should.equal(shows.length, 1);
			});

			it("should maintain order with unordered show incoming", function() {
				var channelsRow = getChannelComponent(view);
				should.exist(channelsRow[0]);
				createShow(0, 'test2', '2015-05-27 20:30:00', '2015-05-27 21:00:00');
				view.$.gridWrapperComponent.updateShows();
				createShow(0, 'test3', '2015-05-27 21:00:00', '2015-05-27 22:00:00');
				view.$.gridWrapperComponent.updateShows();
				createShow(0, 'test1', '2015-05-27 19:00:00', '2015-05-27 20:30:00');
				view.$.gridWrapperComponent.updateShows();
				var shows = channelsRow[0].getClientControls();
				should.equal(shows.length, 4);
				should.equal(channelsRow[1].getClientControls().length, 1);
				for (var i=1; i<4; i++) {
					should.equal(shows[i-1].$.title.content, 'TEST'+i);
				}
				should.equal(shows[3].$.title.content, '');
			});
		});
	});
})();