#include "pch.h"
#include "SurfaceReconstructor.h"
#include <pcl/surface/poisson.h>
#include <pcl/io/ply_io.h>

using namespace hiveObliquePhotography::SceneReconstruction;

//*****************************************************************
//FUNCTION: 
bool ISurfaceReconstructor::onProductCreatedV(const hiveConfig::CHiveConfig* vConfig, PointCloud_t::Ptr vPointCloudScene)
{
	_ASSERTE(vConfig && vPointCloudScene);
	m_pConfig = vConfig;
	m_pSceneCloud = std::make_shared<pcl::PointCloud<pcl::PointNormal>>();
	pcl::copyPointCloud(*vPointCloudScene, *m_pSceneCloud);
	return true;
}

