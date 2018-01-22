#include "stdafx.h"
#include "nNetwork.h"

using namespace nNetwork;
using namespace std;

nNodeNetworkConfig DefaultConfig {
	/*MinInitialSynapseWeight*/ 0.1,
	/*MaxInitialSynapseWeight*/ 0.2,

	/*MinInitialDecay*/ 0.1,
	/*MaxInitialDecay*/ 0.05,

	/*MinResetCount*/ 3,
	/*MaxResetCount*/ 3,

	/*SensesPerTick*/ 4,
	[](int nodeLocation) { return vector<int>{nodeLocation}; }
};

vType GenerateInitialWeight(const nNodeNetworkConfig &config) {
	vType r = (vType)rand() / (vType)INT_MAX;
	vType min = config.MinInitialSynapseWeight;
	vType delta = config.MaxInitialSynapseWeight - config.MinInitialSynapseWeight;

	return min + (delta * r);
}

vType GenerateInitialDecay(const nNodeNetworkConfig &config) {
	vType r = (vType)rand() / (vType)INT_MAX;
	vType min = config.MinInitialDecay;
	vType delta = config.MaxInitialDecay - config.MinInitialDecay;

	return min + (delta * r);
}

int GenerateInitialRestCount(const nNodeNetworkConfig &config) {
	if (config.MaxRestCount == config.MinRestCount)
		return config.MaxRestCount;

	int delta = config.MaxRestCount - config.MinRestCount;
	delta = rand() % delta;

	return config.MinRestCount + delta;
}

nNodeNetwork::nNodeNetwork(const vector<int>& layerCounts, const ISensor& sensor)
	: nNodeNetwork::nNodeNetwork(layerCounts, sensor, DefaultConfig)
{
}

nNodeNetwork::nNodeNetwork(const vector<int>& layerCounts, const ISensor& sensor, const nNodeNetworkConfig& config)
	: m_sensor{ sensor }
	, m_config{ config }
	, m_nextNetworkId{ 0 }
	, m_tickCount{ 0 }
	// Construct a new nNodeNetwork
{
	BuildNetwork(layerCounts, sensor);
}

nNodeNetwork::~nNodeNetwork() 
	// release the nodes in m_layers.
{
	for (auto pLayer : m_layers) {
		if (pLayer) {
			for (auto pNode : *pLayer)
				delete pNode;
			delete pLayer;
		}
	}
}

const ISensor& nNodeNetwork::GetSensor() const {
	return m_sensor;
}

vector<int> nNodeNetwork::GetLayerCounts() const
{
	vector<int> result;
	for (auto layer : m_layers) {
		result.push_back(layer->size());
	}
	return std::move(result);
}

const nNode& nNodeNetwork::GetNodeByGlobalId(int globalId) const
	// Return the node that has the requested globalId.
{
	for (auto layer : m_layers)
	{
		for (auto nNode : *layer)
		{
			if (nNode->GetGlobalId() == globalId)
				return *nNode;
		}
	}

	throw "globalId not found.";
}

const nNode& nNodeNetwork::GetNodeByNetworkId(int networkId) const
	// Return the node that has the requested networkId.
{
	for (auto layer : m_layers)
	{
		for (auto nNode : *layer)
		{
			if (nNode->GetNetworkId() == networkId)
				return *nNode;
		}
	}

	throw "No node with this networkId exists.";
}

nNode& nNodeNetwork::GetMutableNodeByGlobalId(int globalId) const
	// Return the node with the requested globalId
{
	for (auto pLayer : m_layers) {
		for (auto pNode : *pLayer) {
			if (pNode->GetGlobalId() == globalId)
				return *pNode;
		}
	}

	throw "No node with this globalId exists.";
}

nNode& nNodeNetwork::GetMutableNodeByNetworkId(int networkId) const
// Return the node with the requested networkId
{
	for (auto pLayer : m_layers) {
		for (auto pNode : *pLayer)
			if (pNode->GetNetworkId() == networkId)
				return *pNode;
	}

	throw "No node with this networkId exists.";
}

