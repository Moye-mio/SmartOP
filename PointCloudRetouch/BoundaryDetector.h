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

		};
	}
}
