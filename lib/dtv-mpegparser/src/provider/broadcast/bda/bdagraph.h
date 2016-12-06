using namespace std;

#include <wtypes.h>
#include <unknwn.h>
#include <ole2.h>
#include <limits.h>

/* FIXME: mingw.org doesn't define secure versions of
 * http://msdn.microsoft.com/en-us/library/f30dzcf6.aspxu */
#define NO_DSHOW_STRSAFE
#include <dshow.h>

#include <comcat.h>
#include "bdadefs.h"

#include <string>
enum {
    DVB_T  = 0x00000100
};

namespace tuner {
namespace demuxer {
namespace ts {
  class DemuxerImpl;
}
}

}

class BDAGraph : public ISampleGrabberCB
{
public:
    BDAGraph( tuner::demuxer::ts::DemuxerImpl *demux );
    virtual ~BDAGraph();

    bool initialize();
    bool checkDevice( const std::string &vendorId, const std::string &productId );
    bool setFrequency( int freq, int khBandwidth );

    float getSignalStrength();
    float getSignalNoiseRatio();

private:
    /* ISampleGrabberCB methods */
    ULONG ul_cbrc;
    STDMETHODIMP_( ULONG ) AddRef( ) { return ++ul_cbrc; }
    STDMETHODIMP_( ULONG ) Release( ) { return --ul_cbrc; }
    STDMETHODIMP QueryInterface( REFIID /*riid*/, void** /*p_p_object*/ )
        { return E_NOTIMPL; }
    STDMETHODIMP SampleCB( double d_time, IMediaSample* p_sample );
    STDMETHODIMP BufferCB( double d_time, BYTE* p_buffer, long l_buffer_len );

    CLSID     guid_network_type;   /* network type in use */
    long      l_tuner_used;        /* Index of the Tuning Device in use */

    /* registration number for the RunningObjectTable */
    DWORD     d_graph_register;

    tuner::demuxer::ts::DemuxerImpl *_demux;

    IMediaControl*  p_media_control;
    IGraphBuilder*  p_filter_graph;
    ITuningSpace*   p_tuning_space;
    ITuneRequest*   p_tune_request;

    ICreateDevEnum* p_system_dev_enum;
    IBaseFilter*    p_network_provider;
    IBaseFilter*    p_tuner_device;
    IBaseFilter*    p_capture_device;
    IBaseFilter*    p_sample_grabber;
    IBaseFilter*    p_mpeg_demux;
    IBaseFilter*    p_transport_info;
    IScanningTuner* p_scanning_tuner;
    ISampleGrabber* p_grabber;

    HRESULT SetUpTuner( REFCLSID guid_this_network_type );
    HRESULT Build( );
    HRESULT Check( REFCLSID guid_this_network_type );
    HRESULT GetFilterName( IBaseFilter* p_filter, char** psz_bstr_name );
    HRESULT GetPinName( IPin* p_pin, char** psz_bstr_name );
    bool lookUpDevice( REFCLSID this_clsid, const std::string &vendorId, const std::string &productId );
    HRESULT FindFilter( REFCLSID clsid, long* i_moniker_used,
        IBaseFilter* p_upstream, IBaseFilter** p_p_downstream);
    HRESULT Connect( IBaseFilter* p_filter_upstream,
        IBaseFilter* p_filter_downstream);
    HRESULT Start( );
    HRESULT Destroy( );
    HRESULT Register( );
    void Deregister( );
};
