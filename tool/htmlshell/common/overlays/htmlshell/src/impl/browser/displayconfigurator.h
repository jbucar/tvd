#pragma once

#include "configuredisplaytask.h"

#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "base/timer/timer.h"
#include "ui/display/types/native_display_observer.h"

namespace ui {
class NativeDisplayDelegate;
class DisplaySnapshot;
}

namespace tvd {

class BrowserMainParts;
class UpdateDisplayConfigurationTask;

class DisplayConfigurator : public ui::NativeDisplayObserver {
public:
	explicit DisplayConfigurator( BrowserMainParts *bMainParts );
	virtual ~DisplayConfigurator();

	void initialize();

	// ui::NativeDisplayObserver implementation:
	void OnConfigurationChanged() override;

protected:
	void OnConfigured( ConfigureDisplaysTask::Status success );
	void ConfigureDisplays();
	void RunPendingConfiguration();

private:
	BrowserMainParts *_browserMainParts;
	bool _forceConfigure;
	base::OneShotTimer _configureTimer;
	scoped_ptr<ui::NativeDisplayDelegate> _nativeDisplayDelegate;
	std::unique_ptr<UpdateDisplayConfigurationTask> _configurationTask;
	base::WeakPtrFactory<DisplayConfigurator> _weakPtrFactory;

	DISALLOW_COPY_AND_ASSIGN(DisplayConfigurator);
};

}
