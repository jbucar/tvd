#pragma once

namespace switches {

// Path to system profile directory
const char kSysProfile[] = "sys-profile";

// Path to user profile directory
const char kUsrProfile[] = "usr-profile";

// Path to main manifest
const char kManifest[] = "manifest";

// Extra user-agent string
const char kExtraUA[] = "extra-ua";

// Path to plugin manifest
const char kPlugins[] = "plugins";

// Comma separated list of allowed plugins
const char kAllowedPlugins[] = "allowed-plugins";

// Custom schema for handling srtdv:// urls
const char kUdpScheme[] = "udp";

// Size of the root window
const char kWindowSize[] = "window-size";

// JavaScript code to inject on startup browser
const char kInitJs[] = "init-js";

}
