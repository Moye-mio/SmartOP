#include "pch.h"
#include "PointCloudVisualizer.h"
#include "InteractionCallback.h"
#include "PointCloudRetouchInterface.h"
#include "VisualizationConfig.h"
#include <omp.h>

#define RECORD_TIME_BEGIN clock_t StartTime, FinishTime;\
StartTime = clock();

#define RECORD_TIME_END(Name) FinishTime = clock();\
std::cout << "\n" << #Name << "花费时间: " << (int)(FinishTime - StartTime) << " ms\n";

using namespace hiveObliquePhotography::Visualization;

CPointCloudVisualizer::CPointCloudVisualizer()
{
}

CPointCloudVisualizer::~CPointCloudVisualizer()
{
	delete m_pPCLVisualizer;
	delete m_pCallback;
}

//*****************************************************************
//FUNCTION: 
void CPointCloudVisualizer::init(PointCloud_t::Ptr vPointCloud, bool vIsInQt)
{
	_ASSERTE(vPointCloud);
	m_pSceneCloud = vPointCloud;

	m_pPCLVisualizer = new pcl::visualization::PCLVisualizer("Visualizer", !vIsInQt);
	m_pCallback = new CInteractionCallback(m_pPCLVisualizer);
	m_pPCLVisualizer->setBackgroundColor(0.2, 0.2, 0.2);
	m_pPCLVisualizer->setShowFPS(false);
}

//*****************************************************************
//FUNCTION: 
void CPointCloudVisualizer::reset(PointCloud_t::Ptr vPointCloud)
{
	_ASSERTE(vPointCloud);
	m_pSceneCloud = vPointCloud;
}

//*****************************************************************
//FUNCTION: 
void CPointCloudVisualizer::refresh(const std::vector<std::size_t>& vPointLabel, bool vResetCamera)
{
	RECORD_TIME_BEGIN

	_ASSERTE(!m_pSceneCloud->empty());

	m_pPCLVisualizer->removeAllPointClouds();
	
	_ASSERTE(vPointLabel.size() == m_pSceneCloud->size());

	PointCloud_t::Ptr pCloud2Show(new PointCloud_t);
	pCloud2Show->resize(m_pSceneCloud->size());
	std::memcpy(pCloud2Show->data(), m_pSceneCloud->data(), m_pSceneCloud->size() * sizeof(PointCloud_t::PointType));
	
	#pragma omp parallel for
	for (int i = 0; i < m_pSceneCloud->size(); i++)
	{
		switch (vPointLabel[i])
		{
		case 0:
		{
			pCloud2Show->points[i].a = 0;
			break;
		}
		case 1:
		{
			unsigned char StandardBlue[4] = { 255, 0, 0, 255 };
			std::memcpy(&pCloud2Show->points[i].rgba, StandardBlue, sizeof(StandardBlue));
			break;
		}
		case 2:
		{
			unsigned char StandardRed[4] = { 0, 0, 255, 255 };	//gbr
			std::memcpy(&pCloud2Show->points[i].rgba, StandardRed, sizeof(StandardRed));
			break;
		}
		case 3:
		{
			pCloud2Show->points[i].a = 255;
			break;
		}
		case 4:
		{
			unsigned char StandardGreen[4] = { 0, 255, 0, 255 };
			std::memcpy(&pCloud2Show->points[i].rgba, StandardGreen, sizeof(StandardGreen));
			break;
		}
		}
	}

	auto PointSize = *hiveObliquePhotography::Visualization::CVisualizationConfig::getInstance()->getAttribute<int>("POINT_SHOW_SIZE");
	
	pcl::visualization::PointCloudColorHandlerRGBAField<PointCloud_t::PointType> RGBAColor(pCloud2Show);
	m_pPCLVisualizer->addPointCloud(pCloud2Show, RGBAColor, "Cloud2Show");
	m_pPCLVisualizer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, PointSize, "Cloud2Show");

	if (vResetCamera)
		m_pPCLVisualizer->resetCamera();
	else
	{
		m_pPCLVisualizer->updateCamera();
	}

	RECORD_TIME_END(显示)
}

void CPointCloudVisualizer::run()
{
	while (!m_pPCLVisualizer->wasStopped())
	{
		m_pPCLVisualizer->spinOnce(16);
	}
}