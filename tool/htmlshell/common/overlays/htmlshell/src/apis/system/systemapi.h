#pragma once

#include "../shellapi.h"

namespace tvd {

namespace impl {
class SystemApi_1_0;
}

class MainDelegate;

class SystemApi : public ShellApi {
public:
	explicit SystemApi( MainDelegate *delegate );
	virtual ~SystemApi();

protected:
	virtual bool init() override;
	virtual void fin() override;

	virtual bool registerInterfaces() override;
	virtual uint32_t getLastVersion() const override;

private:
	impl::SystemApi_1_0 *_iface;
	MainDelegate *_delegate;
};

}
