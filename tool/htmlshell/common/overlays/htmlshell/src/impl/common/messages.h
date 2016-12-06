#include "ipc/ipc_message_macros.h"
#include "base/values.h"
#include <string>

#define IPC_MESSAGE_START ExtensionMsgStart

IPC_SYNC_MESSAGE_CONTROL2_2( Tvd_GetNewSiteInfo,
                             int, /* render view routing id */
                             int, /* render frame routing id */
                             int, /* [out] site id */
                             std::string /* [out] init JavaScript code */ )

IPC_MESSAGE_ROUTED3( Tvd_ExtQuery,
                     int, /*query id*/
                     std::string, /* msg */
                     base::ListValue /* params */ )

IPC_MESSAGE_ROUTED5( Tvd_ExtQueryResponse,
                     int, /*site id*/
                     int, /*query id*/
                     std::string, /*error message*/
                     base::ListValue, /* response */
                     bool /*is a signal event*/ )
