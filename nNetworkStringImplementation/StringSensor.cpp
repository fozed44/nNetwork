#include "nNetworkStringImplementation.h"

#include <vector>

StringSensor::StringSensor(StringSensable *sensable) {
	_pSensable = sensable;
}


StringSensor::~StringSensor() {
}

vType StringSensor::Sense(const std::vector<int>& location) const {

	// ISensor::Sense must take an output from ISensable.Sense and
	// relate it to a predicate such that the closer the ISensable.Sense
	// result is to true, the closer the result of ISensore::Sense is to one.
	throw "Incorrect implementation";
	//
	//

	unsigned char sensedChar = _pSensable->Sense(location);

	return (vType)sensedChar / 255.0f;
}
