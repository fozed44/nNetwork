#pragma once

#include <vector>
#include <memory>

#include <functional>
#include <thread>
#include <mutex>

#define __DEBUG__

// Define the base type of the node values, weight values, etc.
// Should only ever be float or double.
using vType = double;

// The value that nNode::m_currentValue must reach for the node to fire.
// must be <= 1.0; nNode::m_currentValue cannot take on values greater
// than 1.0;
const vType NODE_TRIGGER_POINT = 0.95;

namespace nNetwork {

	// Forward declarations
	class nNode;

	//++ ISensable
	//
	//+ Purpose:
	//		An object that presents data to a sensor.
	template<typename T> class ISensable {
	public:
		virtual ~ISensable() {};

		virtual int GetDimensionCount() const = 0;
		virtual int GetDimensionLength(int dimensionNumber) const = 0;

		virtual T Sense(const std::vector<int>& location) const = 0;
	};

	//++ ISensor
	///
	//+ Purpose:
	//		Connection between a nSensingNode and an ISensable.
	//
	//+ Remarks:
	//		All sensing nodes in a network can/should connect to an ISensable via
	//		the same sensor.
	class ISensor {
	public:
		virtual ~ISensor() {};
		virtual vType Sense(const std::vector<int>& senseLocation) const = 0;
	};

	//++ nSynapse
	struct nSynapse {
		vType weight;
		nNode *pNode;
	};

	//++ nNode
	//
	//+ Purpose:
	//		Represents a normal neuron. Sensing nodes are derived from nNode
	class nNode {
		friend class nSensingNode;
	public:
		nNode(int networkId);
		virtual ~nNode();

		std::vector<nSynapse> Synapses;

		int   GetNetworkId()    const { return m_networkId; }
		int   GetGlobalId()     const { return m_globalId; }
		vType GetCurrentValue() const { return m_currentValue; }
		void  SetCurrentValue(vType value) { m_currentValue = value; }
		

		void Tick();

	protected:
		static int s_currentGlobalId;

		int   m_globalId;  // Globally identifies the node.
		int   m_networkId; // Locally identifies the node. In a copied nNetwork (created via GetSnapshot())
						   // matching nodes have matching ids.


		// The current VALUE of the node... The node will fire if m_currentValue goes above
		// NODE_TRIGGER_POINT;
		vType m_currentValue;

		// m_currentValue will decay by this amount on each call to Tick.
		vType m_decay;

		// Once a node fires, it must rest this many ticks before it can fire again.
		int m_maxRestCount;

		// When a node fires, m_restCount is set to m_maxRestCount. If m_restCount is > 0
		// Tick decrements m_restCount rather and performing an actual tick. m_restCount
		// must reach 0 before the node starts up again.
		// ActivateFromSynapse() also does nothing if m_restCount > 0.
		int m_restCount = 0;

		void Fire();
		void ActivateFromSynapse(const nSynapse& synapse);
	};

	//++ nSensingNode
	//
	//+ Purpose:
	//		Node that is capable of a connection to an ISensor implementation.
	class nSensingNode : public nNode
	{
	public:
		nSensingNode(int networkId, const std::vector<int>& senseLocation) : nNode(networkId), m_senseLocation{ senseLocation } {};
		virtual ~nSensingNode() {};

		virtual void Sense(const ISensor& sensor);
	protected:
		std::vector<int> m_senseLocation;
	};
	
	//++ nNodeNetworkConfig
	//
	//+ Purpose:
	//		
	//++ nNodeNetwork
	//		nNodeNetwork configuration passed to the nNodeNetwork ctor	
	struct nNodeNetworkConfig
	{
		// Min and max synapse weights.
		vType MinInitialSynapseWeight;
		vType MaxInitialSynapseWeight;

		// During each Tick(), m_currentValue -= m_decay;
		vType MinInitialDecay;
		vType MaxInitialDecay;

		// Every time a node fires, it node must 'rest' for ResetCount ticks
		// before it can fire again.
		int MinRestCount;
		int MaxRestCount;

		// Tick is called once every SensesPerTick times Sense() is called.
		int   SensesPerTick;

		std::vector<int>(*pSensorLocationMapper)(int);
	};



	//+ Purpose:
	//		Container for a neural network.
	class nNodeNetwork
	{
	public:
		nNodeNetwork(const std::vector<int>& layerCounts, const ISensor& sensor);
		nNodeNetwork(const std::vector<int>& layerCounts, const ISensor& sensor, const nNodeNetworkConfig& config);
		~nNodeNetwork();

		void Tick();
		
		std::unique_ptr<nNodeNetwork> GetSnapShot() const;

		std::vector<int>    GetLayerCounts() const;

		const ISensor& GetSensor() const;

		const nNode& GetNodeByGlobalId(int globalId)   const;
		const nNode& GetNodeByNetworkId(int networkId) const;

		void ForEach(std::function<void(const nNode&)> fn) const;

#ifdef __DEBUG__

		nNode* GetResultNode();
		std::vector<nSensingNode*>* GetSensingNodes();

#endif

	private:
		nNodeNetworkConfig m_config;
		const ISensor&     m_sensor;
		int                m_nextNetworkId;

		// m_tickCount is used to determine when to call sense on the sensing nodes. Its just
		// used to track the ratio between SenseTick and NodeTick
		int m_tickCount;

		// Call Sense() on all sensing nodes.
		void SenseTick();

		// Call Tick() on all nodes.
		void NodeTick();

		// _layers is the owner of the network memory.
		std::vector<std::vector<nNode*>*> m_layers;

		// _topLayer contains pointers to nodes owned by m_layers.
		std::vector<nSensingNode*> m_sensingLayer;

		// m_pResultNode is also owned by m_layers
		nNode*	m_pResultNode;

		/*-----------------------------------------------------------------------------------------
			Network building methods.
		-----------------------------------------------------------------------------------------*/
		void BuildFirstLayer(int count, const ISensor& sensor);
		void BuildNextLayer(int count);
		void BuildNetwork(const std::vector<int>& layerCounts, const ISensor& sensor);
		void BuildSynapses() const;
		void BuildLayerSynapses(const std::vector<nNode*>* const bottomLayer, const std::vector<nNode*>* const topLayer) const;
		void CopySensingLayer();
		void CopyResultNode();

		/*-----------------------------------------------------------------------------------------
			Methods used for copying a network.
		-----------------------------------------------------------------------------------------*/
		nNode& GetMutableNodeByGlobalId(int globalId) const;
		nNode& GetMutableNodeByNetworkId(int networkId) const;
		
	};

	struct nExecuterContext {
		int  ExitToken{ 0 };
		int  PauseToken{ 1 };
		int  CurrentIteration{ 0 };
		mutable std::mutex Lock;
	};

	class nExecuter
		// Container to execute a network in its own thread.
	{
	public:
		virtual ~nExecuter();
		nExecuter(const std::vector<int>& layerCounts, const ISensor& sensor);
		nExecuter(const std::vector<int>& layerCounts, const ISensor& sensor, nNodeNetworkConfig config);

		void Start();
		void Pause();
		void Exit();

		int   GetCurrentIterations() const;

		std::unique_ptr<nNodeNetwork> GetSnapShot() const;

	private:
		std::unique_ptr<nNodeNetwork> m_pNetwork;
		std::unique_ptr<std::thread>  m_pThread;
		nExecuterContext              m_executerContext;

		static void ThreadExecuter(nNodeNetwork *pNetwork, nExecuterContext *pContext);
	};

}