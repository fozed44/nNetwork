#pragma once

#include "../nNetwork/nNetwork.h"

class StringSensable : public nNetwork::ISensable<unsigned char> {
public:
	StringSensable(const std::string& target);
	virtual ~StringSensable();

	virtual int GetDimensionCount() const override;
	virtual int GetDimensionLength(int dimension) const override;

	virtual unsigned char Sense(const std::vector<int>& location) const override;	

protected:
	std::string _target;
	uint32_t    _length;
};

class StringSensor : public nNetwork::ISensor {
public:
	StringSensor(StringSensable* pSensable);
	virtual ~StringSensor();

	virtual vType Sense(const std::vector<int>& location) const override;

	vType Sense(int location) const { return Sense(std::vector<int>{location}); }

protected:
	StringSensable *_pSensable;
};
