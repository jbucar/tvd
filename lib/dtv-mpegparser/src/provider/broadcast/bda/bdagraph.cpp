#include "bdagraph.h"
#include "../../../demuxer/ts/demuxerimpl.h"
#include <util/log.h>
#include <util/buffer.h>
#include <util/assert.h>

#undef DEBUG_MONIKER_NAME

extern "C" {

static const IID IID_ISampleGrabber = // {6B652FFF-11FE-4fce-92AD-0266B5D7C78F}
{ 0x6B652FFF, 0x11FE, 0x4FCE, { 0x92, 0xAD, 0x02, 0x66, 0xB5, 0xD7, 0xC7, 0x8F } };
static const CLSID CLSID_SampleGrabber = // {C1F400A0-3F08-11d3-9F0B-006008039E37}
{ 0xC1F400A0, 0x3F08, 0x11d3, { 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37 } };

static const IID IID_IDVBTLocator =
{ 0x8664da16,0xdda2,0x42ac, {0x92,0x6a,0xc1,0x8f,0x91,0x27,0xc3,0x02 } };
static const IID IID_IDVBTuneRequest =
{ 0x0d6f567e,0xa636,0x42bb, {0x83,0xba,0xce,0x4c,0x17,0x04,0xaf,0xa2 } };
static const IID IID_IDVBTuningSpace2 =
{ 0x843188b4,0xce62,0x43db, {0x96,0x6b,0x81,0x45,0xa0,0x94,0xe0,0x40 } };
static const CLSID CLSID_DVBTLocator =
{ 0x9cd64701,0xbdf3,0x4d14, {0x8e,0x03,0xf1,0x29,0x83,0xd8,0x66,0x64 } };
static const IID IID_IScanningTuner =
{ 0x1dfd0a5c,0x0284,0x11d3, {0x9d,0x8e,0x00,0xc0,0x4f,0x72,0xd9,0x80 } };

};

/*****************************************************************************
* Constructor
*****************************************************************************/
BDAGraph::BDAGraph( tuner::demuxer::ts::DemuxerImpl *demux ) :
	_demux( demux ),
	guid_network_type(GUID_NULL),
	l_tuner_used(-1),
	d_graph_register( 0 )
{
	p_media_control = NULL;
	p_tuning_space = NULL;
	p_filter_graph = NULL;
	p_system_dev_enum = NULL;
	p_network_provider = p_tuner_device = p_capture_device = NULL;
	p_sample_grabber = p_mpeg_demux = p_transport_info = NULL;
	p_scanning_tuner = NULL;
	p_grabber = NULL;

	CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );
}

/*****************************************************************************
* Destructor
*****************************************************************************/
BDAGraph::~BDAGraph()
{
	Destroy();

	if( p_tuning_space )
		p_tuning_space->Release();
	p_tuning_space = NULL;

	CoUninitialize();
}

bool BDAGraph::initialize()
{
	HRESULT hr = S_OK;
	GUID guid_network_provider = CLSID_DVBTNetworkProvider;

	hr = Check( guid_network_provider );
	if( p_filter_graph ) {
		hr = Build();
		return !FAILED( hr );
	}
	return false;
}

bool BDAGraph::checkDevice( const std::string &vendorId, const std::string &productId ) {
	return lookUpDevice(KSCATEGORY_BDA_RECEIVER_COMPONENT, vendorId, productId);
}


float BDAGraph::getSignalNoiseRatio(void)
{
	/* not implemented until Windows 7
	 * IBDA_Encoder::GetState */
	return 0.;
}

float BDAGraph::getSignalStrength(void)
{
	HRESULT hr = S_OK;
	long l_strength = 0;
	LTRACE( "BDAGraph", "GetSignalStrength: entering" );
	if( !p_scanning_tuner)
		return 0.;
	hr = p_scanning_tuner->get_SignalStrength( &l_strength );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "GetSignalStrength: "
			"Cannot get value: hr=0x%8lx", hr );
		return 0.;
	}
	LTRACE( "BDAGraph", "GetSignalStrength: got %ld", l_strength );
	if( l_strength == -1 )
		return -1.;
	return l_strength / 100.;
}

/*****************************************************************************
* Set DVB-T
*
* This provides the tune request that everything else is built upon.
*
* Stores the tune request to the scanning tuner, where it is pulled out by
* dvb_tune a/k/a SubmitTuneRequest.
******************************************************************************/
bool BDAGraph::setFrequency(int freq, int khBandwidth)
//int BDAGraph::SetDVBT(long l_frequency, uint32_t fec_hp, uint32_t fec_lp,
//    long l_bandwidth, int transmission, uint32_t guard, int hierarchy)
{
	HRESULT hr = S_OK;

	class localComPtr
	{
		public:
		ITuneRequest*       p_tune_request;
		IDVBTuneRequest*    p_dvb_tune_request;
		IDVBTLocator*       p_dvbt_locator;
		IDVBTuningSpace2*   p_dvb_tuning_space;
		localComPtr():
			p_tune_request(NULL),
			p_dvb_tune_request(NULL),
			p_dvbt_locator(NULL),
			p_dvb_tuning_space(NULL)
			{};
		~localComPtr()
		{
			if( p_tune_request )
				p_tune_request->Release();
			if( p_dvb_tune_request )
				p_dvb_tune_request->Release();
			if( p_dvbt_locator )
				p_dvbt_locator->Release();
			if( p_dvb_tuning_space )
				p_dvb_tuning_space->Release();
		}
	} l;

	/* create local dvbt-specific tune request and locator
	 * then put it to existing scanning tuner */
	BinaryConvolutionCodeRate i_hp_fec = BDA_BCC_RATE_3_4;
	BinaryConvolutionCodeRate i_lp_fec = BDA_BCC_RATE_MAX; //BDA_BCC_RATE_NOT_SET
	GuardInterval i_guard = BDA_GUARD_1_16;
	TransmissionMode i_transmission = BDA_XMIT_MODE_2K;
	HierarchyAlpha i_hierarchy = BDA_HALPHA_1; //BDA_HALPHA_NOT_DEFINED

	/* try to set p_scanning_tuner */
/*    LDEBUG( "BDAGraph", "SetDVBT: set up scanning tuner" );
	hr = Check( CLSID_DVBTNetworkProvider );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "SetDVBT: "\
			"Cannot create Tuning Space: hr=0x%8lx", hr );
		return false;
	}
*/
	if( !p_scanning_tuner )
	{
		LWARN( "BDAGraph", "SetDVBT: Cannot get scanning tuner" );
		return false;
	}

	hr = p_scanning_tuner->get_TuneRequest( &l.p_tune_request );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "SetDVBT: "\
			"Cannot get Tune Request: hr=0x%8lx", hr );
		return false;
	}

	LDEBUG( "BDAGraph", "SetDVBT: Creating DVB tune request" );
	hr = l.p_tune_request->QueryInterface( IID_IDVBTuneRequest,
		reinterpret_cast<void**>( &l.p_dvb_tune_request ) );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "SetDVBT: "\
			"Cannot QI for IDVBTuneRequest: hr=0x%8lx", hr );
		return false;
	}

	l.p_dvb_tune_request->put_ONID( -1 );
	l.p_dvb_tune_request->put_SID( -1 );
	l.p_dvb_tune_request->put_TSID( -1 );

	LDEBUG( "BDAGraph", "SetDVBT: get TS" );
	hr = p_scanning_tuner->get_TuningSpace( &p_tuning_space );
	if( FAILED( hr ) )
	{
		LDEBUG( "BDAGraph", "SetDVBT: "\
			"cannot get tuning space: hr=0x%8lx", hr );
		return false;
	}

	LDEBUG( "BDAGraph", "SetDVBT: QI to DVBT TS" );
	hr = p_tuning_space->QueryInterface( IID_IDVBTuningSpace2,
		reinterpret_cast<void**>( &l.p_dvb_tuning_space ) );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "SetDVBT: "\
			"Cannot QI for IDVBTuningSpace2: hr=0x%8lx", hr );
		return false;
	}

	LDEBUG( "BDAGraph", "SetDVBT: Creating local locator" );
	hr = ::CoCreateInstance( CLSID_DVBTLocator, 0, CLSCTX_INPROC,
		IID_IDVBTLocator, reinterpret_cast<void**>( &l.p_dvbt_locator ) );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "SetDVBT: "\
			"Cannot create the DVBT Locator: hr=0x%8lx", hr );
		return false;
	}

	hr = l.p_dvb_tuning_space->put_SystemType( DVB_Terrestrial );
	//if( SUCCEEDED( hr ) && freq > 0 )
	hr = l.p_dvbt_locator->put_CarrierFrequency( freq ); //*1000
  //  if( SUCCEEDED( hr ) && khBandwidth > 0 )
	hr = l.p_dvbt_locator->put_Bandwidth( khBandwidth/1000 );//khBandwidth/1000 );
