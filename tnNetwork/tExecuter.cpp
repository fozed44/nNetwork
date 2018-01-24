//#include "stdafx.h"
#include <vector>
#include "CppUnitTest.h"
#include <memory>

#include "../nNetworkImplementation/nNetworkStringImplementation.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace tnNetwork
{
	TEST_CLASS(tExecuter)
	{
	public:

		TEST_METHOD(tExecuter_ctor)
		{
			auto pStringSensable = make_unique<StringSensable>("Test String");
			auto pStringSensor   = make_unique<StringSensor>(pStringSensable.get());
			auto pExecuter       = make_unique<nNetwork::nExecuter>(vector<int>{5, 3, 2, 1}, *(pStringSensor.get()));
		}

		TEST_METHOD(tExecuter_Start_Pause)
		{
			auto pStringSensable = make_unique<StringSensable>("Test String");
			auto pStringSensor   = make_unique<StringSensor>(pStringSensable.get());
			auto pExecuter       = make_unique<nNetwork::nExecuter>(vector<int>{5, 3, 2, 1}, *(pStringSensor.get()));

			pExecuter->Start();

			// run 1000 iterations;
			while (pExecuter->GetCurrentIterations() < 1000) {}

			// Make sure that we have actually executed 1000 ticks
			Assert::IsTrue(pExecuter->GetCurrentIterations() >= 1000);

			pExecuter->Exit();		
		}
	};

}