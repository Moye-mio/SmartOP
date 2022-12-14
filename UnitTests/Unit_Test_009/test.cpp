#include "pch.h"

#include "OutlierDetector.h"
#include "PointCloudRetouchConfig.h"
#include "PointCloudRetouchInterface.h"
#include "PointCloudRetouchManager.h"
#include "pcl/io/pcd_io.h"
#include <fstream>
#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

//测试用例列表：
//  * DeathTest_InvalidInput：稳定性测试，输入越界索引；
//  * FunctionTest_Test1：功能测试，离群点去除场景一；
//	* FunctionTest_Test2：功能测试，离群点去除场景二；
//	* FunctionTest_Test3：功能测试，离群点去除场景三；

using namespace  hiveObliquePhotography::PointCloudRetouch;

const std::string ConfigPath = TESTMODEL_DIR + std::string("Config/Test009_PointCloudRetouchConfig.xml");

class TestOutlierDetector : public testing::Test
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
;	}

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

	void TearDown() override
	{
		delete pConfig;
	}
};


TEST_F(TestOutlierDetector, DeathTest_InvalidInput)
{
	initTest(TESTMODEL_DIR + std::string("Test009_Model/test1.pcd"));
	pcl::Indices InputIndices;
	InputIndices.push_back(INT_MAX);
	auto pOutlierDetector = dynamic_cast<COutlierDetector*>(hiveDesignPattern::hiveGetOrCreateProduct<IPointClassifier>("OUTLIER_DETECTOR"));
	auto DevMulThresh = pManager->getOutlierConfig()->getAttribute<float>("DEV_MUL_THRESH").value();
	auto MinK = pManager->getOutlierConfig()->getAttribute<int>("MIN_K").value();
	auto PointFilterCondition = pManager->getOutlierConfig()->getAttribute<bool>("POINT_FILTER_CONDITION").value();
	EXPECT_ANY_THROW(pOutlierDetector->execute<COutlierDetector>(InputIndices, EPointLabel::UNWANTED, DevMulThresh, MinK , PointFilterCondition));
}

TEST_F(TestOutlierDetector, FunctionTest_Test1)
{ 
	initTest(TESTMODEL_DIR + std::string("Test009_Model/test1.pcd"));
	pcl::Indices InputIndices;
	for (int i = 0; i < pManager->getScene().getNumPoint(); i++)
		InputIndices.push_back(i);
	auto pOutlierDetector = hiveDesignPattern::hiveGetOrCreateProduct<COutlierDetector>(KEYWORD::OUTLIER_DETECTOR);
	auto DevMulThresh = pManager->getOutlierConfig()->getAttribute<float>("DEV_MUL_THRESH").value();
	auto MinK = pManager->getOutlierConfig()->getAttribute<int>("MIN_K").value();
	auto PointFilterCondition = pManager->getOutlierConfig()->getAttribute<bool>("POINT_FILTER_CONDITION").value();
	pOutlierDetector->execute<COutlierDetector>(InputIndices, EPointLabel::UNWANTED, DevMulThresh, MinK, PointFilterCondition );
	pcl::Indices OutlierIndices;
	pManager->dumpIndicesByLabel(OutlierIndices, EPointLabel::UNWANTED);

	pcl::Indices GroundTruth;
	loadIndices(TESTMODEL_DIR + std::string("Test009_Model/test1_indices.txt"), GroundTruth);

	pcl::Indices Interaction;
	std::set_intersection(OutlierIndices.begin(), OutlierIndices.end(),
		GroundTruth.begin(), GroundTruth.end(),
		std::inserter(Interaction, Interaction.begin()));
	
	GTEST_ASSERT_EQ(Interaction.size(), GroundTruth.size());

	double runTime;
	runTime = pOutlierDetector->getRunTime();
	std::cout << "Run time is" <<runTime<< std::endl;
}

TEST_F(TestOutlierDetector, FunctionTest_Test2)
{
	initTest(TESTMODEL_DIR + std::string("Test009_Model/test2.pcd"));
	pcl::Indices InputIndices;
	for (int i = 0; i < pManager->getScene().getNumPoint(); i++)
		InputIndices.push_back(i);
	auto pOutlierDetector = dynamic_cast<COutlierDetector*>(hiveDesignPattern::hiveGetOrCreateProduct<IPointClassifier>(KEYWORD::OUTLIER_DETECTOR));
	auto DevMulThresh = pManager->getOutlierConfig()->getAttribute<float>("DEV_MUL_THRESH").value();
	auto MinK = pManager->getOutlierConfig()->getAttribute<int>("MIN_K").value();
	auto PointFilterCondition = pManager->getOutlierConfig()->getAttribute<bool>("POINT_FILTER_CONDITION").value();
	pOutlierDetector->execute<COutlierDetector>(InputIndices, EPointLabel::UNWANTED, DevMulThresh , MinK , PointFilterCondition);

	pcl::Indices OutlierIndices;
	pManager->dumpIndicesByLabel(OutlierIndices, EPointLabel::UNWANTED);

	pcl::Indices GroundTruth;
	loadIndices(TESTMODEL_DIR + std::string("Test009_Model/test2_indices.txt"), GroundTruth);

	pcl::Indices Interaction;
	std::set_intersection(OutlierIndices.begin(), OutlierIndices.end(),
		GroundTruth.begin(), GroundTruth.end(),
		std::inserter(Interaction, Interaction.begin()));

	GTEST_ASSERT_EQ(Interaction.size(), GroundTruth.size());
}

TEST_F(TestOutlierDetector, FunctionTest_Test3)
{
	initTest(TESTMODEL_DIR + std::string("Test009_Model/test3.pcd"));
	pcl::Indices InputIndices;
	for (int i = 0; i < pManager->getScene().getNumPoint(); i++)
		InputIndices.push_back(i);
	auto pOutlierDetector = dynamic_cast<COutlierDetector*>(hiveDesignPattern::hiveGetOrCreateProduct<IPointClassifier>(KEYWORD::OUTLIER_DETECTOR));
	auto DevMulThresh = pManager->getOutlierConfig()->getAttribute<float>("DEV_MUL_THRESH").value();
	auto MinK = pManager->getOutlierConfig()->getAttribute<int>("MIN_K").value();
	auto PointFilterCondition = pManager->getOutlierConfig()->getAttribute<bool>("POINT_FILTER_CONDITION").value();
	pOutlierDetector->execute<COutlierDetector>(InputIndices, EPointLabel::UNWANTED, DevMulThresh, MinK, PointFilterCondition);

	pcl::Indices OutlierIndices;
	pManager->dumpIndicesByLabel(OutlierIndices, EPointLabel::UNWANTED);

	pcl::Indices GroundTruth;
	loadIndices(TESTMODEL_DIR + std::string("Test009_Model/test4_indices.txt"), GroundTruth);

	pcl::Indices Interaction;
	std::set_intersection(OutlierIndices.begin(), OutlierIndices.end(),
		GroundTruth.begin(), GroundTruth.end(),
		std::inserter(Interaction, Interaction.begin()));

	GTEST_ASSERT_LE(abs(int(Interaction.size() - GroundTruth.size())), GroundTruth.size() * 0.1);
}
