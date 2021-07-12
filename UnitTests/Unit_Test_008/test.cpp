#include "pch.h"
#include "PointCloudRetouchInterface.h"
#include "PointCloudRetouchScene.h"
#include "PointLabelSet.h"
#include "NeighborhoodBuilder.h"
#include "RetouchTask.h"
#include "PointCloudRetouchManager.h"
#include "PointCloudRetouchConfig.h"
#include <pcl/point_types.h>
#include "pcl/io/pcd_io.h"
#include "PointCluster.h"

#include <fstream>
#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

using namespace hiveObliquePhotography::PointCloudRetouch;

//���������б�
//  * LabelSet_Undo_Overview_Test:   �Ƿ�ɹ�������LabelSet�ĸ���
//  * Timestamp_Undo_Overview_Test:  �Ƿ�ɹ�������Timestamp�ĸ���
//
//  * LabelSet_Undo_Cleanup_Test:    ��LabelSet�ĳ�����Ӧ��Ӱ���´�ִ�еĽ��
//  * Timestamp_Undo_Cleanup_Test:   ��Timestamp�ĳ�����Ӧ��Ӱ���´�ִ�еĽ��
// 
//  * Empty_ResultQueue_Expect_Test: �ԿյĽ�����н��г�����Ӧ�����쳣
//  * Empty_Input_Expect_Test:       �û���ѡʱ����ǰ״̬��Ӧ�ü���������

using namespace  hiveObliquePhotography::PointCloudRetouch;

const std::string ConfigPath = TESTMODEL_DIR + std::string("Config/Test008_PointCloudRetouchConfig.xml");

class CTestUndo : public testing::Test
{
public:
	hiveConfig::CHiveConfig* pConfig = nullptr;
	CPointCloudRetouchManager* pManager = nullptr;
protected:
	void SetUp() override
	{
		pConfig = new CPointCloudRetouchConfig;
		if (hiveConfig::hiveParseConfig(ConfigPath, hiveConfig::EConfigType::XML, pConfig) != hiveConfig::EParseResult::SUCCEED)
		{
			_HIVE_OUTPUT_WARNING(_FORMAT_STR1("Failed to parse config file [%1%].", ConfigPath));
			return;
		}
	}

	void initTest(const std::string& vModelPath)
	{
		PointCloud_t::Ptr pCloud(new PointCloud_t);
		pcl::io::loadPCDFile(vModelPath, *pCloud);
		ASSERT_GT(pCloud->size(), 0);
		pManager = CPointCloudRetouchManager::getInstance();
		pManager->init(pCloud, pConfig);
	}

	void loadIndices(const std::string& vPath, pcl::Indices& voIndices)
	{
		std::ifstream File(vPath);
		boost::archive::text_iarchive ia(File);
		ia >> BOOST_SERIALIZATION_NVP(voIndices);
		File.close();
	}

	void expandOnce(const std::string& vIndicesPath, const std::string& vCameraPath)
	{
		pcl::Indices Indices;
		if (!vIndicesPath.empty())
			loadIndices(vIndicesPath, Indices);

		
		hiveMarkLitter(Indices, 0.8, 10, { 200, 300 }, {}, { 1000, 1000 });
	}
	
	void TearDown() override
	{
		delete pConfig;
	}
};

TEST_F(CTestUndo, LabelSet_Undo_Overview_Test)
{
	initTest(TESTMODEL_DIR + std::string("General/slice 16.pcd"));
	std::vector<std::size_t> LabelSetBeforeUndo, LabelSetAfterUndo;
	
	hiveDumpPointLabel(LabelSetBeforeUndo);
	expandOnce(TESTMODEL_DIR + std::string("Test008_Model/CompleteBuildingInput.txt"), ("Test008_Model/CompleteBuildingCameraInfo.txt"));
	hiveUndo();
	hiveDumpPointLabel(LabelSetAfterUndo);

	ASSERT_EQ(LabelSetBeforeUndo.size(), LabelSetAfterUndo.size());
}

TEST_F(CTestUndo, Timestamp_Undo_Overview_Test)
{
	initTest(TESTMODEL_DIR + std::string("General/slice 16.pcd"));
	
	const auto TimestampBeforeUndo = pManager->addAndGetTimestamp();
	expandOnce(TESTMODEL_DIR + std::string("Test008_Model/CompleteBuildingInput.txt"), ("Test008_Model/CompleteBuildingCameraInfo.txt"));
	hiveUndo();
	const auto TimestampAfterUndo = pManager->addAndGetTimestamp();

	ASSERT_EQ(TimestampBeforeUndo, TimestampAfterUndo);
}

TEST_F(CTestUndo, LabelSet_Undo_Cleanup_Test)
{
	initTest(TESTMODEL_DIR + std::string("General/slice 16.pcd"));
	std::vector<std::size_t> LabelSetBeforeUndo, LabelSetAfterUndo;

	expandOnce(TESTMODEL_DIR + std::string("Test008_Model/CompleteBuildingInput.txt"), ("Test008_Model/CompleteBuildingCameraInfo.txt"));
	hiveDumpPointLabel(LabelSetBeforeUndo);
	hiveUndo();
	expandOnce(TESTMODEL_DIR + std::string("Test008_Model/CompleteBuildingInput.txt"), ("Test008_Model/CompleteBuildingCameraInfo.txt"));
	hiveDumpPointLabel(LabelSetAfterUndo);

	ASSERT_EQ(LabelSetBeforeUndo.size(), LabelSetAfterUndo.size());
}

TEST_F(CTestUndo, Timestamp_Undo_Cleanup_Test)
{
	initTest(TESTMODEL_DIR + std::string("General/slice 16.pcd"));

	expandOnce(TESTMODEL_DIR + std::string("Test008_Model/CompleteBuildingInput.txt"), ("Test008_Model/CompleteBuildingCameraInfo.txt"));
	const auto TimestampBeforeUndo = pManager->addAndGetTimestamp();
	hiveUndo();
	expandOnce(TESTMODEL_DIR + std::string("Test008_Model/CompleteBuildingInput.txt"), ("Test008_Model/CompleteBuildingCameraInfo.txt"));
	const auto TimestampAfterUndo = pManager->addAndGetTimestamp();

	ASSERT_EQ(TimestampBeforeUndo, TimestampAfterUndo);
}

TEST_F(CTestUndo, Empty_ResultQueue_Expect_Test)
{
	initTest(TESTMODEL_DIR + std::string("General/slice 16.pcd"));

	EXPECT_FALSE(pManager->executeUndo());
	ASSERT_NO_FATAL_FAILURE(pManager->executeUndo());
	ASSERT_NO_THROW(pManager->executeUndo());
}

TEST_F(CTestUndo, Empty_Input_Expect_Test)
{
	initTest(TESTMODEL_DIR + std::string("General/slice 16.pcd"));

	expandOnce({}, ("Test008_Model/CompleteBuildingCameraInfo.txt"));

	EXPECT_FALSE(pManager->executeUndo());
	ASSERT_NO_FATAL_FAILURE(pManager->executeUndo());
	ASSERT_NO_THROW(pManager->executeUndo());
}