/*    if( SUCCEEDED( hr ) && i_hp_fec != BDA_BCC_RATE_NOT_SET )
		hr = l.p_dvbt_locator->put_InnerFECRate( i_hp_fec );
	if( SUCCEEDED( hr ) && i_lp_fec != BDA_BCC_RATE_NOT_SET )
		hr = l.p_dvbt_locator->put_LPInnerFECRate( i_lp_fec );
	if( SUCCEEDED( hr ) && i_guard != BDA_GUARD_NOT_SET )
		hr = l.p_dvbt_locator->put_Guard( i_guard );
	if( SUCCEEDED( hr ) && i_transmission != BDA_XMIT_MODE_NOT_SET )
		hr = l.p_dvbt_locator->put_Mode( i_transmission );
	if( SUCCEEDED( hr ) && i_hierarchy != BDA_HALPHA_NOT_SET )
		hr = l.p_dvbt_locator->put_HAlpha( i_hierarchy );
*/
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "SetDVBT: "\
			"Cannot set tuning parameters on Locator: hr=0x%8lx", hr );
		return false;
	}

	LDEBUG( "BDAGraph", "SetDVBT: putting DVBT locator into local tune request" );

	hr = l.p_dvb_tune_request->put_Locator( l.p_dvbt_locator );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "SetDVBT: "\
			"Cannot put the locator: hr=0x%8lx", hr );
		return false;
	}

	LDEBUG( "BDAGraph", "SetDVBT: putting local Tune Request to scanning tuner" );
	hr = p_scanning_tuner->Validate( l.p_dvb_tune_request );
	if( FAILED( hr ) )
	{
		LDEBUG( "BDAGraph", "SetDVBT: "\
			"Tune Request cannot be validated: hr=0x%8lx", hr );
	}
	/* increments ref count for scanning tuner */
	hr = p_scanning_tuner->put_TuneRequest( l.p_dvb_tune_request );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "SetDVBT: "\
			"Cannot put the tune request: hr=0x%8lx", hr );
		return false;
	}

	LDEBUG( "BDAGraph", "SetDVBT: return success" );

	hr = Start();
	return !FAILED( hr );
}


/*****************************************************************************
* SetUpTuner
******************************************************************************
* Sets up global p_scanning_tuner and sets guid_network_type according
* to the Network Type requested.
*
* Logic: if tuner is set up and is the right network type, use it.
* Otherwise, poll the tuner for the right tuning space.
*
* Then set up a tune request and try to validate it. Finally, put
* tune request and tuning space to tuner
*
* on success, sets globals: p_scanning_tuner and guid_network_type
*
******************************************************************************/
HRESULT BDAGraph::SetUpTuner( REFCLSID guid_this_network_type )
{
	HRESULT hr = S_OK;
	class localComPtr
	{
		public:
		ITuningSpaceContainer*      p_tuning_space_container;
		IEnumTuningSpaces*          p_tuning_space_enum;
		ITuningSpace*               p_test_tuning_space;
		ITuneRequest*               p_tune_request;
		IDVBTuneRequest*            p_dvb_tune_request;

		IDigitalCableTuneRequest*   p_cqam_tune_request;
		IATSCChannelTuneRequest*    p_atsc_tune_request;
		ILocator*                   p_locator;
		IDVBTLocator*               p_dvbt_locator;
		IDVBCLocator*               p_dvbc_locator;
		IDVBSLocator*               p_dvbs_locator;

		BSTR                        bstr_name;

		CLSID                       guid_test_network_type;
		char*                       psz_network_name;
		char*                       psz_bstr_name;
		int                         i_name_len;

		localComPtr():
			p_tuning_space_container(NULL),
			p_tuning_space_enum(NULL),
			p_test_tuning_space(NULL),
			p_tune_request(NULL),
			p_dvb_tune_request(NULL),
			p_cqam_tune_request(NULL),
			p_atsc_tune_request(NULL),
			p_locator(NULL),
			p_dvbt_locator(NULL),
			p_dvbc_locator(NULL),
			p_dvbs_locator(NULL),
			bstr_name(NULL),
			guid_test_network_type(GUID_NULL),
			psz_network_name(NULL),
			psz_bstr_name(NULL),
			i_name_len(0)
		{}
		~localComPtr()
		{
			if( p_tuning_space_enum )
				p_tuning_space_enum->Release();
			if( p_tuning_space_container )
				p_tuning_space_container->Release();
			if( p_test_tuning_space )
				p_test_tuning_space->Release();
			if( p_tune_request )
				p_tune_request->Release();
			if( p_dvb_tune_request )
				p_dvb_tune_request->Release();
			if( p_cqam_tune_request )
				p_cqam_tune_request->Release();
			if( p_atsc_tune_request )
				p_atsc_tune_request->Release();
			if( p_locator )
				p_locator->Release();
			if( p_dvbt_locator )
				p_dvbt_locator->Release();
			if( p_dvbc_locator )
				p_dvbc_locator->Release();
			if( p_dvbs_locator )
				p_dvbs_locator->Release();
			SysFreeString( bstr_name );
			delete[] psz_bstr_name;
			free( psz_network_name );
		}
	} l;

	LDEBUG( "BDAGraph", "SetUpTuner: entering" );


	/* We shall test for a specific Tuning space name supplied on the command
	 * line as dvb-network-name=xxx.
	 * For some users with multiple cards and/or multiple networks this could
	 * be useful. This allows us to reasonably safely apply updates to the
	 * System Tuning Space in the registry without disrupting other streams. */

	l.psz_network_name = NULL; //var_GetNonEmptyString( p_access, "dvb-network-name" );

	if( l.psz_network_name )
	{
		LDEBUG( "BDAGraph", "SetUpTuner: Find Tuning Space: %s",
			l.psz_network_name );
	}
	else
	{
		l.psz_network_name = new char[1];
		*l.psz_network_name = '\0';
	}

	/* Tuner may already have been set up. If it is for the same
	 * network type then all is well. Otherwise, reset the Tuning Space and get
	 * a new one */
	LDEBUG( "BDAGraph", "SetUpTuner: Checking for tuning space" );
	if( !p_scanning_tuner )
	{
		LWARN( "BDAGraph", "SetUpTuner: "\
			"Cannot find scanning tuner" );
		return E_FAIL;
	}

	if( p_tuning_space )
	{
		LDEBUG( "BDAGraph", "SetUpTuner: get network type" );
		hr = p_tuning_space->get__NetworkType( &l.guid_test_network_type );
		if( FAILED( hr ) )
		{
			LWARN( "BDAGraph", "Check: "\
				"Cannot get network type: hr=0x%8lx", hr );
			l.guid_test_network_type = GUID_NULL;
		}

		LDEBUG( "BDAGraph", "SetUpTuner: see if it's the right one" );
		if( l.guid_test_network_type == guid_this_network_type )
		{
			LDEBUG( "BDAGraph", "SetUpTuner: it's the right one" );
			SysFreeString( l.bstr_name );

			hr = p_tuning_space->get_UniqueName( &l.bstr_name );
			if( FAILED( hr ) )
			{
				/* should never fail on a good tuning space */
				LDEBUG( "BDAGraph", "SetUpTuner: "\
					"Cannot get UniqueName for Tuning Space: hr=0x%8lx", hr );
				goto NoTuningSpace;
			}

			/* Test for a specific Tuning space name supplied on the command
			 * line as dvb-network-name=xxx */
			if( l.psz_bstr_name )
				delete[] l.psz_bstr_name;
			l.i_name_len = WideCharToMultiByte( CP_ACP, 0, l.bstr_name, -1,
				l.psz_bstr_name, 0, NULL, NULL );
			l.psz_bstr_name = new char[ l.i_name_len ];
			l.i_name_len = WideCharToMultiByte( CP_ACP, 0, l.bstr_name, -1,
				l.psz_bstr_name, l.i_name_len, NULL, NULL );

			/* if no name was requested on command line, or if the name
			 * requested equals the name of this space, we are OK */
			if( *l.psz_network_name == '\0' ||
				strcmp( l.psz_network_name, l.psz_bstr_name ) == 0 )
			{
				LDEBUG( "BDAGraph", "SetUpTuner: Using Tuning Space: %s",
					l.psz_bstr_name );
				/* p_tuning_space and guid_network_type are already set */
				/* you probably already have a tune request, also */
				hr = p_scanning_tuner->get_TuneRequest( &l.p_tune_request );
				if( SUCCEEDED( hr ) )
				{
					return S_OK;
				}
				/* CreateTuneRequest adds l.p_tune_request to p_tuning_space
				 * l.p_tune_request->RefCount = 1 */
				hr = p_tuning_space->CreateTuneRequest( &l.p_tune_request );
				if( SUCCEEDED( hr ) )
				{
					return S_OK;
				}
				LWARN( "BDAGraph", "SetUpTuner: "\
					"Cannot Create Tune Request: hr=0x%8lx", hr );
			   /* fall through to NoTuningSpace */
			}
		}

		/* else different guid_network_type */
	NoTuningSpace:
		if( p_tuning_space )
			p_tuning_space->Release();
		p_tuning_space = NULL;
		/* pro forma; should have returned S_OK if we created this */
		if( l.p_tune_request )
			l.p_tune_request->Release();
		l.p_tune_request = NULL;
	}


	/* p_tuning_space is null at this point; we have already
	   returned S_OK if it was good. So find a tuning
	   space on the scanning tuner. */

	LDEBUG( "BDAGraph", "SetUpTuner: release TuningSpaces Enumerator" );
	if( l.p_tuning_space_enum )
		l.p_tuning_space_enum->Release();
	LDEBUG( "BDAGraph", "SetUpTuner: nullify TuningSpaces Enumerator" );
	l.p_tuning_space_enum = NULL;
	LDEBUG( "BDAGraph", "SetUpTuner: create TuningSpaces Enumerator" );

	hr = p_scanning_tuner->EnumTuningSpaces( &l.p_tuning_space_enum );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "SetUpTuner: "\
			"Cannot create TuningSpaces Enumerator: hr=0x%8lx", hr );
		goto TryToClone;
	}

	do
	{
		LDEBUG( "BDAGraph", "SetUpTuner: top of loop" );
		l.guid_test_network_type = GUID_NULL;
		if( l.p_test_tuning_space )
			l.p_test_tuning_space->Release();
		l.p_test_tuning_space = NULL;
		if( p_tuning_space )
			p_tuning_space->Release();
		p_tuning_space = NULL;
		SysFreeString( l.bstr_name );
		LDEBUG( "BDAGraph", "SetUpTuner: need good TS enum" );
		if( !l.p_tuning_space_enum ) break;
		LDEBUG( "BDAGraph", "SetUpTuner: next tuning space" );
		hr = l.p_tuning_space_enum->Next( 1, &l.p_test_tuning_space, NULL );
		if( hr != S_OK ) break;
		LDEBUG( "BDAGraph", "SetUpTuner: get network type" );
		hr = l.p_test_tuning_space->get__NetworkType( &l.guid_test_network_type );
		if( FAILED( hr ) )
		{
			LWARN( "BDAGraph", "Check: "\
				"Cannot get network type: hr=0x%8lx", hr );
			l.guid_test_network_type = GUID_NULL;
		}
		if( l.guid_test_network_type == guid_this_network_type )
		{
			LDEBUG( "BDAGraph", "SetUpTuner: Found matching space on tuner" );

			SysFreeString( l.bstr_name );
			LDEBUG( "BDAGraph", "SetUpTuner: get unique name" );

			hr = l.p_test_tuning_space->get_UniqueName( &l.bstr_name );
			if( FAILED( hr ) )
			{
				/* should never fail on a good tuning space */
				LDEBUG( "BDAGraph", "SetUpTuner: "\
					"Cannot get UniqueName for Tuning Space: hr=0x%8lx", hr );
				continue;
			}
			LDEBUG( "BDAGraph", "SetUpTuner: convert w to multi" );
			if ( l.psz_bstr_name )
				delete[] l.psz_bstr_name;
			l.i_name_len = WideCharToMultiByte( CP_ACP, 0, l.bstr_name, -1,
				l.psz_bstr_name, 0, NULL, NULL );
			l.psz_bstr_name = new char[ l.i_name_len ];
			l.i_name_len = WideCharToMultiByte( CP_ACP, 0, l.bstr_name, -1,
				l.psz_bstr_name, l.i_name_len, NULL, NULL );
			LDEBUG( "BDAGraph", "SetUpTuner: Using Tuning Space: %s",
				l.psz_bstr_name );
			break;
		}

	}
	while( true );
	LDEBUG( "BDAGraph", "SetUpTuner: checking what we got" );

	if( l.guid_test_network_type == GUID_NULL)
	{
		LDEBUG( "BDAGraph", "SetUpTuner: got null, try to clone" );
		goto TryToClone;
	}

	LDEBUG( "BDAGraph", "SetUpTuner: put TS" );
	hr = p_scanning_tuner->put_TuningSpace( l.p_test_tuning_space );
	if( FAILED( hr ) )
	{
		LDEBUG( "BDAGraph", "SetUpTuner: "\
			"cannot put tuning space: hr=0x%8lx", hr );
		goto TryToClone;
	}

	LDEBUG( "BDAGraph", "SetUpTuner: get default locator" );
	hr = l.p_test_tuning_space->get_DefaultLocator( &l.p_locator );
	if( FAILED( hr ) )
	{
		LDEBUG( "BDAGraph", "SetUpTuner: "\
			"cannot get default locator: hr=0x%8lx", hr );
		goto TryToClone;
	}

	LDEBUG( "BDAGraph", "SetUpTuner: create tune request" );
	hr = l.p_test_tuning_space->CreateTuneRequest( &l.p_tune_request );
	if( FAILED( hr ) )
	{
		LDEBUG( "BDAGraph", "SetUpTuner: "\
			"cannot create tune request: hr=0x%8lx", hr );
		goto TryToClone;
	}

	LDEBUG( "BDAGraph", "SetUpTuner: put locator" );
	hr = l.p_tune_request->put_Locator( l.p_locator );
	if( FAILED( hr ) )
	{
		LDEBUG( "BDAGraph", "SetUpTuner: "\
			"cannot put locator: hr=0x%8lx", hr );
		goto TryToClone;
	}

	LDEBUG( "BDAGraph", "SetUpTuner: try to validate tune request" );
	hr = p_scanning_tuner->Validate( l.p_tune_request );
	if( FAILED( hr ) )
	{
		LDEBUG( "BDAGraph", "SetUpTuner: "\
			"Tune Request cannot be validated: hr=0x%8lx", hr );
	}

	LDEBUG( "BDAGraph", "SetUpTuner: Attach tune request to Scanning Tuner");
	/* increments ref count for scanning tuner */
	hr = p_scanning_tuner->put_TuneRequest( l.p_tune_request );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "SetUpTuner: "\
			"Cannot submit the tune request: hr=0x%8lx", hr );
		return hr;
	}

	LDEBUG( "BDAGraph", "SetUpTuner: Tuning Space and Tune Request created" );
	return S_OK;

	/* Get the SystemTuningSpaces container
	 * p_tuning_space_container->Refcount = 1  */
