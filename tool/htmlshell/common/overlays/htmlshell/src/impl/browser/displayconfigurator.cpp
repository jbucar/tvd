#include "displayconfigurator.h"
#include "browsermainparts.h"

#include "ui/display/types/display_snapshot.h"
#include "ui/display/types/native_display_delegate.h"
#include "ui/ozone/public/ozone_platform.h"

namespace tvd {

DisplayConfigurator::DisplayConfigurator( BrowserMainParts *bMainParts )
	: _browserMainParts(bMainParts), _weakPtrFactory(this)
{
	_forceConfigure = true;
}

DisplayConfigurator::~DisplayConfigurator()
{
	_browserMainParts = nullptr;
}

void DisplayConfigurator::initialize() {
	LOG(INFO) << __FUNCTION__ << "()";

	_nativeDisplayDelegate = ui::OzonePlatform::GetInstance()->CreateNativeDisplayDelegate();
	_nativeDisplayDelegate->AddObserver(this);
	_nativeDisplayDelegate->Initialize();
	_configurationTask.reset(new UpdateDisplayConfigurationTask(_nativeDisplayDelegate.get(), base::Bind(&DisplayConfigurator::OnConfigured, _weakPtrFactory.GetWeakPtr())));
	_configurationTask->Run();
}

void DisplayConfigurator::OnConfigurationChanged() {
	LOG(INFO) << __FUNCTION__ << "()";

	if (_configureTimer.IsRunning()) {
		_configureTimer.Reset();
	} else {
		_configureTimer.Start(FROM_HERE, base::TimeDelta::FromMilliseconds(500), this, &DisplayConfigurator::ConfigureDisplays);
	}
}

void DisplayConfigurator::ConfigureDisplays() {
	LOG(INFO) << __FUNCTION__ << "()";

	_forceConfigure = true;
	RunPendingConfiguration();
}

void DisplayConfigurator::OnConfigured( ConfigureDisplaysTask::Status success ) {
	LOG(INFO) << __FUNCTION__ << "() success=" << success;

	_configurationTask.reset();
	if (success != ConfigureDisplaysTask::ERROR) {
		_browserMainParts->createPlatform();
		if (!_configureTimer.IsRunning() && _forceConfigure) {
			_configureTimer.Start(FROM_HERE, base::TimeDelta::FromMilliseconds(500), this, &DisplayConfigurator::RunPendingConfiguration);
		}
	}
}

void DisplayConfigurator::RunPendingConfiguration() {
	LOG(INFO) << __FUNCTION__ << "()";

	// Configuration task is currently running. Do not start a second configuration.
	if (_configurationTask) {
		LOG(INFO) << __FUNCTION__ << "() configuration pending, skiping!";
		return;
	}

	if (!_forceConfigure) {
		LOG(ERROR) << "Called RunPendingConfiguration without any changes requested";
		return;
	}

	_configurationTask.reset(new UpdateDisplayConfigurationTask(_nativeDisplayDelegate.get(), base::Bind(&DisplayConfigurator::OnConfigured, _weakPtrFactory.GetWeakPtr())));
	_forceConfigure = false;
	_configurationTask->Run();
}

}
