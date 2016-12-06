#pragma once

#include "base/auto_reset.h"
#include "base/callback.h"
#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "ui/ozone/public/ozone_platform.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/size.h"

#include <queue>

namespace ui {
class NativeDisplayDelegate;
class DisplaySnapshot;
class DisplayMode;
}

namespace tvd {

struct DisplayState {
	ui::DisplaySnapshot *display = nullptr;
	const ui::DisplayMode *selected_mode = nullptr;
};

struct DisplayConfigureRequest {
	DisplayConfigureRequest( ui::DisplaySnapshot *display, const ui::DisplayMode *mode, const gfx::Point &origin )
		: display(display), mode(mode), origin(origin) {}

	ui::DisplaySnapshot *display;
	const ui::DisplayMode *mode;
	gfx::Point origin;
};

class ConfigureDisplaysTask {
public:
	enum Status { ERROR, SUCCESS, PARTIAL_SUCCESS };
	typedef base::Callback<void(Status)> ResponseCallback;

	ConfigureDisplaysTask( ui::NativeDisplayDelegate *delegate, const std::vector<DisplayConfigureRequest> &requests, const ResponseCallback &callback );
	~ConfigureDisplaysTask();

	void Run();

private:
	void OnConfigured(size_t index, bool success);

	ui::NativeDisplayDelegate* delegate_;  // Not owned.
	std::vector<DisplayConfigureRequest> requests_;
	ResponseCallback callback_;
	std::queue<size_t> pending_request_indexes_;
	bool is_configuring_;
	size_t num_displays_configured_;
	Status task_status_;

	base::WeakPtrFactory<ConfigureDisplaysTask> weak_ptr_factory_;

	DISALLOW_COPY_AND_ASSIGN(ConfigureDisplaysTask);
};

class UpdateDisplayConfigurationTask {
public:
	static const ui::DisplayMode *FindDisplayModeMatchingSize( const ui::DisplaySnapshot &display,	const gfx::Size &size );

	UpdateDisplayConfigurationTask( ui::NativeDisplayDelegate *delegate, const ConfigureDisplaysTask::ResponseCallback &callback );
	~UpdateDisplayConfigurationTask();

	void Run();
	void OnDisplaysUpdated( const std::vector<ui::DisplaySnapshot*> &displays );

protected:
	bool GetDisplayLayout( const std::vector<ui::DisplaySnapshot*>& displays, std::vector<DisplayConfigureRequest>* requests );
	std::vector<DisplayState> ParseDisplays( const std::vector<ui::DisplaySnapshot*> &snapshots ) const;
	const ui::DisplayMode *GetUserSelectedMode( const ui::DisplaySnapshot &display ) const;
	bool GetResolutionForDisplayId( const ui::DisplaySnapshot &display, gfx::Size *size ) const;


private:
	ui::NativeDisplayDelegate *_delegate;
	ConfigureDisplaysTask::ResponseCallback callback_;
	gfx::Size framebuffer_size_;
	std::unique_ptr<ConfigureDisplaysTask> _configureTask;
	base::WeakPtrFactory<UpdateDisplayConfigurationTask> weak_ptr_factory_;

	DISALLOW_COPY_AND_ASSIGN(UpdateDisplayConfigurationTask);
};

}
