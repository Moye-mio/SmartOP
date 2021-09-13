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
void COutlierDetector::runV(pcl::Indices& vInputIndices, EPointLabel vTargetLabel,const float DEV_MUL_THRESH,const int MIN_K, const bool POINT_FILTER_CONDITION)  //FIXME: ʵ�ʴ�vConfig��������ֵ��������ֱ�Ӱ�������ֵ��Ϊ�����������������Ͳ�����CHiveConfig�ˣ���Ĳ�������Ҳ���ø���
{
	if (vInputIndices.empty()) return;

	auto pManager = CPointCloudRetouchManager::getInstance();
	for (auto CurrentIndex : vInputIndices)
		if (CurrentIndex < 0 || CurrentIndex >= pManager->getScene().getNumPoint())
			_THROW_RUNTIME_ERROR("Index is out of range");
	
	PointCloud_t::Ptr pCloud(new pcl::PointCloud<pcl::PointSurfel>);
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

#ifdef _UNIT_TEST
	hiveCommon::CCPUTimer Timer;
	Timer.start();
#endif // _UNIT_TEST


	PointCloud_t::Ptr pResultCloud(new pcl::PointCloud<pcl::PointSurfel>);
	pcl::StatisticalOutlierRemoval<pcl::PointSurfel> StatisticalOutlier;     //FIXME: ��include���ļ�������pcl�ṩ�˶���ȥ����Ⱥ��ķ�����Ϊʲôѡ��ǰ���֣��й�������
	StatisticalOutlier.setInputCloud(pCloud);
	StatisticalOutlier.setMeanK(MIN_K);
	StatisticalOutlier.setStddevMulThresh(DEV_MUL_THRESH);
	StatisticalOutlier.setNegative(POINT_FILTER_CONDITION);
	StatisticalOutlier.filter(*pResultCloud);

#ifdef _UNIT_TEST
	Timer.stop();
	m_RunTime = Timer.getElapsedTimeInMS();
#endif //_UNIT_TEST

	for (auto& Point : pResultCloud->points)
	   pManager->tagPointLabel(Point.curvature, vTargetLabel, 0, 0);
	
	pManager->recordCurrentStatus();
}

