#include "platformdataaura.h"
#include "screen.h"
#include "../ui/aura/testfocusclient.h"
#include "ui/aura/client/aura_constants.h"
#include "ui/aura/client/default_capture_client.h"
#include "ui/aura/env.h"
#include "ui/aura/layout_manager.h"
#include "ui/aura/window.h"
#include "ui/aura/window_tree_host.h"
#include "ui/base/ime/input_method.h"
#include "ui/gfx/screen.h"
#include "ui/wm/core/default_activation_client.h"
#include <list>

namespace tvd {

class FillLayout : public aura::LayoutManager {
public:
	FillLayout() : _cursorWin(nullptr) {}
	~FillLayout() override {}

	void OnWindowResized() override {}
	void OnChildWindowVisibilityChanged( aura::Window *child, bool visible ) override {}
	void OnWillRemoveWindowFromLayout( aura::Window *child ) override {}

	void OnWindowRemovedFromLayout( aura::Window *child ) override {
		if (child == _cursorWin) {
			_cursorWin = nullptr;
		}
		else if (child->name() == "HTMLSHELL_BROWSER") {
			_windows.remove(child);
		}
	}

	void OnWindowAddedToLayout( aura::Window *child ) override {
		if (child->name() == "HTMLSHELL_CURSOR") {
			_cursorWin = child;
			_cursorWin->parent()->StackChildAtTop(_cursorWin);
		}
		else if (child->name() == "HTMLSHELL_BROWSER") {
			auto it = _windows.begin();
			int id = child->id();
			aura::Window *prev = nullptr;
			while (it != _windows.end() && id < (*it)->id()) {
				prev = *it;
				++it;
			}
			_windows.insert(it, child);
			if (prev) {
				child->parent()->StackChildBelow(child, prev);
			} else if (it != _windows.end()) {
				child->parent()->StackChildAbove(child, *it);
			} else if (_cursorWin) {
				child->parent()->StackChildBelow(child, _cursorWin);
			} else {
				child->parent()->StackChildAtTop(child);
			}
		}
		else if (_cursorWin) {
			// Ensure _cursorWin is always on top of every other window
			_cursorWin->parent()->StackChildAtTop(_cursorWin);
		}
	}

	void SetChildBounds( aura::Window *child, const gfx::Rect& requested_bounds ) override {
		SetChildBoundsDirect(child, requested_bounds);
	}

private:
	aura::Window *_cursorWin;
	std::list<aura::Window*> _windows;

	DISALLOW_COPY_AND_ASSIGN(FillLayout);
};

PlatformDataAura::PlatformDataAura( const gfx::Rect &bounds )
{
	CHECK(aura::Env::GetInstance());
	_host.reset(aura::WindowTreeHost::Create(bounds));
	gfx::Screen::SetScreenInstance(gfx::SCREEN_TYPE_NATIVE, new Screen(_host.get(), bounds));
	_host->GetInputMethod()->OnFocus();
	_host->InitHost();
	aura::Window *root = _host->window();
	root->SetName("HTMLSHELL_ROOT");
	root->SetLayoutManager(new FillLayout());
	root->SetTransparent(true);
	root->layer()->GetCompositor()->SetHostHasTransparentBackground(true);

	_focusClient.reset(new aura::test::TestFocusClient());
	aura::client::SetFocusClient(root, _focusClient.get());

	new wm::DefaultActivationClient(root);
	_captureClient.reset(new aura::client::DefaultCaptureClient(root));
	aura::client::SetWindowTreeClient(root, this);

	_host->Show();
}

PlatformDataAura::~PlatformDataAura()
{
	aura::client::SetWindowTreeClient(_host->window(), nullptr);
}

aura::Window *PlatformDataAura::GetDefaultParent( aura::Window *context, aura::Window *window, const gfx::Rect &bounds ) {
	return _host->window();
}

}