TryToClone:
	LWARN( "BDAGraph", "SetUpTuner: won't try to clone " );
	return E_FAIL;
}

/******************************************************************************
* Check
*******************************************************************************
* Check if tuner supports this network type
*
* on success, sets globals:
* systems, l_tuner_used, p_network_provider, p_scanning_tuner, p_tuner_device,
* p_tuning_space, p_filter_graph
******************************************************************************/
HRESULT BDAGraph::Check( REFCLSID guid_this_network_type )
{
	HRESULT hr = S_OK;

	class localComPtr
	{
		public:
		ITuningSpaceContainer*  p_tuning_space_container;

		localComPtr():
			 p_tuning_space_container(NULL)
		{};
		~localComPtr()
		{
			if( p_tuning_space_container )
				p_tuning_space_container->Release();
		}
	} l;

	LDEBUG( "BDAGraph", "Check: entering ");

	/* Note that the systems global is persistent across Destroy().
	 * So we need to see if a tuner has been physically removed from
	 * the system since the last Check. Before we do anything,
	 * assume that this Check will fail and remove this network type
	 * from systems. It will be restored if the Check passes.
	 */


	/* If we have already have a filter graph, rebuild it*/
	LDEBUG( "BDAGraph", "Check: Destroying filter graph" );
	if( p_filter_graph )
		Destroy();
	p_filter_graph = NULL;
	hr = ::CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC,
		IID_IGraphBuilder, reinterpret_cast<void**>( &p_filter_graph ) );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "Check: "\
			"Cannot CoCreate IFilterGraph: hr=0x%8lx", hr );
		return hr;
	}

	/* First filter in the graph is the Network Provider and
	 * its Scanning Tuner which takes the Tune Request */
	if( p_network_provider )
		p_network_provider->Release();
	p_network_provider = NULL;
	hr = ::CoCreateInstance( guid_this_network_type, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, reinterpret_cast<void**>( &p_network_provider ) );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "Check: "\
			"Cannot CoCreate Network Provider: hr=0x%8lx", hr );
		return hr;
	}

	LDEBUG( "BDAGraph", "Check: adding Network Provider to graph");
	hr = p_filter_graph->AddFilter( p_network_provider, L"Network Provider" );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "Check: "\
			"Cannot load network provider: hr=0x%8lx", hr );
		return hr;
	}

	/* Add the Network Tuner to the Network Provider. On subsequent calls,
	 * l_tuner_used will cause a different tuner to be selected.
	 *
	 * To select a specific device first get the parameter that nominates the
	 * device (dvb-adapter) and use the value to initialise l_tuner_used.
	 * Note that dvb-adapter is 1-based, while l_tuner_used is 0-based.
	 * When FindFilter returns, check the contents of l_tuner_used.
	 * If it is not what was selected, then the requested device was not
	 * available, so return with an error. */

	long l_adapter = -1;
	l_adapter = 0; //var_GetInteger( p_access, "dvb-adapter" );
	if( l_tuner_used < 0 && l_adapter >= 0 )
		l_tuner_used = l_adapter - 1;

	/* If tuner is in cold state, we have to do a successful put_TuneRequest
	 * before it will Connect. */
	LDEBUG( "BDAGraph", "Check: Creating Scanning Tuner");
	if( p_scanning_tuner )
		p_scanning_tuner->Release();
	p_scanning_tuner = NULL;
	hr = p_network_provider->QueryInterface( IID_IScanningTuner,
		reinterpret_cast<void**>( &p_scanning_tuner ) );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "Check: "\
			"Cannot QI Network Provider for Scanning Tuner: hr=0x%8lx", hr );
		return hr;
	}

	/* try to set up p_scanning_tuner */
	LDEBUG( "BDAGraph", "Check: Calling SetUpTuner" );
	hr = SetUpTuner( guid_this_network_type );
	if( FAILED( hr ) )
	{
		LDEBUG( "BDAGraph", "Check: "\
			"Cannot set up scanner in Check mode: hr=0x%8lx", hr );
		return hr;
	}

	LDEBUG( "BDAGraph", "Check: "\
		"Calling FindFilter for KSCATEGORY_BDA_NETWORK_TUNER with "\
		"p_network_provider; l_tuner_used=%ld", l_tuner_used );
	hr = FindFilter( KSCATEGORY_BDA_NETWORK_TUNER, &l_tuner_used,
		p_network_provider, &p_tuner_device );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "Check: "\
			"Cannot load tuner device and connect network provider: "\
			"hr=0x%8lx", hr );
		return hr;
	}

	if( l_adapter > 0 && l_tuner_used != l_adapter )
	{
		 LWARN( "BDAGraph", "Check: "\
			 "Tuner device %ld is not available", l_adapter );
		 return E_FAIL;
	}

	LDEBUG( "BDAGraph", "Check: Using adapter %ld", l_tuner_used );
	/* success!
	 * already set l_tuner_used,
	 * p_tuning_space
	 */
	LDEBUG( "BDAGraph", "Check: check succeeded: hr=0x%8lx", hr );
	return S_OK;
}


