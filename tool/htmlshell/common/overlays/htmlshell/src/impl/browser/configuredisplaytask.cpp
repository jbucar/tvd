#include "configuredisplaytask.h"
#include "../../switches.h"
#include "../../util.h"

#include "base/bind.h"
#include "base/command_line.h"
#include "ui/display/types/display_snapshot.h"
#include "ui/display/types/native_display_delegate.h"

namespace tvd {

const ui::DisplayMode *FindNextMode(const ui::DisplaySnapshot &display_state, const ui::DisplayMode *display_mode) {
	if (!display_mode)
		return nullptr;

	int best_mode_pixels = 0;
	const ui::DisplayMode *best_mode = nullptr;
	int current_mode_pixels = display_mode->size().GetArea();
	for (auto mode : display_state.modes()) {
		int pixel_count = mode->size().GetArea();
		if (pixel_count < current_mode_pixels && pixel_count > best_mode_pixels) {
			best_mode = mode;
			best_mode_pixels = pixel_count;
		}
	}
	return best_mode;
}

ConfigureDisplaysTask::ConfigureDisplaysTask( ui::NativeDisplayDelegate *delegate, const std::vector<DisplayConfigureRequest> &requests, const ResponseCallback &callback )
	: delegate_(delegate),
	  requests_(requests),
	  callback_(callback),
	  is_configuring_(false),
	  num_displays_configured_(0),
	  task_status_(SUCCESS),
	  weak_ptr_factory_(this)
{
	for (size_t i = 0; i < requests_.size(); ++i)
		pending_request_indexes_.push(i);

}

ConfigureDisplaysTask::~ConfigureDisplaysTask()
{}

void ConfigureDisplaysTask::Run() {
	if (is_configuring_) {
		return;
	}

	{
		base::AutoReset<bool> recursivity_guard(&is_configuring_, true);
		while (!pending_request_indexes_.empty()) {
			size_t index = pending_request_indexes_.front();
			DisplayConfigureRequest* request = &requests_[index];
			pending_request_indexes_.pop();
			delegate_->Configure(*request->display, request->mode, request->origin, base::Bind(&ConfigureDisplaysTask::OnConfigured, weak_ptr_factory_.GetWeakPtr(), index));
		}
	}

	// Nothing should be modified after the |callback_| is called since the task may be deleted in the callback.
	if (num_displays_configured_ == requests_.size()) {
		callback_.Run(task_status_);
	}
}

void ConfigureDisplaysTask::OnConfigured(size_t index, bool success) {
	DisplayConfigureRequest* request = &requests_[index];
	VLOG(2) << "Configured status=" << success << " display=" << request->display->display_id() << " origin=" << request->origin.ToString() << " mode=" << (request->mode ? request->mode->ToString() : "null");
	if (!success) {
		request->mode = FindNextMode(*request->display, request->mode);
		if (request->mode) {
			pending_request_indexes_.push(index);
			if (task_status_ == SUCCESS)
				task_status_ = PARTIAL_SUCCESS;

			Run();
			return;
		}
	} else {
		request->display->set_current_mode(request->mode);
		request->display->set_origin(request->origin);
	}

	num_displays_configured_++;
	if (!success)
		task_status_ = ERROR;

	Run();
}

// UpdateDisplayConfigurationTask implementation:
UpdateDisplayConfigurationTask::UpdateDisplayConfigurationTask( ui::NativeDisplayDelegate *delegate, const ConfigureDisplaysTask::ResponseCallback &callback )
	: _delegate(delegate), callback_(callback), weak_ptr_factory_(this)
{
	_delegate->GrabServer();
}

UpdateDisplayConfigurationTask::~UpdateDisplayConfigurationTask()
{
	_delegate->UngrabServer();
}

void UpdateDisplayConfigurationTask::Run() {
	_delegate->GetDisplays(base::Bind(&UpdateDisplayConfigurationTask::OnDisplaysUpdated, weak_ptr_factory_.GetWeakPtr()));
}

void UpdateDisplayConfigurationTask::OnDisplaysUpdated( const std::vector<ui::DisplaySnapshot*> &displays ) {
	LOG(INFO) << __FUNCTION__ << "() displays=" << displays.size();

	if (displays.size() > 0) {
		for (auto display : displays) {
			LOG(INFO) << __FUNCTION__ << "()\t Display: " << display->display_name();
			LOG(INFO) << __FUNCTION__ << "()\t\t sys_path=" << display->sys_path().value();
			LOG(INFO) << __FUNCTION__ << "()\t\t origin=" << display->origin().ToString();
			LOG(INFO) << __FUNCTION__ << "()\t\t physical_size=" << display->physical_size().ToString();
			LOG(INFO) << __FUNCTION__ << "()\t\t current_mode=" << display->current_mode()->ToString();
			LOG(INFO) << __FUNCTION__ << "()\t\t native_mode=" << display->native_mode()->ToString();
			LOG(INFO) << __FUNCTION__ << "()\t\t modes=" << display->modes().size();
			for (auto mode : display->modes()) {
				LOG(INFO) << __FUNCTION__ << "()\t\t\t " << mode->ToString();
			}
		}
	}

	std::vector<DisplayConfigureRequest> requests;
	if (!GetDisplayLayout(displays, &requests)) {
		callback_.Run(ConfigureDisplaysTask::ERROR);
		return;
	}
	if (!requests.empty()) {
		CHECK(!framebuffer_size_.IsEmpty());
		_delegate->CreateFrameBuffer(framebuffer_size_);
		_configureTask.reset(new ConfigureDisplaysTask(_delegate, requests, callback_));
		_configureTask->Run();
	} else {
		LOG(WARNING) << "No displays";
		callback_.Run(ConfigureDisplaysTask::SUCCESS);
	}
}

bool UpdateDisplayConfigurationTask::GetDisplayLayout( const std::vector<ui::DisplaySnapshot*>& displays, std::vector<DisplayConfigureRequest>* requests ) {
	LOG(INFO) << "UpdateDisplayConfigurationTask::" << __FUNCTION__ << "()";
	std::vector<DisplayState> states = ParseDisplays(displays);
	for (size_t i = 0; i < displays.size(); ++i) {
		requests->push_back(DisplayConfigureRequest(displays[i], displays[i]->current_mode(), gfx::Point()));
	}

	for (size_t i = 0; i < states.size(); ++i) {
		const DisplayState *state = &states[i];
		(*requests)[i].mode = state->selected_mode;

		if (states.size() == 1) {
			const ui::DisplayMode* mode_info = state->selected_mode;
			LOG(INFO) << "UpdateDisplayConfigurationTask::" << __FUNCTION__ << "() > selected_mode=" << mode_info->ToString();
			if (!mode_info) {
				LOG(WARNING) << "No selected mode when configuring display: " << state->display->ToString();
				return false;
			}
			framebuffer_size_ = mode_info->size();
		}
	}

	return true;
}

std::vector<DisplayState> UpdateDisplayConfigurationTask::ParseDisplays( const std::vector<ui::DisplaySnapshot*> &snapshots ) const {
	std::vector<DisplayState> cached_displays;
	for (auto snapshot : snapshots) {
		DisplayState display_state;
		display_state.display = snapshot;
		display_state.selected_mode = GetUserSelectedMode(*snapshot);
		cached_displays.push_back(display_state);
	}
	return cached_displays;
}

const ui::DisplayMode *UpdateDisplayConfigurationTask::GetUserSelectedMode( const ui::DisplaySnapshot &display ) const {
	gfx::Size size;
	const ui::DisplayMode *selected_mode = nullptr;
	if (GetResolutionForDisplayId(display, &size)) {
		selected_mode = FindDisplayModeMatchingSize(display, size);
	}
	return selected_mode ? selected_mode : display.native_mode();
}

bool UpdateDisplayConfigurationTask::GetResolutionForDisplayId( const ui::DisplaySnapshot &display, gfx::Size *size ) const {
	*size = display.native_mode()->size();

	if (base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kWindowSize)) {
		gfx::Size requested = util::getWindowSize();

		int bestMatch = std::abs(size->GetArea() - requested.GetArea());
		for (auto mode : display.modes()) {
			int current = std::abs(mode->size().GetArea() - requested.GetArea());
			if (current < bestMatch) {
				bestMatch = current;
				*size = mode->size();
			}
		}
	}

	base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(switches::kWindowSize, size->ToString());
	return true;
}

// static
const ui::DisplayMode *UpdateDisplayConfigurationTask::FindDisplayModeMatchingSize( const ui::DisplaySnapshot &display,	const gfx::Size &size ) {
	const ui::DisplayMode *best_mode = NULL;
	for (auto mode : display.modes()) {
		if (mode->size() != size)
			continue;
		if (mode == display.native_mode()) {
			best_mode = mode;
			break;
		}
		if (!best_mode) {
			best_mode = mode;
			continue;
		}
		if (mode->is_interlaced()) {
			if (!best_mode->is_interlaced())
				continue;
		} else {
			// Reset the best rate if the non interlaced is found the first time.
			if (best_mode->is_interlaced()) {
				best_mode = mode;
				continue;
			}
		}
		if (mode->refresh_rate() < best_mode->refresh_rate())
			continue;

		best_mode = mode;
	}

	return best_mode;
}

}
