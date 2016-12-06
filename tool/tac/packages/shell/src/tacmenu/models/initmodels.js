/*
* Initialize all models
*/

/*
* Applications
*/
var Applications = new AppsCollection();
Applications.fetch();

var FavoriteApplications = new FavAppsCollection();
FavoriteApplications.setCollection(Applications);

var ApplicationsCategoriesCollection = (function(apps, favs) {
	collection = new AppCategoriesCollection();
	collection.setCollection(apps);

	// Update Categories collection when an App is favoritized
	var updateAppsCat = collection.collectionModelsChanged.bind(collection);
	favs.on('add', updateAppsCat);
	favs.on('remove', updateAppsCat);

	// Initialize
	collection.collectionModelsChanged();

	return collection;
})(Applications, FavoriteApplications);


/*
* Session
*/
var session = new SessionModel();
session.initialize();

/*
* ScanModel
*/
var scanModel = new ScanModel().addAPI('tvd', tvd).initialize();

/*
* Tvd
*/
var tvdadapter = new TvdAdapter().addAPI('tvd', tvd).init();

var channels = new ChsCollection().addAPIs({'tvd': tvd, 'tvdAdapter': tvdadapter, 'scan': scanModel});
channels.initialize();

var availableChannels = new AvailableChCollection();
availableChannels.set( 'collection', channels );
availableChannels.init();

var chFavs = new FavChCollection();
chFavs.set( 'collection', availableChannels );

var bckChs = new BckChCollection();
bckChs.set( 'collection', channels );

var ChannelCategories = new ChannelCategoriesCtor(session, chFavs);

// Force collection population.
channels.populate();
ChannelCategories.populate();

var details = new DetailsModel();


// Devel
var LogConfig = new LogConfigGlue();
var DevMode = new DevelopmentModeGlue().init();

// Network
var netmodules = NetworkModule.init();

var Connections = netmodules.connections;
var Connectivity = netmodules.connectivity;
var ActiveConnection = netmodules.activeConnection;
var WirelessAPs = netmodules.wirelessAPs;
var WirelessConnection = netmodules.wirelessConnection;
Connectivity.sync();

/*
* Update
*/
var Update = new UpdateModel().init();

/*
* ServicesModel
*/
var Services = new ServicesCollection();

/*
* GingaModel
*/
var gingaModel = new GingaModel().init();

/*
* GeoLocalModel
*/
var geolocalModel = new GeoLocalModel().init();

// Image
var imgsProviders = new enyo.Collection();
initImageProviderCollection(imgsProviders);
