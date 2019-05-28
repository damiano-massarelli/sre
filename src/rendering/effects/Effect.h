#pragma once
#include <string>

class Effect
{
private:
	std::string mEffectPath;

public:
	Effect(const std::string& effectPath);

	const std::string& getEffectPath() const;

	virtual ~Effect() = default;
};
