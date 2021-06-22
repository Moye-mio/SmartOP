#include "pch.h"
#include "PointCluster.h"
#include "Feature.h"

using namespace hiveObliquePhotography::PointCloudRetouch;

//*****************************************************************
//FUNCTION: 
double CPointCluster::evaluateProbability(pcl::index_t vInputPoint) const
{
	_ASSERTE(!m_FeatureSet.empty() && !m_FeatureWeightSet.empty());

	if (vInputPoint < 0 || vInputPoint > CPointCloudRetouchManager::getInstance()->getRetouchScene().getNumPoint())
		_THROW_RUNTIME_ERROR("Index is out of range");
	
	double Probability = 0;

	for (auto i = 0; i < m_FeatureSet.size(); i++)
	{
		if (m_FeatureWeightSet[i] == 0) continue;
		
		Probability += m_FeatureWeightSet[i] * m_FeatureSet[i]->evaluateFeatureMatchFactorV(vInputPoint);
	}

	_ASSERTE((Probability >= 0) && (Probability <= 1));
	return Probability;
}

//*****************************************************************
//FUNCTION: 
bool CPointCluster::init(const hiveConfig::CHiveConfig* vConfig, std::uint32_t vClusterCenter, EPointLabel vLabel, const std::vector<pcl::index_t>& vFeatureGenerationSet, const std::vector<pcl::index_t>& vValidationSet, std::uint32_t vCreationTimestamp)
{
	_ASSERTE(vConfig && !vFeatureGenerationSet.empty() && !vValidationSet.empty());
	m_pConfig = vConfig;
	m_CreationTimestamp = vCreationTimestamp;
	m_ClusterCenter = vClusterCenter;
	m_Label = vLabel;
	m_ClusterCoreRegion = vFeatureGenerationSet;  //����std::vector����

	__createFeatureObjectSet();

	for (auto e : m_FeatureSet)
	{
		m_FeatureWeightSet.emplace_back(e->generateFeatureV(vFeatureGenerationSet, vValidationSet, m_ClusterCenter));
	}

	return true;
}

//*****************************************************************
//FUNCTION: 
void CPointCluster::__createFeatureObjectSet()
{
	for (auto i = 0; i < m_pConfig->getNumSubconfig(); i++)
	{
		const hiveConfig::CHiveConfig* pConfig = m_pConfig->getSubconfigAt(i);
		if (_IS_STR_IDENTICAL(pConfig->getSubconfigType(), std::string("FEATURE")))
		{
			auto pCreateFunc = [&](const std::string& vFeatureName)
			{
				if (_IS_STR_IDENTICAL(pConfig->getName(), vFeatureName))
				{
					std::optional<std::string> PlanarFeatureSig = pConfig->getAttribute<std::string>("SIG");
					_ASSERTE(PlanarFeatureSig.has_value());
					auto pFeature = hiveDesignPattern::hiveGetOrCreateProduct<IFeature>(PlanarFeatureSig.value(), pConfig);
					_HIVE_EARLY_EXIT(!pFeature, _FORMAT_STR1("Fail to execute cluster expander due to the failure of creating [%1%].", PlanarFeatureSig.value()));
					m_FeatureSet.push_back(pFeature);
				}
			};
			
			pCreateFunc("PlanarFeature");
			pCreateFunc("VFHFeature");
			pCreateFunc("ColorFeature");
		}
	}
}

//*****************************************************************
//FUNCTION: 
bool CPointCluster::isBelongingTo(double vProbability) const
{
	const float ExpectProbability = 0.6f;
	if (vProbability >= ExpectProbability)
		return true;
	else
		return false;
}