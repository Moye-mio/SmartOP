#include "pch.h"
#include "PointCloudRetouchInterface.h"
#include "PointCloudRetouchScene.h"
#include "PointLabelSet.h"
#include "NeighborhoodBuilder.h"
#include "RetouchTask.h"
#include "PointCloudRetouchManager.h"
#include "PointCloudRetouchConfig.h"

#include <fstream>
#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

//��ҪΧ��CPointCloudRetouchManager::init()���в���
//ע�ⲻҪֱ��һ����ֱ�Ӳ�������������������������ʵ�֣�Ŀǰ�����ڲ�����������������Ĺ��к�����Ҫ�ֱ������Щ���к���
//���в��ԣ������ǲ���CPointCloudRetouchManager::init()

//���������б�
//  * InitPointCloudRetouchScene: �ܹ�������ʼ��PointCloudRetouchScene
//  * DeathTest_InitSceneWithErrorptr: �����ô�ָ���ʼ��Scene
// 
//  * InitPointLabelSet: �ܹ�������ʼ��PointLabelSet
//  * DeathTest_InitSceneWithNegativeSize: �����ø�����С��ʼ��LabelSet
// 
//  * CreateNeighborhoodBuilder: �ܹ��ɹ�������NeighborhoodBuilder
// 
//  * InitRetouchTask: �ܹ�������ʼ��RetouchTask
//  * DeathTest_InitRetouchTaskWithErrorConfig: �����ô�Config��ʼ��Task
// 
//  * InitPointCloudRetouchManager: �ܹ�������ʼ��Manager
//  * ResetPointCloudRetouchManager: �ܹ���������Manager
//  * ReInitPointCloudRetouchManager: �ܹ��ظ���ʼ��Manager

using namespace hiveObliquePhotography::PointCloudRetouch; 

const std::string g_CloudPath = TESTMODEL_DIR + std::string("General/slice 15.pcd");
const std::string g_BuilderSig = "";
const std::string g_ConfigPath = TESTMODEL_DIR + std::string("Config/Test010_PointCloudRetouchConfig.xml");
const std::string g_LitterSig = "LitterMarker";
const std::string g_BackgroundSig = "BackgroundMarker";
const std::string g_IndicesPath = TESTMODEL_DIR + std::string("Test010_Model/CompleteBuildingInput.txt");
const std::string g_CameraPath = TESTMODEL_DIR + std::string("Test010_Model/CompleteBuildingCameraInfo.txt");

class CTestInitPointCloudRetouch : public testing::Test
{
public:
	hiveConfig::CHiveConfig* pConfig = nullptr;
	CPointCloudRetouchManager* pManager = nullptr;

protected:
	void SetUp() override
	{
		pConfig = new CPointCloudRetouchConfig;
		if (hiveConfig::hiveParseConfig(g_ConfigPath, hiveConfig::EConfigType::XML, pConfig) != hiveConfig::EParseResult::SUCCEED)
		{
			_HIVE_OUTPUT_WARNING(_FORMAT_STR1("Failed to parse config file [%1%].", g_ConfigPath));
			return;
		}
	}

	void initTest(const std::string& vModelPath, PointCloud_t::Ptr vopCloud)
	{
		pcl::io::loadPCDFile(vModelPath, *vopCloud);
		ASSERT_GT(vopCloud->size(), 0);
		pManager = CPointCloudRetouchManager::getInstance();
		pManager->init(vopCloud, pConfig);
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

		pcl::visualization::Camera Camera;
		auto pVisualizer = new pcl::visualization::PCLVisualizer("Viewer", true);
		pVisualizer->loadCameraParameters(vCameraPath);
		pVisualizer->getCameraParameters(Camera);
		Eigen::Matrix4d ViewMatrix, ProjectionMatrix;
		Camera.computeViewMatrix(ViewMatrix);
		Camera.computeProjectionMatrix(ProjectionMatrix);
		pManager->executeMarker(Indices, ProjectionMatrix * ViewMatrix, [](auto) { return 0.8f; }, EPointLabel::UNWANTED);
	}

