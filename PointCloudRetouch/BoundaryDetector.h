#pragma once
#include "PointClassifier.h"

namespace hiveObliquePhotography
{
	namespace PointCloudRetouch
	{
		class CBoundaryDetector : public IPointClassifier
		{
		public:
			CBoundaryDetector() = default;
			~CBoundaryDetector() = default;

			virtual void runV(std::vector<pcl::index_t>& vioBoundarySet, const hiveConfig::CHiveConfig* vConfig);	//vio: �ǿմ���ָ����ѡ�㼯���մ���Ϊ��������

		private:
			Eigen::Vector3f __calcProjectivePoint(Eigen::Vector3f& vCenterPosition, Eigen::Vector3f& vCenterNormal, Eigen::Vector3f& vProjectPosition);
			float __calcAngle(Eigen::Vector3f& vStandardVector, Eigen::Vector3f& vOtherVector, Eigen::Vector3f& vCenterNormal);

		};
	}
}