/******************************************************************************
* Build
*******************************************************************************
* Build the Filter Graph
*
* connects filters and
* creates the media control and registers the graph
* on success, sets globals:
* d_graph_register, p_media_control, p_grabber, p_sample_grabber,
* p_mpeg_demux, p_transport_info
******************************************************************************/
HRESULT BDAGraph::Build()
{
	HRESULT hr = S_OK;
	long            l_capture_used;
	long            l_tif_used;
	AM_MEDIA_TYPE   grabber_media_type;

	class localComPtr
	{
		public:
		ITuningSpaceContainer*  p_tuning_space_container;
		localComPtr():
			p_tuning_space_container(NULL)
		{};
		~localComPtr()
		{
			if( p_tuning_space_container )
				p_tuning_space_container->Release();
		}
	} l;

	LDEBUG( "BDAGraph", "Build: entering");

	/* at this point, you've connected to a scanning tuner of the right
	 * network type.
	 */
	if( !p_scanning_tuner || !p_tuner_device )
	{
		LWARN( "BDAGraph", "Build: "\
			"Scanning Tuner does not exist" );
		return E_FAIL;
	}

	hr = p_scanning_tuner->get_TuneRequest( &p_tune_request );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "Build: no tune request" );
		return hr;
	}
	hr = p_scanning_tuner->get_TuningSpace( &p_tuning_space );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "Build: no tuning space" );
		return hr;
	}
	hr = p_tuning_space->get__NetworkType( &guid_network_type );


	/* Always look for all capture devices to match the Network Tuner */
	l_capture_used = -1;
	LDEBUG( "BDAGraph", "Build: "\
		"Calling FindFilter for KSCATEGORY_BDA_RECEIVER_COMPONENT with "\
		"p_tuner_device; l_capture_used=%ld", l_capture_used );
	hr = FindFilter( KSCATEGORY_BDA_RECEIVER_COMPONENT, &l_capture_used,
		p_tuner_device, &p_capture_device );
	if( FAILED( hr ) )
	{
		/* Some BDA drivers do not provide a Capture Device Filter so force
		 * the Sample Grabber to connect directly to the Tuner Device */
		LDEBUG( "BDAGraph", "Build: "\
			"Cannot find Capture device. Connect to tuner "\
			"and AddRef() : hr=0x%8lx", hr );
		p_capture_device = p_tuner_device;
		p_capture_device->AddRef();
	}


	if( p_sample_grabber )
		 p_sample_grabber->Release();
	p_sample_grabber = NULL;
	/* Insert the Sample Grabber to tap into the Transport Stream. */
	hr = ::CoCreateInstance( CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, reinterpret_cast<void**>( &p_sample_grabber ) );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "Build: "\
			"Cannot load Sample Grabber Filter: hr=0x%8lx", hr );
		return hr;
	}

	hr = p_filter_graph->AddFilter( p_sample_grabber, L"Sample Grabber" );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "Build: "\
			"Cannot add Sample Grabber Filter to graph: hr=0x%8lx", hr );
		return hr;
	}

	// create the sample grabber
	if( p_grabber )
		p_grabber->Release();
	p_grabber = NULL;
	hr = p_sample_grabber->QueryInterface( IID_ISampleGrabber,
		reinterpret_cast<void**>( &p_grabber ) );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "Build: "\
			"Cannot QI Sample Grabber Filter: hr=0x%8lx", hr );
		return hr;
	}

	/* Try the possible stream type */
	hr = E_FAIL;
	for( int i = 0; i < 2; i++ )
	{
		ZeroMemory( &grabber_media_type, sizeof( AM_MEDIA_TYPE ) );
		grabber_media_type.majortype = MEDIATYPE_Stream;
		grabber_media_type.subtype   =  i == 0 ? MEDIASUBTYPE_MPEG2_TRANSPORT : KSDATAFORMAT_SUBTYPE_BDA_MPEG2_TRANSPORT;
		LDEBUG( "BDAGraph", "Build: "
						   "Trying connecting with subtype %s",
						   i == 0 ? "MEDIASUBTYPE_MPEG2_TRANSPORT" : "KSDATAFORMAT_SUBTYPE_BDA_MPEG2_TRANSPORT" );
		hr = p_grabber->SetMediaType( &grabber_media_type );
		if( SUCCEEDED( hr ) )
		{
			hr = Connect( p_capture_device, p_sample_grabber );
			if( SUCCEEDED( hr ) )
			{
				LDEBUG( "BDAGraph", "Build: "\
					"Connected capture device to sample grabber" );
				break;
			}
			LWARN( "BDAGraph", "Build: "\
				"Cannot connect Sample Grabber to Capture device: hr=0x%8lx (try %d/2)", hr, 1+i );
		}
		else
		{
			LWARN( "BDAGraph", "Build: "\
				"Cannot set media type on grabber filter: hr=0x%8lx (try %d/2", hr, 1+i );
		}
	}

	LDEBUG( "BDAGraph", "Build: This is where it used to return upon success" );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "Build: "\
			"Cannot use capture device: hr=0x%8lx", hr );
		return hr;
	}

	/* We need the MPEG2 Demultiplexer even though we are going to use the VLC
	 * TS demuxer. The TIF filter connects to the MPEG2 demux and works with
	 * the Network Provider filter to set up the stream */
	//LDEBUG( "BDAGraph", "Build: using MPEG2 demux" );
	if( p_mpeg_demux )
		p_mpeg_demux->Release();
	p_mpeg_demux = NULL;
	hr = ::CoCreateInstance( CLSID_MPEG2Demultiplexer, NULL,
		CLSCTX_INPROC_SERVER, IID_IBaseFilter,
		reinterpret_cast<void**>( &p_mpeg_demux ) );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "Build: "\
			"Cannot CoCreateInstance MPEG2 Demultiplexer: hr=0x%8lx", hr );
		return hr;
	}

	//LDEBUG( "BDAGraph", "Build: adding demux" );
	hr = p_filter_graph->AddFilter( p_mpeg_demux, L"Demux" );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "Build: "\
			"Cannot add demux filter to graph: hr=0x%8lx", hr );
		return hr;
	}

	hr = Connect( p_sample_grabber, p_mpeg_demux );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "Build: "\
			"Cannot connect demux to grabber: hr=0x%8lx", hr );
		return hr;
	}

	//LDEBUG( "BDAGraph", "Build: Connected sample grabber to demux" );
	/* Always look for the Transport Information Filter from the start
	 * of the collection*/
	l_tif_used = -1;
	LDEBUG( "BDAGraph", "Check: "\
		"Calling FindFilter for KSCATEGORY_BDA_TRANSPORT_INFORMATION with "\
		"p_mpeg_demux; l_tif_used=%ld", l_tif_used );

	hr = FindFilter( KSCATEGORY_BDA_TRANSPORT_INFORMATION, &l_tif_used,
		p_mpeg_demux, &p_transport_info );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "Build: "\
			"Cannot load TIF onto demux: hr=0x%8lx", hr );
		return hr;
	}

	/* Configure the Sample Grabber to buffer the samples continuously */
	hr = p_grabber->SetBufferSamples( true );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "Build: "\
			"Cannot set Sample Grabber to buffering: hr=0x%8lx", hr );
		return hr;
	}

	hr = p_grabber->SetOneShot( false );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "Build: "\
			"Cannot set Sample Grabber to multi shot: hr=0x%8lx", hr );
		return hr;
	}

	/* Second parameter to SetCallback specifies the callback method; 0 uses
	 * the ISampleGrabberCB::SampleCB method, which receives an IMediaSample
	 * pointer. */
	//LDEBUG( "BDAGraph", "Build: Adding grabber callback" );
	hr = p_grabber->SetCallback( this, 0 );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "Build: "\
			"Cannot set SampleGrabber Callback: hr=0x%8lx", hr );
		return hr;
	}

	hr = Register(); /* creates d_graph_register */
	if( FAILED( hr ) )
	{
		d_graph_register = 0;
		LDEBUG( "BDAGraph", "Build: "\
			"Cannot register graph: hr=0x%8lx", hr );
	}

	/* The Media Control is used to Run and Stop the Graph */
	if( p_media_control )
		p_media_control->Release();
	p_media_control = NULL;
	hr = p_filter_graph->QueryInterface( IID_IMediaControl,
		reinterpret_cast<void**>( &p_media_control ) );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "Build: "\
			"Cannot QI Media Control: hr=0x%8lx", hr );
		return hr;
	}

	/* success! */
	LDEBUG( "BDAGraph!!!!!!!!!!", "Build: succeeded: hr=0x%8lx", hr );
	return S_OK;
}

