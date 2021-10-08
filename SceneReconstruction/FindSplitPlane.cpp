#include "FindSplitPlane.h"
#include <pcl/features/moment_of_inertia_estimation.h>

using namespace hiveObliquePhotography::SceneReconstruction;

//*****************************************************************
//FUNCTION: �ҵ��������ڵ���ģ��֮����и�ƽ�棻
Eigen::Vector4f CFindSplitPlane::execute(pcl::PointCloud<pcl::PointXYZ>::Ptr vCloudOne, pcl::PointCloud<pcl::PointXYZ>::Ptr vCloudTwo)
{
	pcl::PointXYZ MinPointOfCloudOne;
	pcl::PointXYZ MaxPointOfCloudOne;
	getMinAndMaxPointOfAABB(vCloudOne, MinPointOfCloudOne, MaxPointOfCloudOne);
	pcl::PointXYZ MinPointOfCloudTwo;
	pcl::PointXYZ MaxPointOfCloudTwo;
	getMinAndMaxPointOfAABB(vCloudTwo, MinPointOfCloudTwo, MaxPointOfCloudTwo);

	float HalfModelSize = 25;
	float BoundingBoxError = 2;
	if ((fabs(MinPointOfCloudOne.x - MaxPointOfCloudTwo.x) < BoundingBoxError) || (fabs(MinPointOfCloudTwo.x - MaxPointOfCloudOne.x) < BoundingBoxError))
	{
		if (fabs((MinPointOfCloudOne.x + MaxPointOfCloudOne.x) / 2 - (MinPointOfCloudTwo.x + MaxPointOfCloudTwo.x) / 2) > HalfModelSize)
		{
			//д����ֵ
			return Eigen::Vector4f();
		}
	}

	if ((fabs(MinPointOfCloudOne.y - MaxPointOfCloudTwo.y) < BoundingBoxError) || (fabs(MinPointOfCloudTwo.y - MaxPointOfCloudOne.y) < BoundingBoxError))
	{
		if (fabs((MinPointOfCloudOne.y + MaxPointOfCloudOne.y) / 2 - (MinPointOfCloudTwo.y + MaxPointOfCloudTwo.y) / 2) > HalfModelSize)
		{
			//д����ֵ
			return Eigen::Vector4f();
		}
	}

	if ((fabs(MinPointOfCloudOne.z - MaxPointOfCloudTwo.z) < BoundingBoxError) || (fabs(MinPointOfCloudTwo.z - MaxPointOfCloudOne.z) < BoundingBoxError))
	{
		if (fabs((MinPointOfCloudOne.z + MaxPointOfCloudOne.z) / 2 - (MinPointOfCloudTwo.z + MaxPointOfCloudTwo.z) / 2) > HalfModelSize)
		{
			//д����ֵ
			return Eigen::Vector4f();
		}
	}

	return;
}

//*****************************************************************
//FUNCTION:�õ�����ģ�͵�AABB��Χ�е�������ɵ㣻
void CFindSplitPlane::getMinAndMaxPointOfAABB(pcl::PointCloud<pcl::PointXYZ>::Ptr vCloud,pcl::PointXYZ& vMinPoint, pcl::PointXYZ& vMaxPoint)
{
	pcl::MomentOfInertiaEstimation <pcl::PointXYZ> FeatureExtractor;
	FeatureExtractor.setInputCloud(vCloud);
	FeatureExtractor.compute();
	FeatureExtractor.getAABB(vMinPoint, vMaxPoint);
	return;
}
