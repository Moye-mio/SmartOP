#include "pch.h"
#include "PointCluster.h"
#include "PointClusterExpander.h"
#include "PointCloudRetouchManager.h"

using namespace hiveObliquePhotography::PointCloudRetouch;

_REGISTER_NORMAL_PRODUCT(CPointClusterExpander, KEYWORD::CLUSTER_EXPANDER)

//*****************************************************************
//FUNCTION: 
void CPointClusterExpander::runV(const CPointCluster* vCluster)
{
	if (vCluster == nullptr)
		_THROW_RUNTIME_ERROR("Expander input error");

	CPointCloudRetouchManager *pManager = CPointCloudRetouchManager::getInstance();

	std::queue<pcl::index_t> ExpandingCandidateQueue = __initExpandingCandidateQueue(vCluster);

	std::vector<pcl::index_t> Neighborhood;
	while (!ExpandingCandidateQueue.empty())
	{
		pcl::index_t Candidate = ExpandingCandidateQueue.front();
		ExpandingCandidateQueue.pop();

		std::uint32_t OldClusterIndex = pManager->getClusterIndexAt(Candidate);
		_ASSERTE(OldClusterIndex != vCluster->getClusterIndex());

		bool IsClusterIndexRequired2Change = false;
		double CurrentProbability = vCluster->evaluateProbability(Candidate);
		if (vCluster->isBelongingTo(CurrentProbability))
		{
			IsClusterIndexRequired2Change = (OldClusterIndex == 0) ? true :
				__isReassigned2CurrentCluster(CurrentProbability, vCluster->getClusterIndex(), pManager->getClusterBelongingProbabilityAt(Candidate), OldClusterIndex);
		}

		if (IsClusterIndexRequired2Change)
		{
			pManager->tagPointLabel(Candidate, vCluster->getLabel(), vCluster->getClusterIndex(), CurrentProbability);
			pManager->buildNeighborhood(Candidate, vCluster->getClusterIndex(), Neighborhood);
			for (auto e : Neighborhood) ExpandingCandidateQueue.push(e);
		}
	}
}

//*****************************************************************
//FUNCTION: 
std::queue<pcl::index_t> CPointClusterExpander::__initExpandingCandidateQueue(const CPointCluster* vCluster)
{
	std::queue<pcl::index_t> CandidateQueue;
	const auto SeedClusterIndex = vCluster->getClusterIndex();
	for(auto Index : vCluster->getCoreRegion())
	{
		std::vector<pcl::index_t> Neighborhood;
		CPointCloudRetouchManager::getInstance()->buildNeighborhood(Index, SeedClusterIndex, Neighborhood);
		for (auto Neighbor : Neighborhood) 
			CandidateQueue.push(Neighbor);
	}
	//����NRVO
	return CandidateQueue;
}

//*****************************************************************
//FUNCTION: 
bool CPointClusterExpander::__isReassigned2CurrentCluster(double vCurrentProbability, std::uint32_t vCurrentTimestamp, double vOldProbability, std::uint32_t vOldTimestamp)
{
	return true;
	return vCurrentProbability > vOldProbability || (vCurrentProbability > vOldProbability / 2 && vCurrentTimestamp - vOldTimestamp > 5);

}