#include "pch.h"
#include "OutlierDetector.h"
#include <pcl/filters/statistical_outlier_removal.h>
#include <pcl/filters/radius_outlier_removal.h>
#include <pcl/filters/conditional_removal.h>
#include "PointCloudRetouchManager.h"
#include "common/CpuTimer.h"

using namespace hiveObliquePhotography::PointCloudRetouch;

_REGISTER_EXCLUSIVE_PRODUCT(COutlierDetector, KEYWORD::OUTLIER_DETECTOR)

//*****************************************************************
//FUNCTION: 
void COutlierDetector::runV(pcl::Indices& vInputIndices, EPointLabel vTargetLabel, float vSearchRadius, int vMinNeighbors, bool vCondition)  //FIXME: ʵ�ʴ�vConfig��������ֵ��������ֱ�Ӱ�������ֵ��Ϊ�����������������Ͳ�����CHiveConfig�ˣ���Ĳ�������Ҳ���ø���
{
	if (vInputIndices.empty()) return;

	auto pManager = CPointCloudRetouchManager::getInstance();
	for (auto CurrentIndex : vInputIndices)
		if (CurrentIndex < 0 || CurrentIndex >= pManager->getScene().getNumPoint())
			_THROW_RUNTIME_ERROR("Index is out of range");
	
	pcl::PointCloud<pcl::PointSurfel>::Ptr pCloud(new pcl::PointCloud<pcl::PointSurfel>);
	for (auto Index : vInputIndices)  //FIXME:��������ʵ���Ѿ������������������α����ˣ���һ�λ��UNDETERMINED�ĵ���������ڶ��λ��KEPT�ĵ��������������
									  //      �����α�����������CPointCloudRetouchManager����Ƹ�������ֱ��ֻ����һ�Σ�����һ��PointCloud_t::Ptr��
	{
		pcl::PointSurfel TempPoint;
		auto Pos = CPointCloudRetouchManager::getInstance()->getScene().getPositionAt(Index);
		TempPoint.x = Pos.x();
		TempPoint.y = Pos.y();
		TempPoint.z = Pos.z();
		auto Normal = CPointCloudRetouchManager::getInstance()->getScene().getNormalAt(Index);
		TempPoint.normal_x = Normal.x();
		TempPoint.normal_y = Normal.y();
		TempPoint.normal_z = Normal.z();
		auto Color = CPointCloudRetouchManager::getInstance()->getScene().getColorAt(Index);
		TempPoint.r = Color.x();
		TempPoint.g = Color.y();
		TempPoint.b = Color.z();
		TempPoint.curvature = Index;  //FIXME��ȷ����pcl::RadiusOutlierRemoval�в���ʹ�õ����curvature��������������Σ�գ�pcl����Ժ�����õ���������ԣ�
									  //       ���д������һ�����غ����bug��PointCloud<T>���ǽ��ܵ�ģ�������Զ����ģ�������ֱ�����Զ���Ľṹ�ж���index��
									  //       ���о��ǣ�pcl::RadiusOutlierRemoval������Ҫʲô���ĵ����Բ���ִ�У�һ��Ҫ��ɫ�𣿰뾶����Ҫ����Ĳ�������009���棬
		                              //       ground truth�Ľ���������ģ���϶����õ�������ȷ�����
		pCloud->push_back(TempPoint);
	}

	pcl::PointCloud<pcl::PointSurfel>::Ptr pResultCloud(new pcl::PointCloud<pcl::PointSurfel>);
	pcl::RadiusOutlierRemoval<pcl::PointSurfel> RadiusOutlier;     //FIXME: ��include���ļ�������pcl�ṩ�˶���ȥ����Ⱥ��ķ�����Ϊʲôѡ��ǰ���֣��й�������
	RadiusOutlier.setInputCloud(pCloud);
	RadiusOutlier.setRadiusSearch(vSearchRadius);
	RadiusOutlier.setMinNeighborsInRadius(vMinNeighbors);
	RadiusOutlier.setNegative(vCondition);
	RadiusOutlier.filter(*pResultCloud);

	for (auto& Point : pResultCloud->points)
	   pManager->tagPointLabel(Point.curvature, vTargetLabel, 0, 0);
	
	pManager->recordCurrentStatus();
}

