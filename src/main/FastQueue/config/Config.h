
#ifndef TIACTOR_CONFIG_CONFIG_H
#define TIACTOR_CONFIG_CONFIG_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "TiActor/basic/stddef.h"

#include <string>

namespace TiActor {

class Config {
private:
	std::string name_;

public:
	Config() {
		name_ = "default";
	}

	Config(const std::string & name) {
		name_ = name;
	}

	~Config() {
	}

public:
	std::string getName() const {
		return name_;
	}

	void setName(const std::string & name) {
		name_ = name;
	}
};

} // namespace TiActor

#endif  /* TIACTOR_CONFIG_CONFIG_H */
