#pragma once

namespace hiveObliquePhotography
{
	namespace PointCloudRetouch
	{
		class CPointCluster;

		class CInitialClusterCreator
		{
		public:
			CInitialClusterCreator() = default;
			~CInitialClusterCreator() = default;

			CPointCluster* createInitialCluster(const std::vector<pcl::index_t>& vUserMarkedRegion, float vHardness, float vRadius, EPointLabel vClusterLabel, const Eigen::Vector2f& vCenter, const Eigen::Matrix4d& vPvMatrix, const std::pair<float, float>& vWindowSize, const hiveConfig::CHiveConfig *vClusterConfig);

#ifdef _UNIT_TEST
			void testDivideUserSpecifiedRegion(const std::vector<pcl::index_t>& vUserMarkedRegion, const std::vector<float> vPointHardnessSet, float vDivideThreshold, std::vector<pcl::index_t>& voFeatureGenerationSet, std::vector<pcl::index_t>& voValidationSet) { __divideUserSpecifiedRegion(vUserMarkedRegion, vPointHardnessSet, vDivideThreshold, voFeatureGenerationSet, voValidationSet); }
			void testGenerateHardness4EveryPoint(const std::vector<pcl::index_t>& vUserMarkedRegion, float vHardness, float vRadius, const Eigen::Vector2f& vCenter, const Eigen::Matrix4f& vPvMatrix, const std::pair<float, float>& vWindowSize, std::vector<float>& voPointHardnessSet) { __generateHardness4EveryPoint(vUserMarkedRegion, vHardness, vRadius, vCenter, vPvMatrix, vWindowSize, voPointHardnessSet); }
			pcl::index_t testComputeClusterCenter(const std::vector<pcl::index_t>& vUserMarkedRegion, const std::vector<float> vPointHardnessSet, const Eigen::Vector2f& vCenter, const Eigen::Matrix4f& vPvMatrix, const std::pair<float, float>& vWindowSize) { return __computeClusterCenter(vUserMarkedRegion, vPointHardnessSet, vCenter, vPvMatrix, vWindowSize); }
			//可以把很重要的私有函数做个wrapper放在这里，就可以进行单元测试
#endif

		private:
			void __divideUserSpecifiedRegion(const std::vector<pcl::index_t>& vUserMarkedRegion, const std::vector<float> vPointHardnessSet,  float vDivideThreshold, std::vector<pcl::index_t>& voFeatureGenerationSet, std::vector<pcl::index_t>& voValidationSet);
			void __generateHardness4EveryPoint(const std::vector<pcl::index_t>& vUserMarkedRegion, float vHardness, float vRadius, const Eigen::Vector2f& vCenter, const Eigen::Matrix4d& vPvMatrix, const std::pair<float, float>& vWindowSize, std::vector<float>& voPointHardnessSet);

			pcl::index_t __computeClusterCenter(const std::vector<pcl::index_t>& vUserMarkedRegion, const std::vector<float> vPointHardnessSet, const Eigen::Vector2f& vCenter, const Eigen::Matrix4d& vPvMatrix, const std::pair<float, float>& vWindowSize);

			const float m_DefaultPointSetDivideThreshold = 0.8;
		};
	}
}

