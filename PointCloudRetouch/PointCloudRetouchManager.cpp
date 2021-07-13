#include "pch.h"
#include "PointCloudRetouchManager.h"
#include "PointCluster.h"
#include "NeighborhoodBuilder.h"
#include "OutlierDetector.h"

using namespace hiveObliquePhotography::PointCloudRetouch;

//*****************************************************************
//FUNCTION: 
bool CPointCloudRetouchManager::init(PointCloud_t::Ptr vPointCloud, const hiveConfig::CHiveConfig* vConfig)
{
	_ASSERTE(vPointCloud && vConfig);

	if (m_StatusQueue.size())
		m_StatusQueue.clear();
	m_Scene.init(vPointCloud);
	m_PointLabelSet.init(m_Scene.getNumPoint());
	m_pConfig = vConfig;

	if (_IS_STR_IDENTICAL(vConfig->getSubconfigAt(0)->getSubconfigType(), std::string("POINT_CLOUD_RETOUCN_CONFIG")))
	{
		_ASSERTE(vConfig->getNumSubconfig());
		if (_IS_STR_IDENTICAL(vConfig->getSubconfigAt(0)->getName(), std::string("Retouch")))
			vConfig = vConfig->getSubconfigAt(0);
	}

	auto num = vConfig->getNumSubconfig();

	for (auto i = 0; i < vConfig->getNumSubconfig(); i++)
	{
		const hiveConfig::CHiveConfig* pConfig = vConfig->getSubconfigAt(i);
		if (_IS_STR_IDENTICAL(pConfig->getSubconfigType(), std::string("TASK")))
		{
			if (_IS_STR_IDENTICAL(pConfig->getName(), std::string("LitterMarker")))
			{
				m_LitterMarker.init(pConfig);
			}
			else
			{
				if (_IS_STR_IDENTICAL(pConfig->getName(), std::string("BackgroundMarker")))
				{
					m_BackgroundMarker.init(pConfig);
				}
				else
				{
					_HIVE_EARLY_RETURN(true, _FORMAT_STR1("Unexpeced subconfiguration [%1%].", pConfig->getName()), false);
				}
			}
			continue;
		}
		if (_IS_STR_IDENTICAL(pConfig->getSubconfigType(), std::string("NEIGHBOR_BUILDER")))
		{
			std::optional<std::string> NeighborhoodBuilderSig = pConfig->getAttribute<std::string>("SIG");
			_ASSERTE(NeighborhoodBuilderSig.has_value());
			m_pNeighborhoodBuilder = hiveDesignPattern::hiveCreateProduct<INeighborhoodBuilder>(NeighborhoodBuilderSig.value(), pConfig, vPointCloud, &m_PointLabelSet);
			_HIVE_EARLY_RETURN(!m_pNeighborhoodBuilder, _FORMAT_STR1("Fail to initialize retouch due to the failure of creating  neighborhood builder [%1%].", NeighborhoodBuilderSig.value()), false);
			continue;
		}
		if (_IS_STR_IDENTICAL(pConfig->getSubconfigType(), std::string("OUTLIER")))
		{
			if (_IS_STR_IDENTICAL(pConfig->getName(), std::string("Outlier")))
			{
				m_pOutlierConfig = pConfig;
			}
			continue;
		}
		_HIVE_OUTPUT_WARNING(_FORMAT_STR1("Unknown subconfiguration type [%1%].", pConfig->getSubconfigType()));
	}
	recordCurrentStatus();
	return true;
}

//*****************************************************************
//FUNCTION: 
void CPointCloudRetouchManager::clearMark()
{
	m_PointLabelSet.reset();
	m_pNeighborhoodBuilder->reset();
	m_PointClusterSet.reset();
}

//*****************************************************************
//FUNCTION: 
CPointCluster* CPointCloudRetouchManager::__generateInitialCluster(const std::vector<pcl::index_t>& vUserMarkedRegion, const Eigen::Matrix4d& vPvMatrix, double vHardness, EPointLabel vTargetLabel)
{
	_ASSERTE(m_pConfig);
	_ASSERTE((vTargetLabel == EPointLabel::KEPT) || (vTargetLabel == EPointLabel::UNWANTED));

	const hiveConfig::CHiveConfig* pClusterConfig = (vTargetLabel == EPointLabel::KEPT) ? m_BackgroundMarker.getClusterConfig() : m_LitterMarker.getClusterConfig();
	_ASSERTE(pClusterConfig);

	return m_InitialClusterCreator.createInitialCluster(vUserMarkedRegion, vPvMatrix, vHardness, vTargetLabel, pClusterConfig);
}

//*****************************************************************
//FUNCTION: 
bool CPointCloudRetouchManager::dumpPointLabel(std::vector<std::size_t>& voPointLabel) const
{
	auto NumPoints = m_Scene.getNumPoint();
	if (NumPoints > 0)
	{
		voPointLabel.clear();

		for (int i = 0; i < NumPoints; i++)
		{
			auto Label = m_PointLabelSet.getLabelAt(i);
			voPointLabel.push_back(static_cast<std::size_t>(Label));
		}
		
		return true;
	}
	else
		return false;
}