/* debugging */
bool BDAGraph::lookUpDevice( REFCLSID this_clsid, const std::string &vendorId, const std::string &productId ) {
	HRESULT                 hr = S_OK;
	bool result = false;

	class localComPtr
	{
		public:
		ICreateDevEnum*    p_local_system_dev_enum;
		IEnumMoniker*      p_moniker_enum;
		IMoniker*          p_moniker;
		IBaseFilter*       p_filter;
		IBaseFilter*       p_this_filter;
		IBindCtx*          p_bind_context;
		IPropertyBag*      p_property_bag;

		char*              psz_downstream;
		char*              psz_bstr;
		int                i_bstr_len;

		localComPtr():
			p_local_system_dev_enum(NULL),
			p_moniker_enum(NULL),
			p_moniker(NULL),
			p_filter(NULL),
			p_this_filter(NULL),
			p_bind_context( NULL ),
			p_property_bag(NULL),
			psz_downstream( NULL ),
			psz_bstr( NULL )
		{}
		~localComPtr()
		{
			if( p_property_bag )
				p_property_bag->Release();
			if( p_bind_context )
				p_bind_context->Release();
			if( p_filter )
				p_filter->Release();
			if( p_this_filter )
				p_this_filter->Release();
			if( p_moniker )
				p_moniker->Release();
			if( p_moniker_enum )
				p_moniker_enum->Release();
			if( p_local_system_dev_enum )
				p_local_system_dev_enum->Release();
			if( psz_bstr )
				delete[] psz_bstr;
			if( psz_downstream )
				delete[] psz_downstream;
		}
	} l;


//    LDEBUG( "BDAGraph", "ListFilters: Create local system_dev_enum");
	if( l.p_local_system_dev_enum )
		l.p_local_system_dev_enum->Release();
	l.p_local_system_dev_enum = NULL;
	hr = ::CoCreateInstance( CLSID_SystemDeviceEnum, 0, CLSCTX_INPROC,
		IID_ICreateDevEnum, reinterpret_cast<void**>( &l.p_local_system_dev_enum ) );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "ListFilters: "\
			"Cannot CoCreate SystemDeviceEnum: hr=0x%8lx", hr );
		return false;
	}

	//LDEBUG( "BDAGraph", "ListFilters: Create p_moniker_enum");
	if( l.p_moniker_enum )
		l.p_moniker_enum->Release();
	l.p_moniker_enum = NULL;
	hr = l.p_local_system_dev_enum->CreateClassEnumerator( this_clsid,
		&l.p_moniker_enum, 0 );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "ListFilters: "\
			"Cannot CreateClassEnumerator: hr=0x%8lx", hr );
		return false;
	}

	//LDEBUG( "BDAGraph", "ListFilters: Entering main loop" );
	do
	{
		/* We are overwriting l.p_moniker so we should Release and nullify
		 * It is important that p_moniker and p_property_bag are fully released
		 * l.p_filter may not be dereferenced so we could force to NULL */
		/* LDEBUG( "BDAGraph", "ListFilters: top of main loop");*/
		//LDEBUG( "BDAGraph", "ListFilters: releasing property bag");
		if( l.p_property_bag )
			l.p_property_bag->Release();
		l.p_property_bag = NULL;
		//LDEBUG( "BDAGraph", "ListFilters: releasing filter");
		if( l.p_filter )
			l.p_filter->Release();
		l.p_filter = NULL;
		//LDEBUG( "BDAGraph", "ListFilters: releasing bind context");
		if( l.p_bind_context )
		   l.p_bind_context->Release();
		l.p_bind_context = NULL;
		//LDEBUG( "BDAGraph", "ListFilters: releasing moniker");
		if( l.p_moniker )
			l.p_moniker->Release();
		l.p_moniker = NULL;
		//LDEBUG( "BDAGraph", "ListFilters: trying a moniker");

		if( !l.p_moniker_enum ) break;
		hr = l.p_moniker_enum->Next( 1, &l.p_moniker, 0 );
		if( hr != S_OK ) break;

		/* l.p_bind_context is Released at the top of the loop */
		hr = CreateBindCtx( 0, &l.p_bind_context );
		if( FAILED( hr ) )
		{
			LDEBUG( "BDAGraph", "ListFilters: "\
				"Cannot create bind_context, trying another: hr=0x%8lx", hr );
			continue;
		}

		/* l.p_filter is Released at the top of the loop */
		hr = l.p_moniker->BindToObject( l.p_bind_context, NULL, IID_IBaseFilter,
			reinterpret_cast<void**>( &l.p_filter ) );
		if( FAILED( hr ) )
		{
			LDEBUG( "BDAGraph", "ListFilters: "\
				"Cannot create p_filter, trying another: hr=0x%8lx", hr );
			continue;
		}

//#ifdef DEBUG_MONIKER_NAME
		WCHAR*  pwsz_downstream = NULL;

		hr = l.p_moniker->GetDisplayName(l.p_bind_context, NULL,
			&pwsz_downstream );
		if( FAILED( hr ) )
		{
			LDEBUG( "BDAGraph", "ListFilters: "\
				"Cannot get display name, trying another: hr=0x%8lx", hr );
			continue;
		}

		if( l.psz_downstream )
			delete[] l.psz_downstream;
		l.i_bstr_len = WideCharToMultiByte( CP_ACP, 0, pwsz_downstream, -1,
			l.psz_downstream, 0, NULL, NULL );
		l.psz_downstream = new char[ l.i_bstr_len ];
		l.i_bstr_len = WideCharToMultiByte( CP_ACP, 0, pwsz_downstream, -1,
			l.psz_downstream, l.i_bstr_len, NULL, NULL );

		LPMALLOC p_alloc;
		::CoGetMalloc( 1, &p_alloc );
		p_alloc->Free( pwsz_downstream );
		p_alloc->Release();

		std::string info( l.psz_downstream );
		std::size_t vendorFound = info.find(vendorId);
		std::size_t productFound = info.find(productId);
  		result = vendorFound!=std::string::npos && productFound!=std::string::npos;
		LDEBUG( "BDAGraph", "Tuner check=%d, info=%s", result, info.c_str() );

//#else
	//	l.psz_downstream = strdup( "Downstream" );
//#endif
		/* l.p_property_bag is released at the top of the loop */
		hr = l.p_moniker->BindToStorage( NULL, NULL, IID_IPropertyBag,
			reinterpret_cast<void**>( &l.p_property_bag ) );
		if( FAILED( hr ) )
		{
			LDEBUG( "BDAGraph", "ListFilters: "\
				"Cannot Bind to Property Bag: hr=0x%8lx", hr );
			continue;
		}

		//LDEBUG( "BDAGraph", "ListFilters: displaying another" );
	}
	while( true );

	return result;
}

