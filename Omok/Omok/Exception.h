#pragma once
#include <string>

class Exception {
public:
	Exception(std::string msg) :msg_(msg) {}

	std::string What() const {
		return msg_;
	}
private:
	std::string msg_;
};