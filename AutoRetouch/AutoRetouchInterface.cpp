#include "pch.h"
#include "AutoRetouchInterface.h"

using namespace hiveObliquePhotography::AutoRetouch;


//*****************************************************************
//FUNCTION: 
void hiveObliquePhotography::AutoRetouch::hiveUndoLastOp()
{
	CPointCloudAutoRetouchScene::getInstance()->undoLastOp();
}

//*****************************************************************
//FUNCTION: 
void hiveObliquePhotography::AutoRetouch::hiveInitPointCloudScene(pcl::PointCloud<pcl::PointSurfel>::Ptr vPointCloud)
{
	CPointCloudAutoRetouchScene::getInstance()->init(vPointCloud);
}

//*****************************************************************
//FUNCTION: 
void hiveObliquePhotography::AutoRetouch::hiveGetGlobalPointLabelSet(std::vector<EPointLabel>& voGlobalLabel)
{
	voGlobalLabel = CPointCloudAutoRetouchScene::getInstance()->fetchPointLabelSet()->getPointLabelSet();
}

//*****************************************************************
//FUNCTION: 
bool hiveObliquePhotography::AutoRetouch::hiveSwitchPointLabel(EPointLabel vTo, EPointLabel vFrom)
{
	_ASSERTE(CPointCloudAutoRetouchScene::getInstance() && CPointCloudAutoRetouchScene::getInstance()->fetchPointLabelSet());
	auto& PointLabelSet = CPointCloudAutoRetouchScene::getInstance()->fetchPointLabelSet()->fetchPointLabelSet();

	for (auto& PointLabel : PointLabelSet)
		if (PointLabel == vFrom)
			PointLabel = vTo;
	return true;
}

bool hiveObliquePhotography::AutoRetouch::hiveExecuteBinaryClassifier(const std::string& vClassifierSig)
{
	RECORD_TIME_BEGIN;

	_ASSERTE(vClassifierSig.find(CLASSIFIER_BINARY_VFH) != std::string::npos);

	IPointClassifier* pClassifier = hiveDesignPattern::hiveGetOrCreateProduct<IPointClassifier>(vClassifierSig, CPointCloudAutoRetouchScene::getInstance()->fetchPointLabelSet());
	_HIVE_EARLY_RETURN(!pClassifier, _FORMAT_STR1("Fail to execute classifier [%1%] due to unknown classifier signature.", vClassifierSig), false);

	pClassifier->execute<CBinaryClassifierAlg>(true);
	RECORD_TIME_END(������);
	return true;
}

bool hiveObliquePhotography::AutoRetouch::hiveExecuteClusterAlg2CreateCluster(const std::vector<int>& vPointIndices, EPointLabel vExpectLabel, const pcl::visualization::Camera& vCamera)
{
	RECORD_TIME_BEGIN;
	const std::string ClusterAlgSig = CLASSIFIER_MaxVisibilityCluster;
	IPointClassifier* pClassifier = hiveDesignPattern::hiveGetOrCreateProduct<IPointClassifier>(ClusterAlgSig, CPointCloudAutoRetouchScene::getInstance()->fetchPointLabelSet());
	_HIVE_EARLY_RETURN(!pClassifier, _FORMAT_STR1("Fail to execute classifier [%1%] due to unknown classifier signature.", ClusterAlgSig), false);

	std::vector<uint64_t> PointIndices(vPointIndices.begin(), vPointIndices.end());
	if (pClassifier->execute<CMaxVisibilityClusterAlg>(true, PointIndices, vExpectLabel, vCamera))
	{
		RECORD_TIME_END(����);

		RECORD_TIME_BEGIN;

		static std::size_t ClusterId = 0;
		CPointClusterSet::getInstance()->addPointCluster(std::to_string(ClusterId++), new CPointCluster4VFH(pClassifier->getResultIndices(), vExpectLabel));

		RECORD_TIME_END(��Ӵ�);
		return true;
	}
	else
		return false;
}

bool hiveObliquePhotography::AutoRetouch::hiveExecuteClusterAlg2RegionGrowing(const std::vector<int>& vPointIndices, EPointLabel vExpectLabel, const pcl::visualization::Camera& vCamera)
{
	const std::string ClusterAlgSig = CLASSIFIER_MaxVisibilityCluster;
	IPointClassifier* pClassifier = hiveDesignPattern::hiveGetOrCreateProduct<IPointClassifier>(ClusterAlgSig, CPointCloudAutoRetouchScene::getInstance()->fetchPointLabelSet());
	_HIVE_EARLY_RETURN(!pClassifier, _FORMAT_STR1("Fail to execute classifier [%1%] due to unknown classifier signature.", ClusterAlgSig), false);

	std::vector<uint64_t> PointIndices(vPointIndices.begin(), vPointIndices.end());
	if (pClassifier->execute<CMaxVisibilityClusterAlg>(true, PointIndices, vExpectLabel, vCamera))
	{
		hiveExecuteRegionGrowClassifier( CLASSIFIER_REGION_GROW_COLOR, PointIndices, vExpectLabel);
		return true;
	}
	else
		return false;
}