/******************************************************************************
* FindFilter
* Looks up all filters in a category and connects to the upstream filter until
* a successful match is found. The index of the connected filter is returned.
* On subsequent calls, this can be used to start from that point to find
* another match.
* This is used when the graph does not run because a tuner device is in use so
* another one needs to be selected.
******************************************************************************/
HRESULT BDAGraph::FindFilter( REFCLSID this_clsid, long* i_moniker_used,
	IBaseFilter* p_upstream, IBaseFilter** p_p_downstream )
{
	HRESULT                 hr = S_OK;
	int                     i_moniker_index = -1;
	class localComPtr
	{
		public:
		IEnumMoniker*  p_moniker_enum;
		IMoniker*      p_moniker;
		IBindCtx*      p_bind_context;
		IPropertyBag*  p_property_bag;
		char*          psz_upstream;
		int            i_upstream_len;

		char*          psz_downstream;
		VARIANT        var_bstr;
		int            i_bstr_len;
		localComPtr():
			p_moniker_enum(NULL),
			p_moniker(NULL),
			p_bind_context( NULL ),
			p_property_bag(NULL),
			psz_upstream( NULL ),
			psz_downstream( NULL )
		{
			::VariantInit(&var_bstr);
		}
		~localComPtr()
		{
			if( p_moniker_enum )
				p_moniker_enum->Release();
			if( p_moniker )
				p_moniker->Release();
			if( p_bind_context )
				p_bind_context->Release();
			if( p_property_bag )
				p_property_bag->Release();
			if( psz_upstream )
				delete[] psz_upstream;
			if( psz_downstream )
				delete[] psz_downstream;

			::VariantClear(&var_bstr);
		}
	} l;

	/* create system_dev_enum the first time through, or preserve the
	 * existing one to loop through classes */
	if( !p_system_dev_enum )
	{
		LDEBUG( "BDAGraph", "FindFilter: Create p_system_dev_enum");
		hr = ::CoCreateInstance( CLSID_SystemDeviceEnum, 0, CLSCTX_INPROC,
			IID_ICreateDevEnum, reinterpret_cast<void**>( &p_system_dev_enum ) );
		if( FAILED( hr ) )
		{
			LWARN( "BDAGraph", "FindFilter: "\
				"Cannot CoCreate SystemDeviceEnum: hr=0x%8lx", hr );
			return hr;
		}
	}

	LDEBUG( "BDAGraph", "FindFilter: Create p_moniker_enum");
	hr = p_system_dev_enum->CreateClassEnumerator( this_clsid,
		&l.p_moniker_enum, 0 );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "FindFilter: "\
			"Cannot CreateClassEnumerator: hr=0x%8lx", hr );
		return hr;
	}

	LDEBUG( "BDAGraph", "FindFilter: get filter name");
	hr = GetFilterName( p_upstream, &l.psz_upstream );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "FindFilter: "\
			"Cannot GetFilterName: hr=0x%8lx", hr );
		return hr;
	}

	LDEBUG( "BDAGraph", "FindFilter: "\
		"called with i_moniker_used=%ld, " \
		"p_upstream = %s", *i_moniker_used, l.psz_upstream );

	do
	{
		/* We are overwriting l.p_moniker so we should Release and nullify
		 * It is important that p_moniker and p_property_bag are fully released */
		LDEBUG( "BDAGraph", "FindFilter: top of main loop");
		if( l.p_property_bag )
			l.p_property_bag->Release();
		l.p_property_bag = NULL;
		LDEBUG( "BDAGraph", "FindFilter: releasing bind context");
		if( l.p_bind_context )
		   l.p_bind_context->Release();
		l.p_bind_context = NULL;
		LDEBUG( "BDAGraph", "FindFilter: releasing moniker");
		if( l.p_moniker )
			l.p_moniker->Release();
		LDEBUG( "BDAGraph", "FindFilter: null moniker");
		l.p_moniker = NULL;

		LDEBUG( "BDAGraph", "FindFilter: quit if no enum");
		if( !l.p_moniker_enum ) break;
		LDEBUG( "BDAGraph", "FindFilter: trying a moniker");
		hr = l.p_moniker_enum->Next( 1, &l.p_moniker, 0 );
		if( hr != S_OK ) break;

		i_moniker_index++;

		/* Skip over devices already found on previous calls */
		LDEBUG( "BDAGraph", "FindFilter: skip previously found devices");

		if( i_moniker_index <= *i_moniker_used ) continue;
		*i_moniker_used = i_moniker_index;

		/* l.p_bind_context is Released at the top of the loop */
		LDEBUG( "BDAGraph", "FindFilter: create bind context");
		hr = CreateBindCtx( 0, &l.p_bind_context );
		if( FAILED( hr ) )
		{
			LDEBUG( "BDAGraph", "FindFilter: "\
				"Cannot create bind_context, trying another: hr=0x%8lx", hr );
			continue;
		}

		LDEBUG( "BDAGraph", "FindFilter: try to create downstream filter");
		*p_p_downstream = NULL;
		hr = l.p_moniker->BindToObject( l.p_bind_context, NULL, IID_IBaseFilter,
			reinterpret_cast<void**>( p_p_downstream ) );
		if( FAILED( hr ) )
		{
			LDEBUG( "BDAGraph", "FindFilter: "\
				"Cannot bind to downstream, trying another: hr=0x%8lx", hr );
			continue;
		}

#ifdef DEBUG_MONIKER_NAME
		LDEBUG( "BDAGraph", "FindFilter: get downstream filter name");

		WCHAR*  pwsz_downstream = NULL;

		hr = l.p_moniker->GetDisplayName(l.p_bind_context, NULL,
			&pwsz_downstream );
		if( FAILED( hr ) )
		{
			LDEBUG( "BDAGraph", "FindFilter: "\
				"Cannot get display name, trying another: hr=0x%8lx", hr );
			continue;
		}

		if( l.psz_downstream )
			delete[] l.psz_downstream;
		l.i_bstr_len = WideCharToMultiByte( CP_ACP, 0, pwsz_downstream, -1,
			l.psz_downstream, 0, NULL, NULL );
		l.psz_downstream = new char[ l.i_bstr_len ];
		l.i_bstr_len = WideCharToMultiByte( CP_ACP, 0, pwsz_downstream, -1,
			l.psz_downstream, l.i_bstr_len, NULL, NULL );

		LPMALLOC p_alloc;
		::CoGetMalloc( 1, &p_alloc );
		p_alloc->Free( pwsz_downstream );
		p_alloc->Release();
#else
		l.psz_downstream = strdup( "Downstream" );
#endif

		/* l.p_property_bag is released at the top of the loop */
		LDEBUG( "BDAGraph", "FindFilter: "\
			"Moniker name is  %s, binding to storage",  l.psz_downstream );
		hr = l.p_moniker->BindToStorage( l.p_bind_context, NULL,
			IID_IPropertyBag, reinterpret_cast<void**>( &l.p_property_bag ) );
		if( FAILED( hr ) )
		{
			LDEBUG( "BDAGraph", "FindFilter: "\
				"Cannot Bind to Property Bag: hr=0x%8lx", hr );
			continue;
		}

		LDEBUG( "BDAGraph", "FindFilter: read friendly name");
		hr = l.p_property_bag->Read( L"FriendlyName", &l.var_bstr, NULL );
		if( FAILED( hr ) )
		{
		   LDEBUG( "BDAGraph", "FindFilter: "\
			   "Cannot read friendly name, next?: hr=0x%8lx", hr );
		   continue;
		}

		LDEBUG( "BDAGraph", "FindFilter: add filter to graph" );
		hr = p_filter_graph->AddFilter( *p_p_downstream, l.var_bstr.bstrVal );
		if( FAILED( hr ) )
		{
			LDEBUG( "BDAGraph", "FindFilter: "\
				"Cannot add filter, trying another: hr=0x%8lx", hr );
			continue;
		}

		LDEBUG( "BDAGraph", "FindFilter: "\
			"Trying to Connect %s to %s", l.psz_upstream, l.psz_downstream );
		hr = Connect( p_upstream, *p_p_downstream );
		if( SUCCEEDED( hr ) )
		{
			LDEBUG( "BDAGraph", "FindFilter: Connected %s", l.psz_downstream );
			return S_OK;
		}

		/* Not the filter we want so unload and try the next one */
		/* Warning: RemoveFilter does an undocumented Release()
		 * on pointer but does not set it to NULL */
		LDEBUG( "BDAGraph", "FindFilter: Removing filter" );
		hr = p_filter_graph->RemoveFilter( *p_p_downstream );
		if( FAILED( hr ) )
		{
			LWARN( "BDAGraph", "FindFilter: "\
				"Failed unloading Filter: hr=0x%8lx", hr );
			continue;
		}
		LDEBUG( "BDAGraph", "FindFilter: trying another" );
	}
	while( true );

	/* nothing found */
	LWARN( "BDAGraph", "FindFilter: No filter connected" );
	hr = E_FAIL;
	return hr;
}

