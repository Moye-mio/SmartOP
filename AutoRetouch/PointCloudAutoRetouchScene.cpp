#include "pch.h"
#include "PointCloudAutoRetouchScene.h"

using namespace hiveObliquePhotography::AutoRetouch;

CPointCloudAutoRetouchScene::CPointCloudAutoRetouchScene() : m_pGlobalKdTree(new pcl::search::KdTree<pcl::PointSurfel>)
{

}

CPointCloudAutoRetouchScene::~CPointCloudAutoRetouchScene()
{

}

//*****************************************************************
//FUNCTION: 
bool CPointCloudAutoRetouchScene::undoLastOp()
{
	return m_OpResultQueue.undo();
}

//*****************************************************************
//FUNCTION: 
void CPointCloudAutoRetouchScene::recordCurrentOp(IOpResult* vResult)
{
	_ASSERTE(vResult);
	m_OpResultQueue.recordResult(vResult);
}

//*****************************************************************
//FUNCTION: 
void CPointCloudAutoRetouchScene::init(pcl::PointCloud<pcl::PointSurfel>::Ptr vPointCloudScene)
{
	_ASSERTE(vPointCloudScene);
	m_pPointCloudScene = vPointCloudScene;
	m_pGlobalKdTree->setInputCloud(m_pPointCloudScene);
	m_PointLabelSet.init(m_pPointCloudScene->size(), EPointLabel::UNDETERMINED);
}