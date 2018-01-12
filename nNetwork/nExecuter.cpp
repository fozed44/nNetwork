#include "nNetwork.h"

namespace nNetwork {

	nExecuter::nExecuter(const vector<int>& layerCounts, const ISensor& sensor) 
	{
		m_pNetwork = make_unique<nNodeNetwork>(layerCounts, sensor);
		m_pThread = make_unique<thread>(nExecuter::ThreadExecuter, m_pNetwork.get(), &m_executerContext);
	}

	nExecuter::nExecuter(const vector<int>& layerCounts, const ISensor& sensor, nNodeNetworkConfig config) 
	{
		m_pNetwork = make_unique<nNodeNetwork>(layerCounts, sensor, config);
		m_pThread = make_unique<thread>(nExecuter::ThreadExecuter, m_pNetwork.get(), &m_executerContext);
	}

	nExecuter::~nExecuter()
	{
		if (!m_executerContext.ExitToken) {
			Exit();
		}
		if (m_pThread->joinable())
			m_pThread->join();
	}

	void nExecuter::Start()
	{
		m_executerContext.PauseToken = 0;
	}

	void nExecuter::Pause()
	{
		m_executerContext.PauseToken = 1;
	}

	void nExecuter::Exit()
	{
		m_executerContext.ExitToken = 1;
	}


	int nExecuter::GetCurrentIterations() const
	{
		return m_executerContext.CurrentIteration;
	}

	unique_ptr<nNodeNetwork> nExecuter::GetSnapShot() const
	{
		lock_guard<mutex> lock { m_executerContext.Lock };
		return m_pNetwork->GetSnapShot();
	}

	void nExecuter::ThreadExecuter(nNodeNetwork *pNetwork, nExecuterContext *pContext)
	{
		while (1)
		{
			if (pContext->ExitToken) return;

			if (pContext->PauseToken) continue;
			
			lock_guard<mutex> lock { pContext->Lock };
			pNetwork->Tick();
			++pContext->CurrentIteration;
		}
	}
}