	void TearDown() override {}

};

TEST(Test_InitPointCloudRetouch, InitPointCloudRetouchScene)
{
	PointCloud_t::Ptr pCloud(new PointCloud_t);
	pcl::io::loadPCDFile(g_CloudPath, *pCloud);
	CPointCloudRetouchScene Scene;

	Scene.init(pCloud);
	ASSERT_EQ(Scene.getNumPoint(), 16145);
}

TEST(Test_InitPointCloudRetouch, DeathTest_InitSceneWithErrorPtr)
{
	//��ָ��
	{
		CPointCloudRetouchScene Scene;
		EXPECT_ANY_THROW(Scene.init(nullptr););
	}

	//δ�������ָ��
	{
		CPointCloudRetouchScene Scene;
		PointCloud_t::Ptr pCloud;
		EXPECT_ANY_THROW(Scene.init(pCloud));
	}
}

TEST(Test_InitPointCloudRetouch, InitPointLabelSet)
{
	CPointLabelSet LabelSet;
	const std::size_t Num = 10;

	LabelSet.init(Num);
	ASSERT_EQ(LabelSet.getSize(), Num);

	for (int i = 0; i < Num; i++)
	{
		ASSERT_EQ(LabelSet.getClusterIndexAt(i), 0);
		ASSERT_EQ(LabelSet.getLabelAt(i), EPointLabel::UNDETERMINED);
	}
}

TEST(Test_InitPointCloudRetouch, DeathTest_InitSceneWithNegativeSize)
{
	CPointLabelSet LabelSet;

	//����ҪôthrowҪô��ʼ��Ϊ��
	EXPECT_ANY_THROW(LabelSet.init(-1));
	EXPECT_NE(LabelSet.getSize(), -1);
	EXPECT_EQ(LabelSet.getSize(), 0);
}

//TEST(Test_InitPointCloudRetouch, CreateNeighborhoodBuilder)
//{
//	PointCloud_t::Ptr pCloud(new PointCloud_t);
//	pcl::io::loadPCDFile(g_CloudPath, *pCloud);
//
//	INeighborhoodBuilder* pBuilder = hiveDesignPattern::hiveCreateProduct<INeighborhoodBuilder>(g_BuilderSig, pCloud, nullptr);
//	ASSERT_NE(pBuilder, nullptr);
//	auto pTag = pBuilder->getVisitedTag();
//	ASSERT_TRUE(!pCloud->empty());
//	EXPECT_NO_THROW(pTag[pCloud->size() - 1]);
//	for (int i = 0; i < pCloud->size(); i++)
//		ASSERT_EQ(pTag[i], false);
//}

TEST(Test_InitPointCloudRetouch, InitRetouchTask)
{
	hiveConfig::CHiveConfig* pConfig = new CPointCloudRetouchConfig;
	hiveConfig::hiveParseConfig(g_ConfigPath, hiveConfig::EConfigType::XML, pConfig);

	for (auto i = 0; i < pConfig->getNumSubconfig(); i++)
	{
		const hiveConfig::CHiveConfig* pSubConfig = pConfig->getSubconfigAt(i);
		if (_IS_STR_IDENTICAL(pSubConfig->getName(), std::string(g_LitterSig)))
		{
			CRetouchTask LitterMarker;

			LitterMarker.init(pSubConfig);
			ASSERT_NE(LitterMarker.getExpander(), nullptr);
		}
		else if (_IS_STR_IDENTICAL(pSubConfig->getName(), std::string(g_BackgroundSig)))
		{
			CRetouchTask BackgroundMarker;

			BackgroundMarker.init(pSubConfig);
			ASSERT_NE(BackgroundMarker.getExpander(), nullptr);
		}
	}
}

