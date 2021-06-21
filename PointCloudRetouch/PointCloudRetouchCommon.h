#pragma once

namespace hiveObliquePhotography
{
	namespace PointCloudRetouch
	{
		using PointCloud_t = pcl::PointCloud<pcl::PointSurfel>;

		namespace KEYWORD
		{
			const std::string PLANARITY_FEATURE = "PLANARITY_FEATURE";
			const std::string VFH_FEATURE = "VFH_FEATURE";
			const std::string COLOR_FEATURE = "COLOR_FEATURE";

		}
		enum class EPointLabel : unsigned char
		{
			DISCARDED,
			KEPT,
			UNWANTED,
			UNDETERMINED,
			FILLED,
		};

		template <typename T>
		T NormalDistribution(T vX, T vDelta  = 1)
		{
			return exp(-(vX * vX) / (2 * vDelta * vDelta)) / (2.50662827464 * vDelta);
		}
	}
}