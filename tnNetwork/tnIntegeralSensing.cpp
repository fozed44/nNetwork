

#include "CppUnitTest.h"
#include "../nNetworkImplementation/IntegeralSensing.h"
#include <memory>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace tNetwork
{
	TEST_CLASS(tIntegeral)
	{
	public:
		TEST_METHOD(t_IntegralSensable1d_ctor) 
		{
			unique_ptr<IntegralSensable1d<int>> pIntegralSensable = make_unique<IntegralSensable1d<int>>(vector<int>{1, 1, 2, 3});

			Assert::IsNotNull(pIntegralSensable.get());

			Assert::AreEqual(1, pIntegralSensable->GetDimensionCount());

			Assert::AreEqual(4, pIntegralSensable->GetDimensionLength(1));
		}

		TEST_METHOD(t_IntegralSensable2d_ctor)
		{
			vector<vector<int>> v = vector<vector<int>> 
			{
				{ vector<int>{ 1, 2, 3, 4 } },
				{ vector<int>{ 2, 5, 7, 2 } },
				{ vector<int>{ 3, 6, 8, 3 } },
				{ vector<int>{ 4, 7, 9, 4 } },
				{ vector<int>{ 5, 8, 8, 3 } }
			};

			unique_ptr<IntegralSensable2d<int>> pIntegralSensable = make_unique<IntegralSensable2d<int>>(v);

			Assert::IsNotNull(pIntegralSensable.get());

			Assert::AreEqual(2, pIntegralSensable->GetDimensionCount());

			Assert::AreEqual(4, pIntegralSensable->GetDimensionLength(1));
			Assert::AreEqual(5, pIntegralSensable->GetDimensionLength(2));

			Assert::AreEqual(1, pIntegralSensable->Sense(0, 0));
			Assert::AreEqual(3, pIntegralSensable->Sense(3, 4));

		}

		TEST_METHOD(t_IntegralSensor1d_ctor)
		{
			unique_ptr<IntegralSensable1d<int>> pIntegralSensable = make_unique<IntegralSensable1d<int>>(vector<int>{1, 2, 4, 3});
			unique_ptr<IntegralSensor<int>>     pIntegralSensor   = make_unique<IntegralSensor<int>>(pIntegralSensable.get(), 10);

			Assert::IsNotNull(pIntegralSensable.get());
			Assert::IsNotNull(pIntegralSensor.get());

			Assert::AreEqual(1.0 / 10.0, pIntegralSensor->Sense(vector<int>{0}));
			Assert::AreEqual(3.0 / 10.0, pIntegralSensor->Sense(vector<int>{3}));
		}

		TEST_METHOD(t_IntegralSensor2d_ctor)
		{
			vector<vector<int>> v = vector<vector<int>>
			{
				{ vector<int>{ 1, 2, 3, 4 } },
				{ vector<int>{ 2, 5, 7, 2 } },
				{ vector<int>{ 3, 6, 8, 3 } },
				{ vector<int>{ 4, 7, 9, 4 } },
				{ vector<int>{ 5, 8, 8, 3 } }
			};

			unique_ptr<IntegralSensable2d<int>> pIntegralSensable = make_unique<IntegralSensable2d<int>>(v);
			unique_ptr<IntegralSensor<int>>     pIntegralSensor   = make_unique<IntegralSensor<int>>(pIntegralSensable.get(), 10);

			Assert::IsNotNull(pIntegralSensable.get());
			Assert::IsNotNull(pIntegralSensor.get());

			Assert::AreEqual(1.0/10.0, pIntegralSensor->Sense(vector<int>{0, 0}));
			Assert::AreEqual(3.0/10.0, pIntegralSensor->Sense(vector<int>{3, 4}));
		}
	};
}