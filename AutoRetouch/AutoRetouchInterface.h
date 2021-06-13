#pragma once
#include "AutoRetouchExport.h"
#include "PointCloudAutoRetouchScene.h"
#include "RegionGrowingAlg.h"
#include "RegionGrowingByColorAlg.h"
#include "PointClusterSet.h"
#include "CompositeClassifier.h"
#include "BinaryClassifierAlg.h"
#include "PointCluster4VFH.h"
#include "PointCluster4Score.h"
#include "PointCluster4NormalRatio.h"
#include "CompositeBinaryClassifierAlg.h"
#include "SpatialClusteringAlg.h"
#include "MaxVisibilityClusterAlg.h"

namespace hiveObliquePhotography
{
	namespace AutoRetouch
	{
		AUTORETOUCH_DECLSPEC void hiveInitPointCloudScene(pcl::PointCloud<pcl::PointSurfel>::Ptr vPointCloud);
		AUTORETOUCH_DECLSPEC void hiveUndoLastOp();

		AUTORETOUCH_DECLSPEC void hiveGetGlobalPointLabelSet(std::vector<EPointLabel>& voGlobalLabel);

		AUTORETOUCH_DECLSPEC bool hiveSwitchPointLabel(EPointLabel vTo, EPointLabel vFrom);

		AUTORETOUCH_DECLSPEC bool hiveSwitchPointLabel(const pcl::Indices& vPointIndices, EPointLabel vTo);

		AUTORETOUCH_DECLSPEC bool hiveExecuteBinaryClassifier(const std::string& vClassifierSig, const std::string& vClusterType);

		AUTORETOUCH_DECLSPEC bool hiveExecuteCompositeBinaryClassifier();

		AUTORETOUCH_DECLSPEC bool hiveExecuteClusterAlg2CreateCluster(const pcl::IndicesPtr& vioPointIndices, EPointLabel vExpectLabel, const Eigen::Vector3f& vCameraPos, const std::vector<Eigen::Matrix4d>& vMatrices);

		AUTORETOUCH_DECLSPEC bool hiveExecuteCompositeClusterAndGrowing(const pcl::IndicesPtr& vioPointIndices, EPointLabel vExpectLabel, const Eigen::Vector3f& vCameraPos, const std::vector<Eigen::Matrix4d>& vMatrices);

		AUTORETOUCH_DECLSPEC bool hiveExecuteMaxVisibilityClustering(const pcl::IndicesPtr& vioPointIndices, EPointLabel vFinalLabel, const Eigen::Vector3f& vCameraPos, const std::vector<Eigen::Matrix4d>& vMatrices);

		template<class... TArgs>
		bool hiveExecuteRegionGrowClassifier(const std::string& vClassifierSig, TArgs&&... vArgs)
		{
			IPointClassifier* pClassifier = hiveDesignPattern::hiveGetOrCreateProduct<IPointClassifier>(vClassifierSig, CPointCloudAutoRetouchScene::getInstance()->fetchPointLabelSet());
			_HIVE_EARLY_RETURN(!pClassifier, _FORMAT_STR1("Fail to execute classifier [%1%] due to unknown classifier signature.", vClassifierSig), false);

			return pClassifier->execute<CRegionGrowingByColorAlg>(true, std::forward<TArgs>(vArgs)...);
		}
	}
}
