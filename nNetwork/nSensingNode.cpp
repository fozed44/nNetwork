#include "stdafx.h"
#include "nNetwork.h"

using namespace nNetwork;
using namespace std;

void nSensingNode::Sense(const ISensor& sensor) {
	m_currentValue += sensor.Sense(m_senseLocation);
	if (m_currentValue > NODE_TRIGGER_POINT)
	{
		for (auto synapse : Synapses)
			synapse.pNode->ActivateFromSynapse(synapse);
		m_currentValue = 0;
	}
}

