#include <vector>
#include "CppUnitTest.h"
#include <memory>

#include "../nNetworkStringImplementation/nNetworkStringImplementation.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace tnNetwork {

	TEST_CLASS(tStringSensor) 
	{
	public:
		TEST_METHOD(tStringSensor_ctor)
			// Create a new sensor, make sure the constructor constructs a new object
			// correctly.
		{
			string testString{ "Test String" };
			unique_ptr<StringSensable> pTestSensable = make_unique<StringSensable>(testString);
			unique_ptr<StringSensor>   pTestSensor = make_unique<StringSensor>(pTestSensable.get());

			Assert::IsNotNull(pTestSensor.get());
		}

		TEST_METHOD(tStringSensor_Sense)
			// Check StringSensor.Sense... make sure that it returns some value that
			// is not zero.
		{
			string testString{ "Test String" };
			unique_ptr<StringSensable> pTestSensable = make_unique<StringSensable>(testString);
			unique_ptr<StringSensor> pTestSensor = make_unique<StringSensor>(pTestSensable.get());

			auto result = pTestSensor->Sense(vector<int>{0});

			Assert::AreNotEqual(0.0, (double)result);
		}

	};
}