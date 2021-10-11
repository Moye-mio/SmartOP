#include "pch.h"
#include "FindSplitPlane.h"
#include <pcl/features/moment_of_inertia_estimation.h>

using namespace hiveObliquePhotography::SceneReconstruction;

//*****************************************************************
//FUNCTION: �ҵ��������ڵ���ģ��֮����и�ƽ�棻
Eigen::Vector4f CFindSplitPlane::execute(pcl::PointCloud<pcl::PointXYZ>::Ptr vCloudOne, pcl::PointCloud<pcl::PointXYZ>::Ptr vCloudTwo)
{
	pcl::PointXYZ MinPointOfCloudOne;
	pcl::PointXYZ MaxPointOfCloudOne;
	__getMinAndMaxPointOfAABB(vCloudOne, MinPointOfCloudOne, MaxPointOfCloudOne);
	pcl::PointXYZ MinPointOfCloudTwo;
	pcl::PointXYZ MaxPointOfCloudTwo;
	__getMinAndMaxPointOfAABB(vCloudTwo, MinPointOfCloudTwo, MaxPointOfCloudTwo);

	Eigen::Vector4f voSplitPlane;
	int AxisFlag = 0;
	__judgeSplitPlane(AxisFlag, MinPointOfCloudOne.x, MaxPointOfCloudOne.x, MinPointOfCloudTwo.x, MaxPointOfCloudTwo.x, voSplitPlane);
	AxisFlag = 1;
	__judgeSplitPlane(AxisFlag, MinPointOfCloudOne.y, MaxPointOfCloudOne.y, MinPointOfCloudTwo.y, MaxPointOfCloudTwo.y, voSplitPlane);
	AxisFlag = 2;
	__judgeSplitPlane(AxisFlag, MinPointOfCloudOne.z, MaxPointOfCloudOne.z, MinPointOfCloudTwo.z, MaxPointOfCloudTwo.z, voSplitPlane);

	return voSplitPlane;
}

//*****************************************************************
//FUNCTION:�õ�����ģ�͵�AABB��Χ�е�������ɵ㣻
void CFindSplitPlane::__getMinAndMaxPointOfAABB(pcl::PointCloud<pcl::PointXYZ>::Ptr vCloud, pcl::PointXYZ& voMinPoint, pcl::PointXYZ& voMaxPoint)
{
	pcl::MomentOfInertiaEstimation<pcl::PointXYZ> FeatureExtractor;
	FeatureExtractor.setInputCloud(vCloud);
	FeatureExtractor.compute();
	FeatureExtractor.getAABB(voMinPoint, voMaxPoint);
	return;
}

//*****************************************************************
//FUNCTION:�жϷָ�ƽ�洹ֱ���ĸ��ᣬδȷ���򷵻�ԭֵ��ȷ���򷵻��и�ƽ�棻
void CFindSplitPlane::__judgeSplitPlane(int vAxisFlag,float vMinAxisValueCloudOne, float vMaxAxisValueCloudOne, float vMinAxisValueCloudTwo, float vMaxAxisValueCloudTwo, Eigen::Vector4f& vioSplitPlane)
{
	float HalfModelSize = 25;
	float BoundingBoxError = 2;
	if ((fabs(vMinAxisValueCloudOne - vMaxAxisValueCloudTwo) < BoundingBoxError) || (fabs(vMinAxisValueCloudTwo - vMaxAxisValueCloudOne) < BoundingBoxError))
	{
		if (fabs((vMinAxisValueCloudOne + vMaxAxisValueCloudOne) / 2 - (vMinAxisValueCloudTwo + vMaxAxisValueCloudTwo) / 2) > HalfModelSize)
		{
			float SplitPlaneValue = (vMinAxisValueCloudOne + vMaxAxisValueCloudOne + vMinAxisValueCloudTwo + vMaxAxisValueCloudTwo) / 4;
			//�涨����������;
			float Sign = ((SplitPlaneValue - vMinAxisValueCloudOne) > 0) ? 1 : -1;
			if (vAxisFlag == 0)
				vioSplitPlane = Eigen::Vector4f(1 * Sign, 0, 0, -SplitPlaneValue * Sign);
			if (vAxisFlag == 1)
				vioSplitPlane = Eigen::Vector4f(0, 1 * Sign, 0, -SplitPlaneValue * Sign);
			if (vAxisFlag == 2)
				vioSplitPlane = Eigen::Vector4f(0, 0, 1 * Sign, -SplitPlaneValue * Sign);
		}
	}
}
