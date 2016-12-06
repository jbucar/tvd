#include "downloadmanagerdelegate.h"
#if defined(OS_WIN)
#include <windows.h>
#include <commdlg.h>
#endif
#include "base/bind.h"
#include "base/command_line.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/download_manager.h"
#include "content/public/browser/web_contents.h"
#include "net/base/filename_util.h"
#if defined(OS_WIN)
#include "ui/aura/window.h"
#include "ui/aura/window_tree_host.h"
#endif

namespace tvd {

DownloadManagerDelegate::DownloadManagerDelegate( content::DownloadManager *download_manager )
    : _downloadMgr(download_manager),
      weak_ptr_factory_(this)
{
}

DownloadManagerDelegate::~DownloadManagerDelegate()
{
  if (_downloadMgr) {
    DCHECK_EQ(static_cast<DownloadManagerDelegate*>(this),
              _downloadMgr->GetDelegate());
    _downloadMgr->SetDelegate(NULL);
    _downloadMgr = NULL;
  }
}

void DownloadManagerDelegate::Shutdown() {
	// Revoke any pending callbacks. _downloadMgr et. al. are no longer safe to access after this point.
	weak_ptr_factory_.InvalidateWeakPtrs();
	_downloadMgr = NULL;
}

bool DownloadManagerDelegate::DetermineDownloadTarget( content::DownloadItem *download, const content::DownloadTargetCallback &callback ) {
	DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));
	// This assignment needs to be here because even at the call to SetDownloadManager, the system is not fully initialized.
	if (default_download_path_.empty()) {
		default_download_path_ = _downloadMgr->GetBrowserContext()->GetPath().Append(FILE_PATH_LITERAL("Downloads"));
	}

	if (!download->GetForcedFilePath().empty()) {
		callback.Run(download->GetForcedFilePath(),
                     content::DownloadItem::TARGET_DISPOSITION_OVERWRITE,
                     content::DOWNLOAD_DANGER_TYPE_NOT_DANGEROUS,
                     download->GetForcedFilePath());
		return true;
	}

	FilenameDeterminedCallback filename_determined_callback =
	  base::Bind(&DownloadManagerDelegate::OnDownloadPathGenerated,
	             weak_ptr_factory_.GetWeakPtr(),
	             download->GetId(),
	             callback);

	content::BrowserThread::PostTask( content::BrowserThread::FILE,
                 FROM_HERE,
                 base::Bind(&DownloadManagerDelegate::GenerateFilename,
                 download->GetURL(),
                 download->GetContentDisposition(),
                 download->GetSuggestedFilename(),
                 download->GetMimeType(),
                 default_download_path_,
                 filename_determined_callback));
	return true;
}

bool DownloadManagerDelegate::ShouldOpenDownload( content::DownloadItem *item, const content::DownloadOpenDelayedCallback &callback ) {
	return true;
}

void DownloadManagerDelegate::GetNextId( const content::DownloadIdCallback &callback ) {
	static uint32_t next_id = content::DownloadItem::kInvalidId + 1;
	callback.Run(next_id++);
}

void DownloadManagerDelegate::GenerateFilename( const GURL &url,
                                                     const std::string &content_disposition,
                                                     const std::string &suggested_filename,
                                                     const std::string &mime_type,
                                                     const base::FilePath &suggested_directory,
                                                     const FilenameDeterminedCallback &callback ) {
	DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::FILE));
	base::FilePath generated_name = net::GenerateFileName(url, content_disposition, std::string(), suggested_filename, mime_type, "download");

	if (!base::PathExists(suggested_directory)) {
		base::CreateDirectory(suggested_directory);
	}

	base::FilePath suggested_path(suggested_directory.Append(generated_name));
	content::BrowserThread::PostTask(content::BrowserThread::UI, FROM_HERE, base::Bind(callback, suggested_path));
}

void DownloadManagerDelegate::OnDownloadPathGenerated( uint32_t download_id,
                                                            const content::DownloadTargetCallback &callback,
                                                            const base::FilePath &suggested_path ) {
	DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));
	ChooseDownloadPath(download_id, callback, suggested_path);
}

void DownloadManagerDelegate::ChooseDownloadPath( uint32_t download_id,
                                                       const content::DownloadTargetCallback &callback,
                                                       const base::FilePath &suggested_path ) {
	DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));
	content::DownloadItem *item = _downloadMgr->GetDownload(download_id);
	if (!item || (item->GetState() != content::DownloadItem::IN_PROGRESS)) {
		return;
	}

	base::FilePath result;
#if defined(OS_WIN)
	std::wstring file_part = base::FilePath(suggested_path).BaseName().value();
	wchar_t file_name[MAX_PATH];
	base::wcslcpy(file_name, file_part.c_str(), arraysize(file_name));
	OPENFILENAME save_as;
	ZeroMemory(&save_as, sizeof(save_as));
	save_as.lStructSize = sizeof(OPENFILENAME);
	save_as.hwndOwner = item->GetWebContents()->GetNativeView()->
	  GetHost()->GetAcceleratedWidget();
	save_as.lpstrFile = file_name;
	save_as.nMaxFile = arraysize(file_name);

	std::wstring directory;
	if (!suggested_path.empty()) {
		directory = suggested_path.DirName().value();
	}

	save_as.lpstrInitialDir = directory.c_str();
	save_as.Flags = OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;

	if (GetSaveFileName(&save_as)) {
		result = base::FilePath(std::wstring(save_as.lpstrFile));
	}
#else
	NOTIMPLEMENTED();
#endif

	callback.Run(result, content::DownloadItem::TARGET_DISPOSITION_PROMPT, content::DOWNLOAD_DANGER_TYPE_NOT_DANGEROUS, result);
}

}
