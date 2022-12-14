#include "pch.h"
#include "PointCloudRetouchInterface.h"
#include "BoundaryDetector.h"
#include "PointCloudRetouchManager.h"
#include "PointCloudRetouchConfig.h"
#include "ObliquePhotographyDataInterface.h"

#include <fstream>
#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <pcl/io/pcd_io.h>

//测试用例列表：
// BoundaryDetectionBaseTest:给出不同场景下用户理解的空洞边界，证明空洞边界检测的有效性；
//	* Boundary_Detection_BaseTest_1:整个场景一个小空洞，验证是否能检测单一空洞；
//  * Boundary_Detection_BaseTest_2:场景有五个空洞，验证是否能检测多个空洞；
//  * Boundary_Detection_BaseTest_3:选择区域内有一个完整的空洞以及大洞的若干部分，验证是否能准确检测洞的存在及数量；

using namespace hiveObliquePhotography::PointCloudRetouch;

const auto RetouchConfigFile = TESTMODEL_DIR + std::string("Config/Test018_PointCloudRetouchConfig.xml");
const auto DetectorConfigFile = TESTMODEL_DIR + std::string("Config/Test018_BoundaryDetectorConfig.xml");
const auto DataPath = TESTMODEL_DIR + std::string("Test018_Model/");

const std::vector<std::string> ModelNames{ "one_hole", "five_holes" ,"small_big_holes"};

class TestBoundaryDetection : public testing::Test
{
protected:
	void SetUp() override
	{
		m_TestNumber++;

		m_pRetouchConfig = new CPointCloudRetouchConfig;
		ASSERT_EQ(hiveConfig::hiveParseConfig(RetouchConfigFile, hiveConfig::EConfigType::XML, m_pRetouchConfig), hiveConfig::EParseResult::SUCCEED);

		m_pCloud = std::make_shared<PointCloud_t>();
		m_pCloud = hiveObliquePhotography::hiveInitPointCloudScene({ DataPath + ModelNames[m_TestNumber] + ".ply" }).front();
		m_pManager = CPointCloudRetouchManager::getInstance();
		m_pManager->init({ m_pCloud }, m_pRetouchConfig);

		//读入所有groundtruth
		for (int i = 1; hiveUtility::hiveLocateFile(DataPath + ModelNames[m_TestNumber] + std::to_string(i) + ".txt") != ""; i++)
			m_BoundaryIndices.push_back(_loadIndices(DataPath + ModelNames[m_TestNumber] + std::to_string(i) + ".txt"));
		ASSERT_TRUE(!m_BoundaryIndices.empty());

		m_pDetectorConfig = new CPointCloudRetouchConfig;
		ASSERT_EQ(hiveConfig::hiveParseConfig(DetectorConfigFile, hiveConfig::EConfigType::XML, m_pDetectorConfig), hiveConfig::EParseResult::SUCCEED);
		m_pDetector = hiveDesignPattern::hiveGetOrCreateProduct<CBoundaryDetector>(KEYWORD::BOUNDARY_DETECTOR);
		m_pDetector->init(m_pDetectorConfig);
		ASSERT_NE(m_pDetector, nullptr);
	}

	void TearDown() override
	{
		delete m_pRetouchConfig;
		delete m_pDetectorConfig;
	}

	std::vector<int> _loadIndices(const std::string& vPath)
	{
		std::vector<int> Indices;
		std::ifstream File(vPath);
		boost::archive::text_iarchive ia(File);
		ia >> BOOST_SERIALIZATION_NVP(Indices);
		File.close();
		return Indices;
	}

	hiveConfig::CHiveConfig* m_pRetouchConfig = nullptr;
	hiveConfig::CHiveConfig* m_pDetectorConfig = nullptr;
	CPointCloudRetouchManager* m_pManager = nullptr;
	CBoundaryDetector* m_pDetector = nullptr;
	PointCloud_t::Ptr m_pCloud = nullptr;

	std::vector<std::vector<int>> m_BoundaryIndices;
	static int m_TestNumber;
private:
};
int TestBoundaryDetection::m_TestNumber = -1;

