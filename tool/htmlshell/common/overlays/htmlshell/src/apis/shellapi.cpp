#include "shellapi.h"
#include "dtv-htmlshellsdk/src/libraryinterface.h"
#include "base/logging.h"

namespace tvd {

ShellApi::ShellApi( const std::string &name )
{
	_name = name;
	_registerEnabled = false;
	_initialized = false;
}

ShellApi::~ShellApi()
{
	CHECK(_interfaces.size() == 0);
}

bool ShellApi::initialize() {
	if (_initialized) {
		LOG(WARNING) << "Fail to initialize api " << _name << ". Already initialized!";
		return false;
	}

	if (!init()) {
		LOG(WARNING) << "Fail to init api " << _name << " implementation!";
		return false;
	}

	// Delegate subclases to register interfaces
	_registerEnabled = true;
	if (!registerInterfaces()) {
		LOG(ERROR) << "Fail to register " << _name << " interfaces!";
		finalize();
	}
	_registerEnabled = false;

	_initialized = _interfaces.size() > 0;
	LOG_IF(ERROR, !_initialized) << "Fail to initialize api " << _name << ", any interface registered!";
	return _initialized;
}

void ShellApi::finalize() {
	_interfaces.clear();
	fin();
}

bool ShellApi::init() {
	return true;
}

void ShellApi::fin() {
}

const std::string &ShellApi::name() const {
	return _name;
}

bool ShellApi::hasInterface( uint32_t major, uint32_t minor ) const {
	return getInterface(major, minor) != NULL;
}

void *ShellApi::getInterface( uint32_t major, uint32_t minor ) const {
	uint32_t version = calculateRequiredVersion(major, minor);
	std::vector<ApiInterface>::const_iterator it = _interfaces.begin();
	while (it != _interfaces.end()) {
		if (version == it->first) {
			return it->second;
		}
		it++;
	}
	return NULL;
}

uint32_t ShellApi::calculateRequiredVersion( uint32_t major, uint32_t minor ) const {
	uint32_t required = MAKE_API_VERSION(major, minor);
	return (required > 0) ? required : getLastVersion();
}

bool ShellApi::registerInterface( uint32_t version, void *iface ) {
	if (!_registerEnabled) {
		LOG(WARNING) << "Cannot register interface outside of registerInterfaces() call.";
		return false;
	}
	uint32_t major = GET_MAJOR_VERSION(version);
	uint32_t minor = GET_MINOR_VERSION(version);
	if (hasInterface(major, minor)) {
		LOG(WARNING) << "Already exist interface: " << _name << "-" << major << "." << minor;
		return false;
	}
	_interfaces.push_back(std::make_pair(version, iface));
	return true;
}

}