TEST(Test_InitPointCloudRetouch, DeathTest_InitRetouchTaskWithErrorConfig)
{
	//��Config
	{
		CRetouchTask Task;

		//EXPECT_ANY_THROW(Task.init(nullptr));
		EXPECT_EQ(Task.getExpander(), nullptr);
		EXPECT_EQ(Task.getClusterConfig(), nullptr);
	}

	//��Config
	{
		const std::string OtherConfigPath = "OtherConfig";

		hiveConfig::CHiveConfig* pConfig = new CPointCloudRetouchConfig;
		hiveConfig::hiveParseConfig(OtherConfigPath, hiveConfig::EConfigType::XML, pConfig);
		CRetouchTask Task;

		EXPECT_ANY_THROW(Task.init(pConfig));
		EXPECT_EQ(Task.getExpander(), nullptr);
		EXPECT_EQ(Task.getClusterConfig(), nullptr);
	}
}

TEST(Test_InitPointCloudRetouch, InitPointCloudRetouchManager)
{
	PointCloud_t::Ptr pCloud(new PointCloud_t);
	pcl::PointSurfel t;
	pCloud->push_back(t);
	hiveConfig::CHiveConfig* pConfig = new CPointCloudRetouchConfig;
	hiveConfig::hiveParseConfig(g_ConfigPath, hiveConfig::EConfigType::XML, pConfig);
	auto pManager = CPointCloudRetouchManager::getInstance();

	pManager->init(pCloud, pConfig->findSubconfigByName("Retouch"));
	ASSERT_EQ(pManager->getClusterSet().getNumCluster(), 0);
	ASSERT_EQ(pManager->getLabelSet().getSize(), pCloud->size());
	ASSERT_NE(pManager->getLitterMarker().getExpander(), nullptr);
	ASSERT_NE(pManager->getBackgroundMarker().getExpander(), nullptr);
}

TEST_F(CTestInitPointCloudRetouch, ResetPointCloudRetouchManager)
{
	PointCloud_t::Ptr pCloud(new PointCloud_t);
	initTest(g_CloudPath, pCloud);
	expandOnce(g_IndicesPath, g_CameraPath);
	auto i = pManager->getNumCluster();
	pManager->reset4UnitTest();

	EXPECT_EQ(pManager->getConfig(), nullptr);	
	EXPECT_EQ(pManager->getOutlierConfig(), nullptr);
	EXPECT_EQ(pManager->getBackgroundMarker().getClusterConfig(), nullptr);
	EXPECT_EQ(pManager->getLitterMarker().getClusterConfig(), nullptr);
	EXPECT_EQ(pManager->getLabelSet().getSize(), 0);
	EXPECT_EQ(pManager->getRetouchScene().getNumPoint(), 0);
	EXPECT_EQ(pManager->getNumCluster(), 0);
	EXPECT_EQ(pManager->addAndGetTimestamp(), 1);
	EXPECT_EQ(pManager->getStatusQueue().size(), 0);
	EXPECT_EQ(pManager->getNeighborhoodBuilder(), nullptr);

	//����reset�Ƿ񱨴�
	EXPECT_NO_THROW(pManager->reset4UnitTest(););
	EXPECT_NO_THROW(pManager->reset4UnitTest(););
}

TEST_F(CTestInitPointCloudRetouch, ReInitPointCloudRetouchManager)
{
	PointCloud_t::Ptr pCloud(new PointCloud_t);
	initTest(g_CloudPath, pCloud);
	expandOnce(g_IndicesPath, g_CameraPath);
	initTest(g_CloudPath, pCloud);

	//initǰreset����ȫΪ��ʼ״̬
	EXPECT_EQ(pManager->addAndGetTimestamp(), 1);
	EXPECT_EQ(pManager->getStatusQueue().size(), 1);
	EXPECT_EQ(pManager->getLabelSet().getSize(), pCloud->size());
	EXPECT_EQ(pManager->getRetouchScene().getNumPoint(), pCloud->size());
	EXPECT_EQ(pManager->getConfig()->getSubconfigAt(0)->getSubconfigType(), std::string("POINT_CLOUD_RETOUCN_CONFIG"));
	EXPECT_EQ(pManager->getOutlierConfig()->getName(), std::string("Outlier"));
}