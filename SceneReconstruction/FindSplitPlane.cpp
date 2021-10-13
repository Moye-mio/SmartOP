#include "pch.h"
#include "FindSplitPlane.h"
#include "PointCloudBoundingBox.hpp"

void __judgeSplitPlane(int vAxisFlag, float vMinAxisValueCloudOne, float vMaxAxisValueCloudOne, float vMinAxisValueCloudTwo, float vMaxAxisValueCloudTwo, Eigen::Vector4f& voSplitPlane);

//*****************************************************************
//FUNCTION: �ҵ��������ڵ���ģ��֮����и�ƽ�棻
Eigen::Vector4f hiveObliquePhotography::SceneReconstruction::findSplitPlane(pcl::PointCloud<pcl::PointXYZ>::Ptr vLhs, pcl::PointCloud<pcl::PointXYZ>::Ptr vRhs)
{
	auto AabbOfCloudOne = getAABB<pcl::PointXYZ>(vLhs);
	auto AabbOfCloudTwo = getAABB<pcl::PointXYZ>(vRhs);

	Eigen::Vector4f SplitPlane;
	int AxisFlag = 0;
	__judgeSplitPlane(AxisFlag, AabbOfCloudOne.first.x, AabbOfCloudOne.second.x, AabbOfCloudTwo.first.x, AabbOfCloudTwo.second.x, SplitPlane);
	AxisFlag = 1;
	__judgeSplitPlane(AxisFlag, AabbOfCloudOne.first.y, AabbOfCloudOne.second.y, AabbOfCloudTwo.first.y, AabbOfCloudTwo.second.y, SplitPlane);
	AxisFlag = 2;
	__judgeSplitPlane(AxisFlag, AabbOfCloudOne.first.z, AabbOfCloudOne.second.z, AabbOfCloudTwo.first.z, AabbOfCloudTwo.second.z, SplitPlane);

	return SplitPlane;
}

//*****************************************************************
//FUNCTION:�жϷָ�ƽ�洹ֱ���ĸ��ᣬδȷ���򷵻�ԭֵ��ȷ���򷵻��и�ƽ�棻
void __judgeSplitPlane(int vAxisFlag,float vMinAxisValueCloudOne, float vMaxAxisValueCloudOne, float vMinAxisValueCloudTwo, float vMaxAxisValueCloudTwo, Eigen::Vector4f& voSplitPlane)
{
	float HalfModelSize = 25;
	float BoundingBoxError = 2;
	if ((fabs(vMinAxisValueCloudOne - vMaxAxisValueCloudTwo) < BoundingBoxError) || (fabs(vMinAxisValueCloudTwo - vMaxAxisValueCloudOne) < BoundingBoxError))
	{
		if (fabs((vMinAxisValueCloudOne + vMaxAxisValueCloudOne) / 2 - (vMinAxisValueCloudTwo + vMaxAxisValueCloudTwo) / 2) > HalfModelSize)
		{
			float SplitPlaneValue = (vMinAxisValueCloudOne + vMaxAxisValueCloudOne + vMinAxisValueCloudTwo + vMaxAxisValueCloudTwo) / 4;

			voSplitPlane = Eigen::Vector4f(0, 0, 0, -SplitPlaneValue);
			//ASSERT vAxisFlag == 0/1/2
			voSplitPlane[vAxisFlag] = 1;
			//�涨����������
			if (std::signbit(SplitPlaneValue - vMinAxisValueCloudOne))
				voSplitPlane *= -1;
		}
	}
}
