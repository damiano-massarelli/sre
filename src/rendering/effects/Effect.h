#pragma once
#include <string>

class Effect
{
private:
	std::string mEffectPath;
	std::string mName;

public:
	Effect(const std::string& mName, const std::string& effectPath);

	const std::string& getEffectPath() const;

	const std::string& getName() const;

	virtual ~Effect() = default;
};
