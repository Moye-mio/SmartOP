#include "pch.h"
#include "gtest/gtest.h"
#include "PointCloudAutoRetouchScene.h"
#include "PointCloudVisualizer.h"
#include "PointCloudScene.h"

using namespace hiveObliquePhotography;
using namespace Visualization;

TEST(Test_PointCloudVisualizer, TestInitAndRefresh)
{

	const std::vector<std::string> FilePaths{
		"TestModel/slice 1.pcd", 
		"TestModel/slice 2.pcd", 
		"TestModel/slice 3.pcd", 
		"TestModel/slice 4.pcd"  
	};
	
	auto pCloud = hiveObliquePhotography::CPointCloudScene::getInstance()->loadScene(FilePaths);

	//TODO: Visualizer��pcl����Ҳ��������ָ�����
	hiveObliquePhotography::AutoRetouch::CPointCloudAutoRetouchScene::getInstance()->init(pCloud.get());
	CPointCloudVisualizer::getInstance()->init(pCloud.get());
	CPointCloudVisualizer::getInstance()->refresh();
	
	system("pause");
}