#include "nNetworkStringImplementation.h"

#include <exception>


using namespace std;

StringSensable::StringSensable(const string &target) {
	_target = target;
	_length = _target.length();
}

StringSensable::~StringSensable() { }

int StringSensable::GetDimensionCount() const {
	return 1;
}

int StringSensable::GetDimensionLength(int dimension) const {
	if (dimension != 1)
		throw new std::exception("dimension is out of range.");

	return _length;
}

unsigned char StringSensable::Sense(const std::vector<int>& location) const {

#ifdef _DEBUG_
	if (location.size != 1)
		throw new exception("location does not contain exactly element.");

	if (location[0] < 0 || location[0] > _length)
		throw new exception("location is out of range.");
#endif

	return _target.substr(location[0], 1)[0];
}
