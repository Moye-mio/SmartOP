#include "pch.h"

#include "AutoRetouchInterface.h"
#include "PointCloudAutoRetouchScene.h"

#include <fstream>
#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <pcl/io/pcd_io.h>

using namespace hiveObliquePhotography::AutoRetouch;

class TestAreaPicking : public testing::Test
{
protected:
	std::string m_SamplePaths[3][3] =
	{
		//�Ͻ��ľ���ɼ�������
		{
			"testcase/SimpleTestOne.indices.txt",
			"testcase/SimpleTestOne.camera.txt",
			"groundtruth/LeftBigTree.txt",
		},

		//��Զ�ľ���ɼ�������
		{
			"testcase/SimpleTestTwo.indices.txt",
			"testcase/SimpleTestTwo.camera.txt",
			"groundtruth/Road.txt",
		},
		
		//���ɼ��ľ���������
		{
			"testcase/SimpleTestThree.indices.txt"
			"testcase/SimpleTestThree.camera.txt"
			"groundtruth/RightFourTrees.txt",
		},
	};

	void SetUp() override
	{
		std::string ModelPath("test_tile16/Scu_Tile16.pcd");
		pcl::PointCloud<pcl::PointSurfel>::Ptr pCloud(new pcl::PointCloud<pcl::PointSurfel>);
		pcl::io::loadPCDFile(ModelPath, *pCloud);
		CPointCloudAutoRetouchScene::getInstance()->init(pCloud);
	}

	void TearDown() override
	{
	}

	static void _loadIndices(const std::string& vPath, pcl::Indices& voIndices);
	static void _loadCamera(const std::string& vPath, pcl::visualization::Camera& voCamera);
};


TEST_F(TestAreaPicking, NearestClusterWithMaxVisibility)
{
	const auto& Path = m_SamplePaths[0];
	
	pcl::IndicesPtr pTestee(new pcl::Indices);
	pcl::visualization::Camera Camera;
	pcl::IndicesPtr pGroundTruth(new pcl::Indices);

	_loadIndices(Path[0], *pTestee);
	_loadCamera(Path[1], Camera);
	_loadIndices(Path[2], *pGroundTruth);

	hiveExecuteMaxVisibilityClustering(pTestee, EPointLabel::KEPT, Camera);
	pcl::Indices Difference;
	std::set_difference(pTestee->begin(), pTestee->end(),
		pGroundTruth->begin(), pGroundTruth->end(),
		std::inserter(Difference, Difference.begin()));

	GTEST_ASSERT_EQ(Difference.size(), 0u);
}

TEST_F(TestAreaPicking, FurthestClusterWithMaxVisibility)
{
	const auto& Path = m_SamplePaths[1];

	pcl::IndicesPtr pTestee(new pcl::Indices);
	pcl::visualization::Camera Camera;
	pcl::IndicesPtr pGroundTruth(new pcl::Indices);

	_loadIndices(Path[0], *pTestee);
	_loadCamera(Path[1], Camera);
	_loadIndices(Path[2], *pGroundTruth);

	hiveExecuteMaxVisibilityClustering(pTestee, EPointLabel::KEPT, Camera);
	pcl::Indices Difference;
	std::set_difference(pTestee->begin(), pTestee->end(),
		pGroundTruth->begin(), pGroundTruth->end(),
		std::inserter(Difference, Difference.begin()));

	GTEST_ASSERT_EQ(Difference.size(), 0u);
}

TEST_F(TestAreaPicking, InvisibleClusterWithMaxArea)
{
	const auto& Path = m_SamplePaths[2];

	pcl::IndicesPtr pTestee(new pcl::Indices);
	pcl::visualization::Camera Camera;
	pcl::IndicesPtr pGroundTruth(new pcl::Indices);

	_loadIndices(Path[0], *pTestee);
	_loadCamera(Path[1], Camera);
	_loadIndices(Path[2], *pGroundTruth);

	hiveExecuteMaxVisibilityClustering(pTestee, EPointLabel::KEPT, Camera);
	pcl::Indices Difference;
	std::set_difference(pTestee->begin(), pTestee->end(),
		pGroundTruth->begin(), pGroundTruth->end(),
		std::inserter(Difference, Difference.begin()));

	GTEST_ASSERT_EQ(Difference.size(), 0u);
}

TEST_F(TestAreaPicking, DeathTest_IndicesIsNullptr)
{
	pcl::IndicesPtr pTestee = nullptr;
	pcl::visualization::Camera Camera;
	EXPECT_DEATH(hiveExecuteMaxVisibilityClustering(pTestee, EPointLabel::KEPT, Camera); , "");
}

//*****************************************************************
//FUNCTION: 
void TestAreaPicking::_loadIndices(const std::string& vPath, pcl::Indices& voIndices)
{
	const std::string Path{ vPath };
	std::ifstream File(Path);
	boost::archive::text_iarchive ia(File);
	ia >> BOOST_SERIALIZATION_NVP(voIndices);
	File.close();
}

//*****************************************************************
//FUNCTION: 
void TestAreaPicking::_loadCamera(const std::string& vPath, pcl::visualization::Camera& voCamera)
{
	auto pVisualizer = new pcl::visualization::PCLVisualizer("Viewer", true);
	pVisualizer->loadCameraParameters(vPath);
	pVisualizer->getCameraParameters(voCamera);
}
