#pragma once

#include "base/callback_forward.h"
#include "base/compiler_specific.h"
#include "base/memory/weak_ptr.h"
#include "content/public/browser/download_manager_delegate.h"

namespace content {
class DownloadManager;
}

namespace tvd {

class DownloadManagerDelegate : public content::DownloadManagerDelegate {
public:
	explicit DownloadManagerDelegate( content::DownloadManager *manager );
	~DownloadManagerDelegate() override;

	void Shutdown() override;
	bool DetermineDownloadTarget( content::DownloadItem *download, const content::DownloadTargetCallback &callback ) override;
	bool ShouldOpenDownload( content::DownloadItem *item, const content::DownloadOpenDelayedCallback& callback ) override;
	void GetNextId( const content::DownloadIdCallback &callback ) override;

private:
	friend class base::RefCountedThreadSafe<content::DownloadManagerDelegate>;

	typedef base::Callback<void(const base::FilePath&)> FilenameDeterminedCallback;

	static void GenerateFilename( const GURL& url,
                           const std::string &content_disposition,
                           const std::string& suggested_filename,
                           const std::string& mime_type,
                           const base::FilePath& suggested_directory,
                           const FilenameDeterminedCallback& callback );
	void OnDownloadPathGenerated( uint32_t download_id, const content::DownloadTargetCallback &callback, const base::FilePath &suggested_path );
	void ChooseDownloadPath( uint32_t download_id, const content::DownloadTargetCallback &callback, const base::FilePath &suggested_path );

	content::DownloadManager *_downloadMgr;
	base::FilePath default_download_path_;
	base::WeakPtrFactory<DownloadManagerDelegate> weak_ptr_factory_;

	DISALLOW_COPY_AND_ASSIGN(DownloadManagerDelegate);
};

}