/*****************************************************************************
* Connect is called from Build to enumerate and connect pins
*****************************************************************************/
HRESULT BDAGraph::Connect( IBaseFilter* p_upstream, IBaseFilter* p_downstream )
{
	HRESULT             hr = E_FAIL;
	class localComPtr
	{
		public:
		IPin*      p_pin_upstream;
		IPin*      p_pin_downstream;
		IEnumPins* p_pin_upstream_enum;
		IEnumPins* p_pin_downstream_enum;
		IPin*      p_pin_temp;
		char*      psz_upstream;
		char*      psz_downstream;

		localComPtr():
			p_pin_upstream(NULL), p_pin_downstream(NULL),
			p_pin_upstream_enum(NULL), p_pin_downstream_enum(NULL),
			p_pin_temp(NULL),
			psz_upstream( NULL ),
			psz_downstream( NULL )
			{ };
		~localComPtr()
		{
			if( p_pin_temp )
				p_pin_temp->Release();
			if( p_pin_downstream )
				p_pin_downstream->Release();
			if( p_pin_upstream )
				p_pin_upstream->Release();
			if( p_pin_downstream_enum )
				p_pin_downstream_enum->Release();
			if( p_pin_upstream_enum )
				p_pin_upstream_enum->Release();
			if( psz_upstream )
				delete[] psz_upstream;
			if( psz_downstream )
				delete[] psz_downstream;
		}
	} l;

	PIN_DIRECTION pin_dir;

	//LDEBUG( "BDAGraph", "Connect: entering" );
	hr = p_upstream->EnumPins( &l.p_pin_upstream_enum );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "Connect: "\
			"Cannot get upstream filter enumerator: hr=0x%8lx", hr );
		return hr;
	}

	do
	{
		/* Release l.p_pin_upstream before next iteration */
		if( l.p_pin_upstream )
			l.p_pin_upstream ->Release();
		l.p_pin_upstream = NULL;
		if( !l.p_pin_upstream_enum ) break;
		hr = l.p_pin_upstream_enum->Next( 1, &l.p_pin_upstream, 0 );
		if( hr != S_OK ) break;

		//LDEBUG( "BDAGraph", "Connect: get pin name");
		hr = GetPinName( l.p_pin_upstream, &l.psz_upstream );
		if( FAILED( hr ) )
		{
			LWARN( "BDAGraph", "Connect: "\
				"Cannot GetPinName: hr=0x%8lx", hr );
			return hr;
		}
		//LDEBUG( "BDAGraph", "Connect: p_pin_upstream = %s", l.psz_upstream );

		hr = l.p_pin_upstream->QueryDirection( &pin_dir );
		if( FAILED( hr ) )
		{
			LWARN( "BDAGraph", "Connect: "\
				"Cannot get upstream filter pin direction: hr=0x%8lx", hr );
			return hr;
		}

		hr = l.p_pin_upstream->ConnectedTo( &l.p_pin_downstream );
		if( SUCCEEDED( hr ) )
		{
			l.p_pin_downstream->Release();
			l.p_pin_downstream = NULL;
		}

		if( FAILED( hr ) && hr != VFW_E_NOT_CONNECTED )
		{
			LWARN( "BDAGraph", "Connect: "\
				"Cannot check upstream filter connection: hr=0x%8lx", hr );
			return hr;
		}

		if( ( pin_dir == PINDIR_OUTPUT ) && ( hr == VFW_E_NOT_CONNECTED ) )
		{
			/* The upstream pin is not yet connected so check each pin on the
			 * downstream filter */
			//LDEBUG( "BDAGraph", "Connect: enumerating downstream pins" );
			hr = p_downstream->EnumPins( &l.p_pin_downstream_enum );
			if( FAILED( hr ) )
			{
				LWARN( "BDAGraph", "Connect: Cannot get "\
					"downstream filter enumerator: hr=0x%8lx", hr );
				return hr;
			}

			do
			{
				/* Release l.p_pin_downstream before next iteration */
				if( l.p_pin_downstream )
					l.p_pin_downstream ->Release();
				l.p_pin_downstream = NULL;
				if( !l.p_pin_downstream_enum ) break;
				hr = l.p_pin_downstream_enum->Next( 1, &l.p_pin_downstream, 0 );
				if( hr != S_OK ) break;

				//LDEBUG( "BDAGraph", "Connect: get pin name");
				hr = GetPinName( l.p_pin_downstream, &l.psz_downstream );
				if( FAILED( hr ) )
				{
					LWARN( "BDAGraph", "Connect: "\
						"Cannot GetPinName: hr=0x%8lx", hr );
					return hr;
				}
/*
				LDEBUG( "BDAGraph", "Connect: Trying p_downstream = %s",
					l.psz_downstream );
*/

				hr = l.p_pin_downstream->QueryDirection( &pin_dir );
				if( FAILED( hr ) )
				{
					LWARN( "BDAGraph", "Connect: Cannot get "\
						"downstream filter pin direction: hr=0x%8lx", hr );
					return hr;
				}

				/* Looking for a free Pin to connect to
				 * A connected Pin may have an reference count > 1
				 * so Release and nullify the pointer */
				hr = l.p_pin_downstream->ConnectedTo( &l.p_pin_temp );
				if( SUCCEEDED( hr ) )
				{
					l.p_pin_temp->Release();
					l.p_pin_temp = NULL;
				}

				if( hr != VFW_E_NOT_CONNECTED )
				{
					if( FAILED( hr ) )
					{
						LWARN( "BDAGraph", "Connect: Cannot check "\
							"downstream filter connection: hr=0x%8lx", hr );
						return hr;
					}
				}

				if( ( pin_dir == PINDIR_INPUT ) &&
					( hr == VFW_E_NOT_CONNECTED ) )
				{
					//LDEBUG( "BDAGraph", "Connect: trying to connect pins" );

					hr = p_filter_graph->ConnectDirect( l.p_pin_upstream,
						l.p_pin_downstream, NULL );
					if( SUCCEEDED( hr ) )
					{
						/* If we arrive here then we have a matching pair of
						 * pins. */
						return S_OK;
					}
				}
				/* If we arrive here it means this downstream pin is not
				 * suitable so try the next downstream pin.
				 * l.p_pin_downstream is released at the top of the loop */
			}
			while( true );
			/* If we arrive here then we ran out of pins before we found a
			 * suitable one. Release outstanding refcounts */
			if( l.p_pin_downstream_enum )
				l.p_pin_downstream_enum->Release();
			l.p_pin_downstream_enum = NULL;
			if( l.p_pin_downstream )
				l.p_pin_downstream->Release();
			l.p_pin_downstream = NULL;
		}
		/* If we arrive here it means this upstream pin is not suitable
		 * so try the next upstream pin
		 * l.p_pin_upstream is released at the top of the loop */
	}
	while( true );
	/* If we arrive here it means we did not find any pair of suitable pins
	 * Outstanding refcounts are released in the destructor */
	//LDEBUG( "BDAGraph", "Connect: No pins connected" );
	return E_FAIL;
}

/*****************************************************************************
* Start uses MediaControl to start the graph
*****************************************************************************/
HRESULT BDAGraph::Start()
{
	HRESULT hr = S_OK;
	OAFilterState i_state; /* State_Stopped, State_Paused, State_Running */

	LDEBUG( "BDAGraph", "Start: entering" );

	if( !p_media_control )
	{
		LWARN( "BDAGraph", "Start: Media Control has not been created" );
		return E_FAIL;
	}

	LDEBUG( "BDAGraph", "Start: Run()" );
	hr = p_media_control->Run();
	if( SUCCEEDED( hr ) )
	{
		LDEBUG( "BDAGraph", "Start: Graph started, hr=0x%lx", hr );
		return S_OK;
	}

	LDEBUG( "BDAGraph", "Start: would not start, will retry" );
	/* Query the state of the graph - timeout after 100 milliseconds */
	while( (hr = p_media_control->GetState( 100, &i_state) ) != S_OK )
	{
		if( FAILED( hr ) )
		{
			LWARN( "BDAGraph",
				"Start: Cannot get Graph state: hr=0x%8lx", hr );
			return hr;
		}
	}

	LDEBUG( "BDAGraph", "Start: got state" );
	if( i_state == State_Running )
	{
		LDEBUG( "BDAGraph", "Graph started after a delay, hr=0x%lx", hr );
		return S_OK;
	}

	/* The Graph is not running so stop it and return an error */
	LWARN( "BDAGraph", "Start: Graph not started: %d", (int)i_state );
	hr = p_media_control->StopWhenReady(); /* Instead of Stop() */
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph",
			"Start: Cannot stop Graph after Run failed: hr=0x%8lx", hr );
		return hr;
	}

	return E_FAIL;
}

/******************************************************************************
* SampleCB - Callback when the Sample Grabber has a sample
******************************************************************************/
STDMETHODIMP BDAGraph::SampleCB( double /*date*/, IMediaSample *p_sample )
{
	if( p_sample->IsDiscontinuity() == S_OK )
		LWARN( "BDAGraph", "BDA SampleCB: Sample Discontinuity.");

	util::Buffer *buf=NULL;
	buf = _demux->allocBuffer();
	DTV_ASSERT(buf);

	//  Read from network
	const size_t len = p_sample->GetActualDataLength();
	if (len<0) {
		_demux->freeBuffer( buf );
	}
	else if (len) {// && (getSignalStrength() > 0.0)) {
		//  Enqueue buffer into demuxer
//		LERROR( "BDAGraph", "BDA SampleCB: len=%d, capacity=%d", len, buf->capacity());
		buf->resize( len );
		BYTE *p_sample_data;
		p_sample->GetPointer( &p_sample_data );
		buf->swap( p_sample_data, len );
		_demux->pushData( buf );
		buf = NULL;
	}
	return S_OK;
}

STDMETHODIMP BDAGraph::BufferCB( double /*date*/, BYTE* /*buffer*/,
								 long /*buffer_len*/ )
{
	return E_FAIL;
}

