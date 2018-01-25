#pragma once

#include <assert.h>
#include <type_traits>
#include "../nNetwork/nNetwork.h"

template<typename T>
class IIntegralSensable : public nNetwork::ISensable<T> {
	static_assert(std::is_integral<T>::value, "T is not integral");
public:
	virtual ~IIntegralSensable() {}	
};

template<typename T>
class IntegralSensable1d : public IIntegralSensable<T> {
public:
	IntegralSensable1d(const std::vector<T>& target) : m_target{ target } {}
	virtual ~IntegralSensable1d() {}

	virtual int GetDimensionCount() const override { return 1; }
	virtual int GetDimensionLength(int dimension) const override { assert(dimension == 1); return (int)m_target.size(); }

	virtual T Sense(const std::vector<int>& location) const override {
		assert(location.size() == 1);
		return (T)m_target[location[0]];		
	}
protected:
	std::vector<T> m_target;
};

template<typename T>
class IntegralSensable2d : public IIntegralSensable<T> {
public:
	IntegralSensable2d(const std::vector<std::vector<T>>& target) : m_target{ target } {
		m_height = m_target.size();
		m_width  = m_target[0].size();

		// Assert that all x vectors are of the same width.
		for (auto v : m_target)
			assert(v.size() == m_width);
	}
	virtual ~IntegralSensable2d() {}

	virtual int GetDimensionCount() const override { return 2; }
	virtual int GetDimensionLength(int dimension) const override { assert(dimension == 1 || dimension == 2); return dimension == 1 ? m_width : m_height; }

	virtual T Sense(const std::vector<int>& location) const override {
		assert(location.size() == 2);

		int x = location[0];
		int y = location[1];

		assert(x < m_width);
		assert(y < m_height);

		return (T)m_target[y][x];
	}

	T Sense(int x, int y) { return Sense(std::vector<int>{ x, y }); }

private:
	// The outer vector represents Y, while the inner vector represents X....
	// This way the Row data is layed out in contiguous memory.
	std::vector<std::vector<T>> m_target;
	int m_height;
	int m_width;
};

template<typename T>
class IntegralSensor : public nNetwork::ISensor {
public:
	IntegralSensor(IIntegralSensable<T>* pSensable, T max) : m_pSensable{ pSensable }, m_max{ max } {};
	virtual ~IntegralSensor() {}

	virtual vType Sense(const std::vector<int>& location) const override { return (vType)m_pSensable->Sense(location)/(vType)m_max; }

protected:
	IIntegralSensable<T>* m_pSensable;
	T m_max;
};