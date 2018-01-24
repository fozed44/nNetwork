#include "stdafx.h"
#include "nNetwork.h"

using namespace std;
using namespace nNetwork;

int nNode::s_currentGlobalId{ 0 };

nNode::nNode(int networkId) : m_networkId{ networkId }
{
	m_globalId = s_currentGlobalId++;
}

nNode::~nNode() {}

void nNode::Fire()
{
	if (!m_restCount)
	{
		for (auto synapse : Synapses)
			synapse.pNode->ActivateFromSynapse(synapse);

		// After firing the node rests.
		m_restCount = m_maxRestCount;

		// And the current value decays to 0
		m_currentValue = 0;
	}
}

void nNode::ActivateFromSynapse(const nSynapse& synapse)
	// Called by another node when the other node is firing....
{

	if (!m_restCount)
	{
		m_currentValue += synapse.weight;

		// Keep m_currentValue clipped to 1.0
		if (m_currentValue > 1.0)
			m_currentValue = 1.0;

		if (m_currentValue > NODE_TRIGGER_POINT)
			Fire();
	}
}

void nNode::Tick()
	// The node decays on each tick.
	// If the node is resting (m_restCount > 0) decrement m_restCount.
{

	m_currentValue -= m_decay;
	if (m_currentValue < 0)
		m_currentValue = 0;

	if (m_restCount)
		--m_restCount;
}
 