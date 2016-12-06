#pragma once

#include <string>
#include <vector>
#include <stdint.h>

namespace tvd {

class ShellApi {
public:
	explicit ShellApi( const std::string &name );
	virtual ~ShellApi();

	bool initialize();
	void finalize();

	const std::string &name() const;

	bool hasInterface( uint32_t major, uint32_t minor ) const;
	void *getInterface( uint32_t major, uint32_t minor ) const;

protected:
	virtual bool init();
	virtual void fin();

	// Sublcases Must register their api interfaces here
	virtual bool registerInterfaces()=0;
	bool registerInterface( uint32_t version, void *iface );

	uint32_t calculateRequiredVersion( uint32_t major, uint32_t minor ) const;
	virtual uint32_t getLastVersion() const =0;

	typedef std::pair<uint32_t,void*> ApiInterface;

private:
	bool _registerEnabled;
	bool _initialized;
	std::string _name;
	std::vector<ApiInterface> _interfaces;
};

}
