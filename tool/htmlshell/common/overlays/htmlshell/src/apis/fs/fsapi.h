#pragma once

#include "../shellapi.h"
#include "base/files/file_path.h"

namespace tvd {

namespace impl {
class FsApi_1_0;
}

class FsApi : public ShellApi {
public:
	FsApi();
	virtual ~FsApi();

protected:
	virtual bool init() override;
	virtual void fin() override;

	virtual bool registerInterfaces() override;
	virtual uint32_t getLastVersion() const override;

	base::FilePath getProfilePath( const std::string &type, const std::string &key );

private:
	impl::FsApi_1_0 *_iface;
};

}
