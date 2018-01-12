#include "CppUnitTest.h"
#include "../nNetwork/nNetwork.h"
#include "../nNetworkStringImplementation/nNetworkStringImplementation.h"
#include <vector>
#include <memory>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace nNetwork;

namespace tNetwork
{
	TEST_CLASS(tNodeNetwork)
	{
	public:
		TEST_METHOD(tnNodeNetwork_ctor)
			// A nNodeNetwork is built during construction.
		{
			auto pStringSensable = new StringSensable("Test String");
			auto pStringSensor   = new StringSensor(pStringSensable);
			auto pNetwork        = new nNodeNetwork(vector<int>{5, 3, 2, 1}, *pStringSensor);
			
			Assert::IsNotNull(pNetwork);
			Assert::IsTrue(pNetwork != 0);			
		}

		TEST_METHOD(tnNodeNetwork_GetLayerCounts)
		{
			auto pStringSensable = new StringSensable("Test String");
			auto pStringSensor   = new StringSensor(pStringSensable);
			auto pNetwork        = new nNodeNetwork(vector<int>{5, 3, 2, 1}, *pStringSensor);

			auto layerCounts = pNetwork->GetLayerCounts();

			Assert::AreEqual(4, (int)layerCounts.size());
			Assert::AreEqual(1, layerCounts.back());
			layerCounts.pop_back();
			Assert::AreEqual(2, layerCounts.back());
			layerCounts.pop_back();
			Assert::AreEqual(3, layerCounts.back());
			layerCounts.pop_back();
			Assert::AreEqual(5, layerCounts.back());
		}

		TEST_METHOD(tNodeNetwork_ForEach)
			// General test of nNodeNetwork::ForEach...
			// Use the ForEach method to count the number of nodes in the
			// network.
		{

			auto pStringSensable = new StringSensable("Test String");
			auto pStringSensor   = new StringSensor(pStringSensable);
			auto pNetwork        = new nNodeNetwork(vector<int>{5, 3, 2, 1}, *pStringSensor);

			int count = 0;
			pNetwork->ForEach([&count](const nNode& node) {count++; });

			Assert::AreEqual(11, count);			
		}

		TEST_METHOD(tNodeNetwork_GetSnapShot_Easy)
			// Test the GetSnapShot method.
			// Create a new network with 11 total nodes, then create a copy
			// of that network and make sure the number of nodes in the copied
			// network is the same as the number of nodes in the source network.
		{
			auto pStringSensable = new StringSensable("Test String");
			auto pStringSensor   = new StringSensor(pStringSensable);
			auto pNetwork        = new nNodeNetwork(vector<int>{5, 3, 2, 1}, *pStringSensor);

			auto pSnapshot = pNetwork->GetSnapShot();

			int count = 0;
			
			pSnapshot->ForEach([&count](const nNode& node) { count++; });

			Assert::AreEqual(11, count);
		}

		TEST_METHOD(tNodeNetwork_GetSnapShot_Hard)
			// Test the GetSnapShot and ForEach.
			// Create a new network, then create a copy of that network and make
			// sure that each node in the copy has the same number of synapses as its
			// source node and that each synapse has the same weight as its source
			// synapse.

			// Also make sure that each of the copied nodes has the same CurrentValue as
			// the node that it was copied from.
		{
			auto pStringSensable = new StringSensable("Test String");
			auto pStringSensor   = new StringSensor(pStringSensable);
			auto pNetwork        = new nNodeNetwork(vector<int>{5, 3, 2, 1}, *pStringSensor);

			auto pSnapshot = pNetwork->GetSnapShot();

			pSnapshot->ForEach(
				[pNetwork](const nNode& node) 
				{
					auto sourceNode = pNetwork->GetNodeByNetworkId(node.GetNetworkId());

					Assert::AreEqual(sourceNode.GetCurrentValue(), node.GetCurrentValue());

					for (unsigned x = 0; x < node.Synapses.size(); ++x) {
						Assert::AreEqual(node.Synapses[x].weight, sourceNode.Synapses[x].weight);
					}
				}
			);
		}

		TEST_METHOD(tNodeNetwork_GetSensingNodes)
			// Check to make sure that the m_pSensingNodes vector is populated after building
			// the network
		{
			auto pStringSensable = new StringSensable("Test String");
			auto pStringSensor = new StringSensor(pStringSensable);
			auto pNetwork = new nNodeNetwork(vector<int>{5, 3, 2, 1}, *pStringSensor);

			vector<nSensingNode*>* result = pNetwork->GetSensingNodes();

			Assert::IsNotNull(result);
			Assert::AreEqual(5, (int)result->size());

		};

		TEST_METHOD(tNodeNetwork_GetResultNode) 
			// Check to make sure that m_pResultNode is populated after building the network.
		{
			unique_ptr<StringSensable> pStringSensable = make_unique<StringSensable>("Test String");
			unique_ptr<StringSensor>   pStringSensor   = make_unique<StringSensor>(pStringSensable.get());
			unique_ptr<nNodeNetwork>   pNetwork        = make_unique<nNodeNetwork>(vector<int>{5, 3, 2, 1}, *pStringSensor);

			nNode* pResult = pNetwork->GetResultNode();

			Assert::IsNotNull(pResult);
		}
	};
}