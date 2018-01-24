#include "CppUnitTest.h"
#include "../nNetwork/nNetwork.h"
#include "../nNetworkImplementation/nNetworkStringImplementation.h"
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

		TEST_METHOD(tNodeNetwork_SingleNodeNetwork)
			// Perform edge testing where there is a single node in the network.1
		{
			unique_ptr<StringSensable> pStringSensable = make_unique<StringSensable>("Test String");
			unique_ptr<StringSensor>   pStringSensor   = make_unique<StringSensor>(pStringSensable.get());
			unique_ptr<nNodeNetwork>   pNetwork        = make_unique<nNodeNetwork>(vector<int>{1}, *pStringSensor);

			nNode* pResultNode = pNetwork->GetResultNode();
			
			// Make sure we can get the result node.
			Assert::IsNotNull(pResultNode);

			vector<nSensingNode*>* pSensingNodes = pNetwork->GetSensingNodes();

			// Make sure we can get the sensing nodes (just one node in this case).
			Assert::IsNotNull(pSensingNodes);

			// Should be a single node in pSensingNodes.
			Assert::AreEqual(1, (int)pSensingNodes->size());

			// Make sure there is actually a node in pSensingNodes.
			Assert::IsNotNull((*pSensingNodes)[0]);

			// The result node and the sensing node should be the same node.
			Assert::AreEqual(reinterpret_cast<int>((*pSensingNodes)[0]), reinterpret_cast<int>(pResultNode));
		}

		TEST_METHOD(tNodeNetwork_SensingLayerSynapses)
			// Test each node in the sensing layer, making sure that each sensing node
			// has a synapse for each node in the layer above the sensing layer.

			// For each synapse, make sure that the node pointer for that synapse is pointing
			// to the correct node in the next layer.

		{
			unique_ptr<StringSensable> pStringSensable = make_unique<StringSensable>("Test String");
			unique_ptr<StringSensor>   pStringSensor = make_unique<StringSensor>(pStringSensable.get());
			unique_ptr<nNodeNetwork>   pNetwork = make_unique<nNodeNetwork>(vector<int>{5, 3, 2, 1}, *pStringSensor);

			Assert::IsNotNull(pNetwork.get());

			// Get the sensing layer
			vector<nSensingNode*>* pSensingLayer = pNetwork->GetSensingNodes();

			Assert::IsNotNull(pSensingLayer);

			// Get the layer above the sensing layer
			vector<nNode*>* pNextLayer = pNetwork->GetLayer(1);

			// Make sure we have the right layer, we are looking for the layer just after
			// the sensing layer.
			Assert::IsNotNull(pNextLayer);
			Assert::AreEqual(3, (int)pNextLayer->size());

			for (auto pSensingNode : *pSensingLayer) {

				// Make sure the number of synapses in this node is correct (should be 3 since the layer
				// above the sensing layer has 3 nodes.
				Assert::AreEqual(3, (int)pSensingNode->Synapses.size());

				// Using reinterprate_cast<int> to compare references
				Assert::AreEqual(
					reinterpret_cast<int>(pSensingNode->Synapses[0].pNode),
					reinterpret_cast<int>((*pNextLayer)[0])
				);

				Assert::AreEqual(
					reinterpret_cast<int>(pSensingNode->Synapses[1].pNode),
					reinterpret_cast<int>((*pNextLayer)[1])
				);

				Assert::AreEqual(
					reinterpret_cast<int>(pSensingNode->Synapses[2].pNode),
					reinterpret_cast<int>((*pNextLayer)[2])
				);
			}

		}

		TEST_METHOD(tnNodeNetwork_NextLayerSynapses)
			// Test each node in the layer found just after the sensing layer. Each node should have
			// a synapse for each node in next layer. Further, we want to make sure that the node
			// pointer for each synapse is pointing to the correct node.

			// FOR EXAMPLE

			// Assume that the sensing layer is layer 0.
			// Assume that the layer after the sensing layer is 1.
			// Assume that the next layer after layer 1 is layer 2.

			// We want to make sure that each node in layer 1 has a vector of synapses and that
			// there is a synapse in that vector for each node in layer 2.

		{
			unique_ptr<StringSensable> pStringSensable = make_unique<StringSensable>("Test String");
			unique_ptr<StringSensor>   pStringSensor = make_unique<StringSensor>(pStringSensable.get());
			unique_ptr<nNodeNetwork>   pNetwork = make_unique<nNodeNetwork>(vector<int>{5, 4, 3, 1}, *pStringSensor);

			Assert::IsNotNull(pNetwork.get());

			// Get layer 1
			vector<nNode*>* pLayer1 = pNetwork->GetLayer(1);

			// Make sure we have the right layer by checking the number of nodes in the layer.
			Assert::IsNotNull(pLayer1);
			Assert::AreEqual(4, (int)pLayer1->size());

			// Get layer 2
			vector<nNode*>* pLayer2 = pNetwork->GetLayer(2);

			// Make sure we have the right layer by checking the number of nodes in the layer.
			Assert::IsNotNull(pLayer2);
			Assert::AreEqual(3, (int)pLayer2->size());

			for (auto pLayer1Node : *pLayer1) {

				// Make sure the number of synapses in this node is correct (should be 3 since the layer
				// above the sensing layer has 3 nodes.
				Assert::AreEqual(3, (int)pLayer1Node->Synapses.size());

				// Using reinterprate_cast<int> to compare references
				Assert::AreEqual(
					reinterpret_cast<int>(pLayer1Node->Synapses[0].pNode),
					reinterpret_cast<int>((*pLayer2)[0])
				);

				Assert::AreEqual(
					reinterpret_cast<int>(pLayer1Node->Synapses[1].pNode),
					reinterpret_cast<int>((*pLayer2)[1])
				);

				Assert::AreEqual(
					reinterpret_cast<int>(pLayer1Node->Synapses[2].pNode),
					reinterpret_cast<int>((*pLayer2)[2])
				);
			}

		}

		TEST_METHOD(tnNodeNetwork_ResultNodeGetsFired)
			// Make sure that the result node of a network will at some point get triggered.
		{
			nNodeNetworkConfig Config{
				/*MinInitialSynapseWeight*/ 0.1,
				/*MaxInitialSynapseWeight*/ 0.2,

				/*MinInitialDecay*/ 0.001,
				/*MaxInitialDecay*/ 0.0005,

				/*MinResetCount*/ 3,
				/*MaxResetCount*/ 3,

				[](int nodeLocation) { return vector<int>{nodeLocation}; }
			};

			unique_ptr<StringSensable> pStringSensable = make_unique<StringSensable>("Test String");
			unique_ptr<StringSensor>   pStringSensor   = make_unique<StringSensor>(pStringSensable.get());
			unique_ptr<nNodeNetwork>   pNetwork        = make_unique<nNodeNetwork>(vector<int>{5, 3, 2, 1}, *pStringSensor, Config);
			
			bool resultNodeGreaterThanZero = 0;

			int tickCount = 0;

			for (int x = 100; x; --x, ++tickCount)
			{
				pNetwork->Tick();

				nNode* pNode = pNetwork->GetLayer(0)->at(0);

				vType result = pNode->GetCurrentValue();

				bool bres = result > 0;

				resultNodeGreaterThanZero = resultNodeGreaterThanZero ? true : pNetwork->GetCurrentValue();
			}

			Assert::IsTrue(resultNodeGreaterThanZero);

			// Here we are making sure that the result node has not been altered before a reasonable
			// number of ticks. (this could indicate a bug)
			Assert::IsTrue(tickCount > 10);
		}
	};
	
	
}