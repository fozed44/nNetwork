#include "nNetworkStringImplementation.h"

#include <vector>

StringSensor::StringSensor(StringSensable *sensable) {
	_pSensable = sensable;
}


StringSensor::~StringSensor() {
}

vType StringSensor::Sense(const std::vector<int>& location) const {
	unsigned char sensedChar = _pSensable->Sense(location);

	return (vType)sensedChar / 255.0f;
}
