

#ifndef TIACTOR_CONFIG_CONFIGURATIONFACTORY_H
#define TIACTOR_CONFIG_CONFIGURATIONFACTORY_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "TiActor/basic/stddef.h"
#include "TiActor/config/Config.h"

namespace TiActor {

class ConfigurationFactory {
private:
	//

public:
	ConfigurationFactory() {
	}

	~ConfigurationFactory() {
	}

private:
	static Config loadDefaultConfiguration() {
		return Config("default");
	}

public:
	static Config load() {
		return loadDefaultConfiguration();
	}
};

} // namespace TiActor

#endif  /* TIACTOR_CONFIG_CONFIGURATIONFACTORY_H */
