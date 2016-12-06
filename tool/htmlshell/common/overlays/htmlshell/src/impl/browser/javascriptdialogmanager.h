#pragma once

#include "content/public/browser/javascript_dialog_manager.h"

namespace tvd {

class JavaScriptDialogManager : public content::JavaScriptDialogManager {
public:
	~JavaScriptDialogManager() override;

	void RunJavaScriptDialog(content::WebContents *web_contents, const GURL &origin_url, const std::string &accept_lang, content::JavaScriptMessageType type, const base::string16 &message_text, const base::string16 &text, const DialogClosedCallback &callback, bool *did_suppress_message) override;
	void RunBeforeUnloadDialog(content::WebContents *web_contents, const base::string16 &message, bool is_reload, const DialogClosedCallback &callback) override;
	bool HandleJavaScriptDialog(content::WebContents *web_contents, bool accept, const base::string16* prompt_override) override;
	void CancelActiveAndPendingDialogs(content::WebContents *web_contents) override;
	void ResetDialogState(content::WebContents *web_contents) override;
};

}
