#pragma once
#include <iostream>

class Serializable
{
public:
	virtual std::ostream& serialize(std::ostream& os) const = 0;
	virtual std::istream& deserialize(std::istream& is) = 0;
};