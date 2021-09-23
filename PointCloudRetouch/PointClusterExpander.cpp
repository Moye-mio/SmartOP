#include "pch.h"
#include "PointClusterExpander.h"
#include "common/CpuTimer.h"
#include "PointCluster.h"
#include "PointCloudRetouchManager.h"

using namespace hiveObliquePhotography::PointCloudRetouch;

_REGISTER_NORMAL_PRODUCT(CPointClusterExpander, KEYWORD::CLUSTER_EXPANDER)

//*****************************************************************
//FUNCTION: 
void CPointClusterExpander::runV(const CPointCluster* vCluster)
{
	if (vCluster == nullptr || vCluster->getCoreRegion().size() == 0)   //FIXME-014: ����Ĳ�����Ч�Լ��ʧ�������쳣�ķ�ʽ̫����
		_THROW_RUNTIME_ERROR("Expander input error");

	m_ExpandedPointSet.clear();

	CPointCloudRetouchManager *pManager = CPointCloudRetouchManager::getInstance();
	std::queue<pcl::index_t> ExpandingCandidateQueue = __initExpandingCandidateQueue(vCluster);
	std::deque TraversedFlag(pManager->getScene().getNumPoint(), false);  //FIXME-014��Ϊʲô��deque���о����ǰ�vector����
	
#ifdef _UNIT_TEST
	hiveCommon::CCPUTimer Timer;
	Timer.start();
#endif // _UNIT_TEST
	
	while (!ExpandingCandidateQueue.empty())
	{
		pcl::index_t Candidate = ExpandingCandidateQueue.front();
		ExpandingCandidateQueue.pop();

		if (TraversedFlag.at(Candidate))
			continue;
		else
			TraversedFlag.at(Candidate) = true;
		
		std::size_t CandidateLabel;  //FIXME-014�����Ƿ����㣬CandidateLabel����Ҫ��std::size_t����dumpPointLabelAt()��ǿ�ư�EPointLabelתΪsize_t���õ���������ǿ��ת����EPointLabel
		pManager->dumpPointLabelAt(CandidateLabel, Candidate);
		if (vCluster->getLabel() == EPointLabel::UNWANTED && static_cast<EPointLabel>(CandidateLabel) == EPointLabel::KEPT)
			continue;
		
		std::uint32_t OldClusterIndex = pManager->getClusterIndexAt(Candidate);
		//_ASSERTE(OldClusterIndex != vCluster->getClusterIndex());      //FIXME-014������ע�����д�����������ԭ����

		double CurrentProbability = vCluster->evaluateProbability(Candidate);
		if (vCluster->isBelongingTo(CurrentProbability))
		{
			if (OldClusterIndex == 0 ||  //FIXME-014: �����0��ʲô��˼��һ�������index������ǣ���#define����߿ɶ���
				__isReassigned2CurrentCluster(CurrentProbability, vCluster->getClusterIndex(), pManager->getClusterBelongingProbabilityAt(Candidate), OldClusterIndex))
			{
				if (static_cast<EPointLabel>(CandidateLabel) != EPointLabel::DISCARDED) pManager->tagPointLabel(Candidate, vCluster->getLabel(), vCluster->getClusterIndex(), CurrentProbability);
				m_ExpandedPointSet.push_back(Candidate);  //FIXME-014���������˼�ǣ���ʹCandidate�����ΪDISCARDED����Ҳ�������������������
				for (auto e : pManager->buildNeighborhood(Candidate)) ExpandingCandidateQueue.push(e);  //FIXME-014����������Ҫ����������������buildNeighborhood���ù����ٴΣ�Ч�ʲ�������
			}
		}
		else
		{//FIXME-014: else���ֹ���ע���������
		//	//WARNING!
		//	pManager->tagPointLabel(Candidate, EPointLabel::FILLED, pManager->getClusterIndexAt(Candidate), pManager->getClusterBelongingProbabilityAt(Candidate));
			//pManager->tagPointLabel(Candidate, vCluster->getLabel(), vCluster->getClusterIndex(), CurrentProbability);
		//	//WARNING!
		//	hiveEventLogger::hiveOutputEvent(_FORMAT_STR2("Point: %1% is left in expander, its probability is %2%, below are infos:\n", Candidate, CurrentProbability) + vCluster->getDebugInfos(Candidate));
		}
	}

#ifdef _UNIT_TEST
	Timer.stop();
	m_RunTime = Timer.getElapsedTimeInMS();
#endif // _UNIT_TEST


	pManager->recordCurrentStatus();
}

//*****************************************************************
//FUNCTION: 
std::queue<pcl::index_t> CPointClusterExpander::__initExpandingCandidateQueue(const CPointCluster* vCluster)
{
	std::queue<pcl::index_t> CandidateQueue;
	for (auto Index : vCluster->getCoreRegion())
	{
		for (auto Neighbor : CPointCloudRetouchManager::getInstance()->buildNeighborhood(Index))  //FIXME-014����ÿ��core region��ĵ㶼ȥ�����򣬲���ʱ�� ��û���������õķ������������ǽ��Ƶ�Ч�ʸ��ߵģ�
			if (find(vCluster->getCoreRegion().begin(), vCluster->getCoreRegion().end(), Neighbor) == vCluster->getCoreRegion().end())
				CandidateQueue.push(Neighbor);
	}
	//����NRVO
	return CandidateQueue;
}

//*****************************************************************
//FUNCTION: 
bool CPointClusterExpander::__isReassigned2CurrentCluster(double vCurrentProbability, std::uint32_t vCurrentTimestamp, double vOldProbability, std::uint32_t vOldTimestamp)
{
	const double X = (vCurrentTimestamp - vOldTimestamp - 1.0) / 3.0;  //FIXME-014��һ���magic number������������߼���ʲô��

	return vCurrentProbability > vOldProbability
	|| vCurrentTimestamp - vOldTimestamp > 4
	|| vCurrentProbability > vOldProbability * 2.0 * pow(X, 3.0) - 3.0 * pow(X, 2.0) + 1.0;
}