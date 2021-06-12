#pragma once
#include "PointCluster.h"

namespace hiveObliquePhotography
{
	using PointCloud_t = pcl::PointCloud<pcl::PointSurfel>;
	
	namespace AutoRetouch
	{
		class CPointCluster4NormalRatio : public IPointCluster
		{
		public:
			CPointCluster4NormalRatio(const pcl::IndicesPtr& vPointIndices, EPointLabel vLabel);
			~CPointCluster4NormalRatio() override = default;

			double computeDistanceV(pcl::index_t vPointIndex) const override;

		private:
			PointCloud_t::Ptr m_pPointCloud;
		};
	}
}
