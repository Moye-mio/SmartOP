#include "pch.h"
#include "AutoRetouchInterface.h"
#include "PointCloudAutoRetouchScene.h"
#include "PointCloudVisualizer.h"
#include <pcl/io/pcd_io.h>

#include <fstream>
#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

using namespace hiveObliquePhotography::AutoRetouch;

//	���������б�
//	* RegionGrowingByColor�������������������ܷ���ȷ����;
//	* DeathTest_EmptyPoint:��������յ����ӵ㼯�ϣ�
//	* DeathTest_OverIndexIndices:��������Խ������ӵ㼯��
//	* DeathTest_AllPoints:������������������Ϊ���ӵ㣻

const std::string g_Folder = "test_tile16/";
const std::string g_CloudFile = "Scu_Tile16.pcd";
const std::string g_UnwantedTreePoints = "SomeBigTreePoints.txt";

class CTestRegionGrow :public testing::Test
{
protected:
	void SetUp() override
	{
		m_pCloud.reset(new pcl::PointCloud<pcl::PointSurfel>);
		pcl::io::loadPCDFile(g_Folder + g_CloudFile, *m_pCloud);

		hiveObliquePhotography::AutoRetouch::CPointCloudAutoRetouchScene::getInstance()->init(m_pCloud);
	}

	void TearDown() override
	{

	}

	std::vector<std::size_t> loadPointIndices(std::string vPath)
	{
		std::vector<std::size_t> Indices;
		std::ifstream File(vPath.c_str());
		boost::archive::text_iarchive ia(File);
		ia >> BOOST_SERIALIZATION_NVP(Indices);
		File.close();	//ia����ܹر�
		return Indices;
	}

	pcl::PointCloud<pcl::PointSurfel>::Ptr getCloud(){ return m_pCloud; }

private:
	pcl::PointCloud<pcl::PointSurfel>::Ptr m_pCloud = nullptr;
};


//TEST_F(CTestRegionGrow, DeathTest_EmptyPoint)
//{
//	std::vector<std::uint64_t> SeedSet = { };
//	EXPECT_DEATH(hiveObliquePhotography::AutoRetouch::hiveExecuteRegionGrowClassifier(hiveObliquePhotography::AutoRetouch::CLASSIFIER_REGION_GROW_COLOR, SeedSet, EPointLabel::UNWANTED), ".*");
//}
//
//TEST_F(CTestRegionGrow, DeathTest_OverIndexIndices)
//{
//	std::vector<std::uint64_t> SeedSet = {170000,180000 };
//	EXPECT_DEATH(hiveObliquePhotography::AutoRetouch::hiveExecuteRegionGrowClassifier(hiveObliquePhotography::AutoRetouch::CLASSIFIER_REGION_GROW_COLOR, SeedSet, EPointLabel::UNWANTED), ".*");
//
//}

TEST_F(CTestRegionGrow, RegionGrowingByColor)
{
	std::vector<std::uint64_t> SeedSet = { 18,19,20,21,22,23,24,25 };

	hiveObliquePhotography::AutoRetouch::hiveExecuteRegionGrowClassifier(hiveObliquePhotography::AutoRetouch::CLASSIFIER_REGION_GROW_COLOR, SeedSet, EPointLabel::UNWANTED);
	
	hiveObliquePhotography::Visualization::CPointCloudVisualizer::getInstance()->init(getCloud());
	hiveObliquePhotography::Visualization::CPointCloudVisualizer::getInstance()->refresh();
}










