#include "pch.h"
#include "PointLabelSet.h"

using namespace hiveObliquePhotography::PointCloudRetouch;

//*****************************************************************
//FUNCTION: 
void CPointLabelSet::tagCoreRegion4Cluster(const std::vector<pcl::index_t>& vTargetPointSet, EPointLabel vTargetLabel, std::uint32_t vClusterIndex)
{
	for (auto e : vTargetPointSet)
	{
		if (__throwLabelIndexOutOfRange(e)) 
			break;
		
		m_LabelSet[e].ClusterIndex = vClusterIndex;
		m_LabelSet[e].PointLabel   = vTargetLabel;
		m_LabelSet[e].Probability  = 1.0;
	}
}

//*****************************************************************
//FUNCTION: 
void CPointLabelSet::init(std::size_t vSize)
{
	_ASSERTE(vSize > 0);
	m_LabelSet = std::vector<SPointLabel>(vSize);
}

//*****************************************************************
//FUNCTION: 
void CPointLabelSet::clear()
{
	m_LabelSet.clear();
	m_LabelSet.shrink_to_fit();
}

//*****************************************************************
//FUNCTION: 
void CPointLabelSet::reset()
{
	m_LabelSet = std::vector<SPointLabel>(m_LabelSet.size());
}

//*****************************************************************
//FUNCTION: 
void CPointLabelSet::tagPointLabel(pcl::index_t vPoint, EPointLabel vTargetLabel, std::uint32_t vClusterIndex, double vClusterBelongingProbability)
{
	if (__throwLabelIndexOutOfRange(vPoint)) return;
	if (vClusterBelongingProbability < 0)
		//_THROW_RUNTIME_ERROR("Illegal probability input");	// 
		return;
	m_LabelSet[vPoint].PointLabel = vTargetLabel;
	m_LabelSet[vPoint].ClusterIndex = vClusterIndex;
	m_LabelSet[vPoint].Probability = vClusterBelongingProbability;
}