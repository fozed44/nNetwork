//#include "stdafx.h"
#include <vector>
#include "CppUnitTest.h"
#include <memory>

#include "../nNetworkImplementation/nNetworkStringImplementation.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace tnNetwork
{		
	TEST_CLASS(tStringSensable)
	{
	public:
		
		TEST_METHOD(StringSensable_GetDimensionCount)
			// Build a string sensable, make sure that its dimension count is 1.
		{
			string testString { "Test string" };
			unique_ptr<StringSensable> ss = make_unique<StringSensable>(testString);

			auto result = ss->GetDimensionCount();

			Assert::AreEqual(1, result);
		}

		TEST_METHOD(StringSensable_GetDimensionLength) 
			// Build a string sensable, make sure that its dimension length is equal to the length
			// of the string used to construct the string sensable.
		{
			string testString{ "Test string" };
			unique_ptr<StringSensable> ss = make_unique<StringSensable>(testString);

			auto result = ss->GetDimensionLength(1);

			Assert::AreEqual(11, result);
		}

		TEST_METHOD(StringSensable_SenseA)
			// Build a string sensable, make sure that calling Sense at a particular location
			// returns the character in the string at the location sensed.
		{
			string testString{ "Test string" };
			unique_ptr<StringSensable> ss = make_unique<StringSensable>(testString);

			auto result = ss->Sense(vector<int>{0});
			Assert::AreEqual((unsigned char)'T', result);

			result = ss->Sense(vector<int>{1});
			Assert::AreEqual((unsigned char)'e', result);

			result = ss->Sense(vector<int>{2});
			Assert::AreEqual((unsigned char)'s', result);
		}

	};
}