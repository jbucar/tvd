#pragma once

#include "base/logging.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/renderer/render_thread.h"
#include "ui/gfx/geometry/size.h"
#include <string>

#define TVD_UIT content::BrowserThread::UI
#define TVD_IOT content::BrowserThread::IO
#define TVD_FILET content::BrowserThread::FILE

#define TVD_CURRENTLY_ON_RT() (!!content::RenderThread::Get())
#define TVD_CURRENTLY_ON(id) content::BrowserThread::CurrentlyOn(id)
#define TVD_CURRENTLY_ON_UIT() TVD_CURRENTLY_ON(TVD_UIT)
#define TVD_CURRENTLY_ON_IOT() TVD_CURRENTLY_ON(TVD_IOT)
#define TVD_CURRENTLY_ON_FILET() TVD_CURRENTLY_ON(TVD_FILET)

#define TVD_REQUIRE(id) CHECK(TVD_CURRENTLY_ON(id))
#define TVD_REQUIRE_UIT() TVD_REQUIRE(TVD_UIT)
#define TVD_REQUIRE_IOT() TVD_REQUIRE(TVD_IOT)
#define TVD_REQUIRE_FILET() TVD_REQUIRE(TVD_FILET)
#define TVD_REQUIRE_RT() CHECK(TVD_CURRENTLY_ON_RT())
#define TVD_REQUIRE_RT_RETURN(var) \
  if (!TVD_CURRENTLY_ON_RT()) { \
    NOTREACHED() << "called on invalid thread"; \
    return var; \
  }

#define UNUSED(x) (void)(x)

namespace tvd {
namespace util {

gfx::Size getWindowSize();
void postTask( int tid, const base::Closure &task );

namespace log {
void init();
}

}
}
