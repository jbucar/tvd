#include "javascriptdialogmanager.h"
#include "base/strings/utf_string_conversions.h"

namespace tvd {

JavaScriptDialogManager::~JavaScriptDialogManager()
{
}

void JavaScriptDialogManager::RunJavaScriptDialog( content::WebContents *web_contents, const GURL &origin_url, const std::string &accept_lang, content::JavaScriptMessageType type, const base::string16 &message_text, const base::string16 &text, const DialogClosedCallback &callback, bool *did_suppress_message ) {
}

void JavaScriptDialogManager::RunBeforeUnloadDialog( content::WebContents *web_contents, const base::string16 &message, bool is_reload, const DialogClosedCallback &callback) {
	callback.Run(message != base::UTF8ToUTF16("HTMLSHELL_ASYNC_EXIT"), base::string16());
}

bool JavaScriptDialogManager::HandleJavaScriptDialog(content::WebContents *web_contents, bool accept, const base::string16* prompt_override) {
	return false;
}

void JavaScriptDialogManager::CancelActiveAndPendingDialogs(content::WebContents *web_contents) {
}

void JavaScriptDialogManager::ResetDialogState(content::WebContents *web_contents) {
}

}
