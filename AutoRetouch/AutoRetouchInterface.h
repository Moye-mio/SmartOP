#pragma once
#include "AutoRetouchExport.h"
#include "PointCloudAutoRetouchScene.h"
#include "AutoRetouchConfig.h"
#include "PointClusterSet.h"
#include "PointCluster4VFH.h"
#include "PointCluster4Score.h"
#include "PointCluster4NormalRatio.h"
#include "RegionGrowingByColorAlg.h"
#include "BinaryClassifierAlg.h"
#include "MaxVisibilityClusterAlg.h"
#include "OutlierDetectingAlg.h"
#include "CompositeBinaryClassifierAlg.h"

namespace hiveObliquePhotography
{
	namespace AutoRetouch
	{
		AUTORETOUCH_DECLSPEC void hiveInitPointCloudScene(PointCloud_t::Ptr vPointCloud);
		AUTORETOUCH_DECLSPEC void hiveGetPointCloudForSave(PointCloud_t::Ptr voPointCloud);
		AUTORETOUCH_DECLSPEC void hiveUndoLastOp();
		AUTORETOUCH_DECLSPEC void hiveGetGlobalPointLabelSet(std::vector<EPointLabel>& voGlobalLabel);
		AUTORETOUCH_DECLSPEC void hiveGetIndicesByLabel(pcl::Indices& voPointIndices, EPointLabel vExpectLabel);
		AUTORETOUCH_DECLSPEC void hiveResetSceneSelectStatus();

		AUTORETOUCH_DECLSPEC bool hiveSwitchPointLabel(EPointLabel vTo, EPointLabel vFrom);
		AUTORETOUCH_DECLSPEC bool hiveGetAutoRetouchConfig(CAutoRetouchConfig*& voConfig);
		AUTORETOUCH_DECLSPEC bool hiveExecuteBinaryClassifier(const std::string& vClassifierSig, const std::string& vClusterType);
		AUTORETOUCH_DECLSPEC bool hiveExecuteCompositeBinaryClassifier();
		AUTORETOUCH_DECLSPEC bool hiveExecuteClusterAlg2CreateCluster(const pcl::IndicesPtr& vioPointIndices, EPointLabel vExpectLabel, const Eigen::Vector3f& vCameraPos, const Eigen::Matrix4d& vPvMatrix);
		AUTORETOUCH_DECLSPEC bool hiveExecuteCompositeClusterAndGrowing(const pcl::IndicesPtr& vioPointIndices, EPointLabel vExpectLabel, const Eigen::Vector3f& vCameraPos, const Eigen::Matrix4d& vPvMatrix);
		AUTORETOUCH_DECLSPEC bool hiveExecuteMaxVisibilityClustering(const pcl::IndicesPtr& vioPointIndices, EPointLabel vExpectLabel, const Eigen::Vector3f& vCameraPos, const Eigen::Matrix4d& vPvMatrix);
		AUTORETOUCH_DECLSPEC bool hiveExecuteRegionGrowingByColor(const pcl::Indices& vPointIndices, EPointLabel vExpectLabel);
		AUTORETOUCH_DECLSPEC bool hiveExecuteOutlierDetecting(pcl::Indices& vioPointIndices, EPointLabel vExpectLabel);

		AUTORETOUCH_DECLSPEC bool hiveInitBackgroundMarker(const hiveConfig::CHiveConfig *vBackgroundMarkerConfig);
		AUTORETOUCH_DECLSPEC bool hiveInitLitterMarker(const hiveConfig::CHiveConfig *vLitterMarkerConfig);
		AUTORETOUCH_DECLSPEC bool hiveMarkBackground(const pcl::IndicesPtr& vMarkedRegion, const Eigen::Vector3f& vCameraPos, const Eigen::Matrix4d& vPvMatrix);
		AUTORETOUCH_DECLSPEC bool hiveMarkLitter(const pcl::IndicesPtr& vMarkedRegion, const Eigen::Vector3f& vCameraPos, const Eigen::Matrix4d& vPvMatrix);
	}
}