TEST_F(TestBoundaryDetection, Boundary_Detection_BaseTest_1)
{
	std::vector<pcl::index_t> BoundaryPoints = _loadIndices(DataPath+"OneHoleInput.txt");
	std::vector<std::vector<pcl::index_t>> HoleSet;
	
	m_pDetector->execute<CBoundaryDetector>(BoundaryPoints, HoleSet);

	std::vector<pcl::index_t> Result;
	for (auto& Hole : HoleSet)
		for (auto Index : Hole)
			Result.push_back(Index);

	EXPECT_EQ(HoleSet.size(), 1);
	ASSERT_TRUE(!Result.empty());

	std::set<pcl::index_t> ResultSet(Result.begin(), Result.end());
	const float ExpectCorrectRate = 80.0f, ExpectErrorRate = 20.0f;

	for (auto& Groundtruth : m_BoundaryIndices)
	{
		std::vector<pcl::index_t> Intersection;
		std::set_intersection(ResultSet.begin(), ResultSet.end(),
			Groundtruth.begin(), Groundtruth.end(), 
			std::inserter(Intersection, Intersection.begin()));
		EXPECT_GE((float)Intersection.size() / Groundtruth.size() * 100.0f, ExpectCorrectRate);

		std::vector<pcl::index_t> Difference;
		std::set_difference(ResultSet.begin(), ResultSet.end(),
			Groundtruth.begin(), Groundtruth.end(),
			std::inserter(Difference, Difference.begin()));
		EXPECT_LE((float)Difference.size() / ResultSet.size() * 100.0f, ExpectErrorRate);
	}

}

TEST_F(TestBoundaryDetection, Boundary_Detection_BaseTest_2)
{
	std::vector<pcl::index_t> BoundaryPoints = _loadIndices(DataPath + "FiveHolesInput.txt");
	std::vector<std::vector<pcl::index_t>> HoleSet;

	m_pDetector->execute<CBoundaryDetector>(BoundaryPoints, HoleSet);
	std::vector<pcl::index_t> Result;
	for (auto& Hole : HoleSet)
		for (auto Index : Hole)
			Result.push_back(Index);
	
	EXPECT_EQ(HoleSet.size(), 5);
	ASSERT_TRUE(!Result.empty());
	std::set<pcl::index_t> ResultSet(Result.begin(), Result.end());

	const float ExpectCorrectRate = 75.0f;

	for (auto& Groundtruth : m_BoundaryIndices)
	{
		std::vector<pcl::index_t> Intersection;
		std::set_intersection(ResultSet.begin(), ResultSet.end(),
			Groundtruth.begin(), Groundtruth.end(),
			std::inserter(Intersection, Intersection.begin()));
		EXPECT_GE((float)Intersection.size() / Groundtruth.size() * 100.0f, ExpectCorrectRate);
	}
}

TEST_F(TestBoundaryDetection, Boundary_Detection_BaseTest_3)
{
	std::vector<pcl::index_t> BoundaryPoints = _loadIndices(DataPath + "small_big_holes_Input.txt");
	std::vector<std::vector<pcl::index_t>> HoleSet;

	m_pDetector->execute<CBoundaryDetector>(BoundaryPoints, HoleSet);
	std::vector<pcl::index_t> Result;
	for (auto& Hole : HoleSet)
		for (auto Index : Hole)
			Result.push_back(Index);

	EXPECT_EQ(HoleSet.size(), 2);
	ASSERT_TRUE(!Result.empty());
	std::set<pcl::index_t> ResultSet(Result.begin(), Result.end());

	const float ExpectRate = 30.0f;

	for (auto& Groundtruth : m_BoundaryIndices)
	{
		std::vector<pcl::index_t> Intersection;
		std::set_intersection(ResultSet.begin(), ResultSet.end(),
			Groundtruth.begin(), Groundtruth.end(),
			std::inserter(Intersection, Intersection.begin()));
		EXPECT_GE((float)Intersection.size() / Groundtruth.size() * 100.0f, ExpectRate);
	}
}
