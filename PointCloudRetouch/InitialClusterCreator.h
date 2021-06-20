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

			CPointCluster* createInitialCluster(const std::vector<pcl::index_t>& vUserMarkedRegion, double vRadius, double vHardness, EPointLabel vClusterLabel, const Eigen::Vector3f& vCameraPos, const Eigen::Matrix4d& vPvMatrix, const hiveConfig::CHiveConfig *vClusterConfig);

#ifdef _UNIT_TEST
//���԰Ѻ���Ҫ��˽�к�������wrapper��������Ϳ��Խ��е�Ԫ����
#endif

		private:
			void __divideUserSpecifiedRegion(const std::vector<pcl::index_t>& vUserMarkedRegion, const std::vector<double> vPointHardnessSet,  double vDivideThreshold, std::vector<pcl::index_t>& voFeatureGenerationSet, std::vector<pcl::index_t>& voValidationSet);
			void __generateHardness4EveryPoint(const std::vector<pcl::index_t>& vUserMarkedRegion, double vRadius, double vHardness, const Eigen::Vector3f& vCameraPos, const Eigen::Matrix4d& vPvMatrix, std::vector<double>& voPointHardnessSet);

			pcl::index_t __computeClusterCenter(const std::vector<pcl::index_t>& vUserMarkedRegion, const std::vector<double> vPointHardnessSet);

			const double m_DefaultPointSetDivideThreshold = 0.8;
		};
	}
}

