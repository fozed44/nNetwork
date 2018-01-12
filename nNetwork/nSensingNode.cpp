#include "stdafx.h"
#include "nNetwork.h"

#define CLIP(A) if((A)>1.0) { (A) = 1.0; }

using namespace nNetwork;

void nSensingNode::Sense(const ISensor& sensor) {
	m_currentValue += sensor.Sense(m_senseLocation);
	if (m_currentValue > NODE_TRIGGER_POINT)
	{
		for (auto synapse : Synapses)
			synapse.pNode->ActivateFromSynapse(synapse);
		m_currentValue = 0;
	}
}