/******************************************************************************
* removes each filter from the graph
******************************************************************************/
HRESULT BDAGraph::Destroy()
{
	HRESULT hr = S_OK;
	ULONG mem_ref = 0;
//    LDEBUG( "BDAGraph", "Destroy: media control 1" );
	if( p_media_control )
		p_media_control->StopWhenReady(); /* Instead of Stop() */

//    LDEBUG( "BDAGraph", "Destroy: deregistering graph" );
	if( d_graph_register )
		Deregister();

//    LDEBUG( "BDAGraph", "Destroy: calling Empty" );
//    output.Empty();

//    LDEBUG( "BDAGraph", "Destroy: TIF" );
	if( p_transport_info )
	{
		/* Warning: RemoveFilter does an undocumented Release()
		 * on pointer but does not set it to NULL */
		hr = p_filter_graph->RemoveFilter( p_transport_info );
		if( FAILED( hr ) )
		{
			LDEBUG( "BDAGraph", "Destroy: "\
				"Failed unloading TIF: hr=0x%8lx", hr );
		}
		p_transport_info = NULL;
	}

//    LDEBUG( "BDAGraph", "Destroy: demux" );
	if( p_mpeg_demux )
	{
		p_filter_graph->RemoveFilter( p_mpeg_demux );
		if( FAILED( hr ) )
		{
			LDEBUG( "BDAGraph", "Destroy: "\
				"Failed unloading demux: hr=0x%8lx", hr );
		}
		p_mpeg_demux = NULL;
	}

//    LDEBUG( "BDAGraph", "Destroy: sample grabber" );
	if( p_grabber )
	{
		mem_ref = p_grabber->Release();
		if( mem_ref != 0 )
		{
			LDEBUG( "BDAGraph", "Destroy: "\
				"Sample grabber mem_ref (varies): mem_ref=%ld", mem_ref );
		}
		p_grabber = NULL;
	}

//    LDEBUG( "BDAGraph", "Destroy: sample grabber filter" );
	if( p_sample_grabber )
	{
		hr = p_filter_graph->RemoveFilter( p_sample_grabber );
		p_sample_grabber = NULL;
		if( FAILED( hr ) )
		{
			LDEBUG( "BDAGraph", "Destroy: "\
				"Failed unloading sampler: hr=0x%8lx", hr );
		}
	}

//    LDEBUG( "BDAGraph", "Destroy: capture device" );
	if( p_capture_device )
	{
		p_filter_graph->RemoveFilter( p_capture_device );
		if( FAILED( hr ) )
		{
			LDEBUG( "BDAGraph", "Destroy: "\
				"Failed unloading capture device: hr=0x%8lx", hr );
		}
		p_capture_device = NULL;
	}

//    LDEBUG( "BDAGraph", "Destroy: tuner device" );
	if( p_tuner_device )
	{
		//LDEBUG( "BDAGraph", "Destroy: remove filter on tuner device" );
		hr = p_filter_graph->RemoveFilter( p_tuner_device );
		//LDEBUG( "BDAGraph", "Destroy: force tuner device to NULL" );
		p_tuner_device = NULL;
		if( FAILED( hr ) )
		{
			LDEBUG( "BDAGraph", "Destroy: "\
				"Failed unloading tuner device: hr=0x%8lx", hr );
		}
	}

//    LDEBUG( "BDAGraph", "Destroy: scanning tuner" );
	if( p_scanning_tuner )
	{
		mem_ref = p_scanning_tuner->Release();
		if( mem_ref != 0 )
		{
			LDEBUG( "BDAGraph", "Destroy: "\
				"Scanning tuner mem_ref (normally 2 if warm, "\
				"3 if active): mem_ref=%ld", mem_ref );
		}
		p_scanning_tuner = NULL;
	}

//    LDEBUG( "BDAGraph", "Destroy: net provider" );
	if( p_network_provider )
	{
		hr = p_filter_graph->RemoveFilter( p_network_provider );
		p_network_provider = NULL;
		if( FAILED( hr ) )
		{
			LDEBUG( "BDAGraph", "Destroy: "\
				"Failed unloading net provider: hr=0x%8lx", hr );
		}
	}

//    LDEBUG( "BDAGraph", "Destroy: filter graph" );
	if( p_filter_graph )
	{
		mem_ref = p_filter_graph->Release();
		if( mem_ref != 0 )
		{
			LDEBUG( "BDAGraph", "Destroy: "\
				"Filter graph mem_ref (normally 1 if active): mem_ref=%ld",
				mem_ref );
		}
		p_filter_graph = NULL;
	}

	/* first call to FindFilter creates p_system_dev_enum */

//    LDEBUG( "BDAGraph", "Destroy: system dev enum" );
	if( p_system_dev_enum )
	{
		mem_ref = p_system_dev_enum->Release();
		if( mem_ref != 0 )
		{
			LDEBUG( "BDAGraph", "Destroy: "\
				"System_dev_enum mem_ref: mem_ref=%ld", mem_ref );
		}
		p_system_dev_enum = NULL;
	}

//    LDEBUG( "BDAGraph", "Destroy: media control 2" );
	if( p_media_control )
	{
		LDEBUG( "BDAGraph", "Destroy: release media control" );
		mem_ref = p_media_control->Release();
		if( mem_ref != 0 )
		{
			LDEBUG( "BDAGraph", "Destroy: "\
				"Media control mem_ref: mem_ref=%ld", mem_ref );
		}
		LDEBUG( "BDAGraph", "Destroy: force media control to NULL" );
		p_media_control = NULL;
	}

	d_graph_register = 0;
	l_tuner_used = -1;
	guid_network_type = GUID_NULL;

//    LDEBUG( "BDAGraph", "Destroy: returning" );
	return S_OK;
}

/*****************************************************************************
* Add/Remove a DirectShow filter graph to/from the Running Object Table.
* Allows GraphEdit to "spy" on a remote filter graph.
******************************************************************************/
HRESULT BDAGraph::Register()
{
	class localComPtr
	{
		public:
		IMoniker*             p_moniker;
		IRunningObjectTable*  p_ro_table;
		localComPtr():
			p_moniker(NULL),
			p_ro_table(NULL)
		{};
		~localComPtr()
		{
			if( p_moniker )
				p_moniker->Release();
			if( p_ro_table )
				p_ro_table->Release();
		}
	} l;
	WCHAR     pwsz_graph_name[128];
	HRESULT   hr;

	hr = ::GetRunningObjectTable( 0, &l.p_ro_table );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "Register: Cannot get ROT: hr=0x%8lx", hr );
		return hr;
	}

	size_t len = sizeof(pwsz_graph_name) / sizeof(pwsz_graph_name[0]);
	_snwprintf( pwsz_graph_name, len - 1, L"VLC BDA Graph %08x Pid %08x",
		(DWORD_PTR) p_filter_graph, ::GetCurrentProcessId() );
	pwsz_graph_name[len-1] = 0;
	hr = CreateItemMoniker( L"!", pwsz_graph_name, &l.p_moniker );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "Register: Cannot Create Moniker: hr=0x%8lx", hr );
		return hr;
	}
	hr = l.p_ro_table->Register( ROTFLAGS_REGISTRATIONKEEPSALIVE,
		p_filter_graph, l.p_moniker, &d_graph_register );
	if( FAILED( hr ) )
	{
		LWARN( "BDAGraph", "Register: Cannot Register Graph: hr=0x%8lx", hr );
		return hr;
	}

	LDEBUG( "BDAGraph", "Register: registered Graph: %S", pwsz_graph_name );
	return hr;
}

void BDAGraph::Deregister()
{
	HRESULT   hr;
	IRunningObjectTable* p_ro_table;
	hr = ::GetRunningObjectTable( 0, &p_ro_table );
	/* docs say this does a Release() on d_graph_register stuff */
	if( SUCCEEDED( hr ) )
		p_ro_table->Revoke( d_graph_register );
	d_graph_register = 0;
	p_ro_table->Release();
}

HRESULT BDAGraph::GetFilterName( IBaseFilter* p_filter, char** psz_bstr_name )
{
	FILTER_INFO filter_info;
	HRESULT     hr = S_OK;

	hr = p_filter->QueryFilterInfo(&filter_info);
	if( FAILED( hr ) )
		return hr;
	int i_name_len = WideCharToMultiByte( CP_ACP, 0, filter_info.achName,
		-1, *psz_bstr_name, 0, NULL, NULL );
	*psz_bstr_name = new char[ i_name_len ];
	i_name_len = WideCharToMultiByte( CP_ACP, 0, filter_info.achName,
		-1, *psz_bstr_name, i_name_len, NULL, NULL );

	// The FILTER_INFO structure holds a pointer to the Filter Graph
	// Manager, with a reference count that must be released.
	if( filter_info.pGraph )
		filter_info.pGraph->Release();
	return S_OK;
}

HRESULT BDAGraph::GetPinName( IPin* p_pin, char** psz_bstr_name )
{
	PIN_INFO    pin_info;
	HRESULT     hr = S_OK;

	hr = p_pin->QueryPinInfo(&pin_info);
	if( FAILED( hr ) )
		return hr;
	int i_name_len = WideCharToMultiByte( CP_ACP, 0, pin_info.achName,
		-1, *psz_bstr_name, 0, NULL, NULL );
	*psz_bstr_name = new char[ i_name_len ];
	i_name_len = WideCharToMultiByte( CP_ACP, 0, pin_info.achName,
		-1, *psz_bstr_name, i_name_len, NULL, NULL );

	// The PIN_INFO structure holds a pointer to the Filter,
	// with a referenppce count that must be released.
	if( pin_info.pFilter )
		pin_info.pFilter->Release();
	return S_OK;
}