void nNodeNetwork::BuildFirstLayer(int count, const ISensor& sensor)
	// The first layer contains nSensingNodes. 
	// All sensing nodes point to the same ISensor, which points to a single ISensable.		
{	
	auto newLayer = new vector<nNode*>();

	// Create 'count' new nodes, add them to the new layer.
	for (int x = 0; x < count; ++x) {
		auto decay = GenerateInitialDecay(m_config);
		auto maxRestCount = GenerateInitialRestCount(m_config);
		newLayer->emplace_back(new nSensingNode(m_nextNetworkId++, decay, maxRestCount, m_config.pSensorLocationMapper(x)));
	}

	m_layers.push_back(newLayer);
}

void nNodeNetwork::BuildNextLayer(int count)
{
	std::vector<nNode*>* layer = new std::vector<nNode*>();

	for (int x = 0; x < count; ++x) {
		auto decay = GenerateInitialDecay(m_config);
		auto maxRestCount = GenerateInitialRestCount(m_config);
		layer->push_back(new nNode(m_nextNetworkId++, decay, maxRestCount));
	}

	m_layers.push_back(layer);
}

void nNodeNetwork::BuildNetwork(const vector<int>& layerCounts, const ISensor& sensor)
	// Builds the network. 
	// The first layer is a layer of nSensingNodes, while all other layers are built using regular
	// nNode objects.
{
	BuildFirstLayer(layerCounts[0], sensor);

	for (uint32_t x = 1; x < layerCounts.size(); ++x)
		BuildNextLayer(layerCounts[x]);

	CopySensingLayer();
	CopyResultNode();

	BuildSynapses();
}

void nNodeNetwork::CopySensingLayer() 
	// The m_sensingLayer member keeps a vector of pointers to the sensing nodes.
{
	m_sensingLayer.clear();

	for (auto pNode : *(m_layers.front())) {
		m_sensingLayer.push_back(dynamic_cast<nSensingNode*>(pNode));
	}

}

void nNodeNetwork::CopyResultNode()
	// m_pResultNode keeps a pointer to the node at the top of network for convenience.
{
	m_pResultNode = m_layers.back()->back();
}

void nNodeNetwork::BuildSynapses() const {
	for (unsigned int x = 0; x < m_layers.size() - 1; ++x) {
		BuildLayerSynapses(m_layers[x], m_layers[x + 1]);
	}
}

void nNodeNetwork::BuildLayerSynapses(const vector<nNode*>* const bottomLayer, const vector<nNode*>* const topLayer) const
{
	for (auto bottomNode : *bottomLayer) {
		for (auto topNode : *topLayer) {
			bottomNode->Synapses.push_back(
				nSynapse { GenerateInitialWeight(m_config), topNode }
			);
		}
	}
}

void nNodeNetwork::ForEach(std::function<void(const nNode&)> fn) const {
	for (auto pLayer : m_layers)
		for (auto pNode : *pLayer)
			fn(*pNode);
}

unique_ptr<nNodeNetwork> nNodeNetwork::GetSnapShot() const
	// Make a copy of the network in its current state. The result is a completely new
	// network that is owned by the caller.
{
	auto result = make_unique<nNodeNetwork>(GetLayerCounts(), m_sensor);	

	for (auto layer : m_layers)
	{
		for (auto node : *layer)
		{
			const nNode& sourceNode = GetNodeByNetworkId(node->GetNetworkId());
			nNode& destNode = result->GetMutableNodeByNetworkId(node->GetNetworkId());

			destNode.SetCurrentValue(sourceNode.GetCurrentValue());

			for (int x = 0; x != sourceNode.Synapses.size(); x++) 
			{
				destNode.Synapses[x].weight = sourceNode.Synapses[x].weight;
				
			}
		}
	}

	return result;
}

void nNodeNetwork::Tick()
// Call nNode.Tick() on each node that is stored in the m_layers vector.
{
	SenseTick();
	if (!(m_tickCount++ % m_config.SensesPerTick))
		NodeTick();
}

void nNodeNetwork::SenseTick()
// Call Sense on all of the sensing nodes.
{
	for (auto pNode : m_sensingLayer)
		pNode->Sense(m_sensor);
}

void nNodeNetwork::NodeTick()
// Call tick on all nodes.
{
	for (auto pLayer : m_layers)
		for (auto pNode : *pLayer)
			pNode->Tick();
}

#ifdef __DEBUG__

nNode* nNodeNetwork::GetResultNode() {
	return m_pResultNode;
}

vector<nSensingNode*>* nNodeNetwork::GetSensingNodes() {
	return &m_sensingLayer;
}

#endif