//*****************************************************************
//FUNCTION: 
bool CPointCloudRetouchManager::executePreprocessor(std::vector<pcl::index_t>& vioPointSet, const Eigen::Matrix4d& vPvMatrix, const std::function<double(Eigen::Vector2d)>& vSignedDistanceFunc, const Eigen::Vector3d& vViewPos)
{
	//TODO: ����
	try
	{
		m_Preprocessor.cullBySdf(vioPointSet, vPvMatrix, vSignedDistanceFunc);
		m_Preprocessor.cullByDepth(vioPointSet, vPvMatrix, vViewPos);
		return true;
	}
	catch (std::runtime_error&)
	{

	}
	catch (...)
	{

	}
	return false;
}

//*****************************************************************
//FUNCTION: 
bool CPointCloudRetouchManager::executeMarker(const std::vector<pcl::index_t>& vUserMarkedRegion, const Eigen::Matrix4d& vPvMatrix, double vHardness, EPointLabel vTargetLabel)
{
	_ASSERTE((vTargetLabel == EPointLabel::UNWANTED) || (vTargetLabel == EPointLabel::KEPT));

	try
	{
		CPointCluster* pInitCluster = __generateInitialCluster(vUserMarkedRegion, vPvMatrix, vHardness, vTargetLabel);
		_ASSERTE(pInitCluster);
		m_PointLabelSet.tagCoreRegion4Cluster(pInitCluster->getCoreRegion(), vTargetLabel, pInitCluster->getClusterIndex());

		if (vTargetLabel == EPointLabel::UNWANTED)
		{
			m_LitterMarker.execute(pInitCluster);
		}
		else
		{
			m_BackgroundMarker.execute(pInitCluster);
		}
		return true;
	}
	catch (std::runtime_error&)
	{
		_HIVE_OUTPUT_WARNING(_FORMAT_STR1("Fail to execute marker because of the following error: [%1%].", e.what()));
	}
	catch (...)
	{
		_HIVE_OUTPUT_WARNING("Fail to execute marker due to unexpected error");
	}
//TODO: ��ִ�й����г����쳣ʱ����Ҫ��״̬����Ϊ����ִ��֮ǰ����Ҫ��������������֤��
	return false;
}

//*****************************************************************
//FUNCTION: 
void CPointCloudRetouchManager::switchLabel(EPointLabel vTo, EPointLabel vFrom)
{
	auto NumPoints = m_Scene.getNumPoint();
	_ASSERTE(NumPoints > 0);

	for (int i = 0; i < NumPoints; i++)
	{
		if (m_PointLabelSet.getLabelAt(i) == vFrom)
			m_PointLabelSet.tagPointLabel(i, vTo, m_PointLabelSet.getClusterIndexAt(i), m_PointLabelSet.getClusterBelongingProbabilityAt(i));
	}
}

//*****************************************************************
//FUNCTION: 
void CPointCloudRetouchManager::setLabel(const std::vector<pcl::index_t>& vPoints, EPointLabel vTarget)
{
	auto NumPoints = m_Scene.getNumPoint();
	_ASSERTE(NumPoints > 0);

	for (auto Index : vPoints)
	{
		auto Label = m_PointLabelSet.getLabelAt(Index);
		if (Label == EPointLabel::KEPT || Label == EPointLabel::UNWANTED)
			m_PointLabelSet.tagPointLabel(Index, vTarget, 0, 0);
	}
}

//*****************************************************************
//FUNCTION: 
std::vector<pcl::index_t> CPointCloudRetouchManager::buildNeighborhood(pcl::index_t vSeed, std::uint32_t vSeedClusterIndex)
{
	//����NRVO
	return m_pNeighborhoodBuilder->buildNeighborhood(vSeed, vSeedClusterIndex);
}

//*****************************************************************
//FUNCTION: 
void CPointCloudRetouchManager::dumpIndicesByLabel(std::vector<pcl::index_t>& vioIndices, EPointLabel vLabel)
{
	for (size_t i = 0; i < m_PointLabelSet.getSize(); i++)
		if (m_PointLabelSet.getLabelAt(i) == vLabel)
				vioIndices.push_back(i);
}

//*****************************************************************
//FUNCTION: 
bool CPointCloudRetouchManager::executeOutlierDetector(EPointLabel vTo)
{
	std::vector<pcl::index_t> Indices;
	dumpIndicesByLabel(Indices, EPointLabel::UNDETERMINED);
	dumpIndicesByLabel(Indices, EPointLabel::KEPT);
	auto pOutlierDetector = dynamic_cast<COutlierDetector*>(hiveDesignPattern::hiveCreateProduct<IPointClassifier>("OUTLIER_DETECTOR"));
	return pOutlierDetector->execute<COutlierDetector>(Indices, vTo, m_pOutlierConfig);
}

//*****************************************************************
//FUNCTION: 
bool CPointCloudRetouchManager::undo()
{
	if (m_StatusQueue.size() > 1)
		m_StatusQueue.pop_back();
	else
		return false;
	auto LastStatus = m_StatusQueue.back();
	m_PointLabelSet = LastStatus.first;
	m_Timestamp = LastStatus.second;
	return true;
}

//*****************************************************************
//FUNCTION: 
void CPointCloudRetouchManager::recordCurrentStatus()
{
	m_StatusQueue.push_back(std::make_pair(m_PointLabelSet, m_Timestamp));
	if (m_StatusQueue.size() > 10)
		m_StatusQueue.pop_front();
}