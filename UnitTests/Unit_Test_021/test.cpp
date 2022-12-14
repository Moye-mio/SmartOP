#include "pch.h"

#include "PointCloudRetouchManager.h"
#include "PointCloudRetouchConfig.h"
#include "PointCloudRetouchInterface.h"
#include "HoleRepairer.h"

//测试用例列表：
//  * Cube: 简单Cube模型的OBB正确性判断


using namespace hiveObliquePhotography::PointCloudRetouch;

const auto CubeModelPath = TESTMODEL_DIR + std::string("Test021_Model/Cube.pcd");
const auto CubeRotate45ModelPath = TESTMODEL_DIR + std::string("Test021_Model/CubeRotate45.ply");
//const auto CubeRotate45ModelPath = TESTMODEL_DIR + std::string("Test021_Model/Cube2.pcd");
const auto CuboidRandomModelPath = TESTMODEL_DIR + std::string("Test021_Model/CuboidRandom.ply");
const auto ConfigPath = TESTMODEL_DIR + std::string("Config/Test021_PointCloudRetouchConfig.xml");

class TestOBB : public testing::Test
{
public:
	hiveConfig::CHiveConfig* pConfig = nullptr;
	CPointCloudRetouchManager* pManager = nullptr;
	int m_CloudSize = 0;

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

	void TearDown() override
	{
		pManager->destroy();
		m_CloudSize = 0;
	}

	void initTest(const std::string& vModelPath)
	{
		std::string FileExtension = hiveUtility::hiveGetFileSuffix(vModelPath);
		PointCloud_t::Ptr pCloud(new PointCloud_t);

		if (FileExtension == "pcd")
			pcl::io::loadPCDFile(vModelPath, *pCloud);
		else if (FileExtension == "ply")
			pcl::io::loadPLYFile(vModelPath, *pCloud);
		else
			return;
		m_CloudSize = pCloud->size();
		ASSERT_GT(pCloud->size(), 0);
		pManager = CPointCloudRetouchManager::getInstance();
		pManager->init(pCloud, pConfig);
	}

	void generateCuboid(pcl::PointCloud<pcl::PointXYZ>::Ptr& vioCloud)
	{
		for (float XAxis = -5; XAxis <= 5; XAxis += 0.1)
			for (float YAxis = -4; YAxis <= 4; YAxis += 0.1)
				for (float ZAxis = -3; ZAxis <= 3; ZAxis += 0.1)
					vioCloud->push_back({XAxis, YAxis, ZAxis});
	}

};

TEST_F(TestOBB, Cube)
{
	initTest(CubeModelPath);
	std::vector<pcl::index_t> Indices;
	for (int i = 0; i < m_CloudSize; i++)
		Indices.push_back(i);
	std::tuple<Eigen::Matrix3f, Eigen::Vector3f, Eigen::Vector3f> CubeOBB;
	
	CubeOBB = pManager->calcOBBByIndices(Indices);
	for (int i = 0; i < 3; i++)
	{
		EXPECT_LT(std::get<1>(CubeOBB)[i] - (-5), 0.01);
		EXPECT_LT(std::get<2>(CubeOBB)[i] - 5, 0.01);
	}

	bool Result = false;
	Eigen::Matrix3f Axis = std::get<0>(CubeOBB);
	Eigen::Matrix3f GTaxis;
	GTaxis << 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0;
	for (int i = 0; i < 3; i++)
		for (int k = 0; k < 3; k++)
		{
			Result = Axis.row(i).isApprox(GTaxis.row(k));
			if (Result)
				break;
		}

	EXPECT_EQ(Result, true);
}

//TEST_F(TestOBB, CubeRotate45)
//{
//	initTest(CubeRotate45ModelPath);
//	std::vector<pcl::index_t> Indices;
//	for (int i = 0; i < m_CloudSize; i++)
//		Indices.push_back(i);
//
//	std::tuple<Eigen::Matrix3f, Eigen::Vector3f, Eigen::Vector3f> CubeOBB;
//	
//	CubeOBB = pManager->calcOBBByIndices(Indices);
//
//	bool Result = false;
//	Eigen::Matrix3f Axis = std::get<0>(CubeOBB);
//	Eigen::Matrix3f GTaxis;
//	Eigen::Matrix3i GToffset;
//	float AxisValue = std::sqrt(2) * 0.5;
//	GTaxis << 1.0, 0.0, 0.0, 
//		0.0, AxisValue, AxisValue, 
//		0, AxisValue, -AxisValue;
//
//	Eigen::Vector3d AxisX(1, 0, 0);
//	Eigen::Vector3d AxisY(0, 1, 0);
//	Eigen::Vector3d AxisZ(0, 0, 1);
//	Eigen::AngleAxisd RotationX(std::numbers::pi * 0.25, Eigen::Vector3d(1, 0, 0));
//
//	Eigen::Vector3f MaxValue(FLT_MIN, FLT_MIN, FLT_MIN);
//	Eigen::Vector3f MinValue(FLT_MAX, FLT_MAX, FLT_MAX);
//	for (int i = 0; i < 2; i++)
//		for (int k = 0; k < 2; k++)
//			for (int m = 0; m < 2; m++)
//			{
//				Eigen::Vector3d Point(5 * std::pow(-1, i), 5 * std::pow(-1, k), 5 * std::pow(-1, m));
//				Eigen::Vector3d PointAfterRotation = RotationX * Point;
//	
//				for (int Axis = 0; Axis < 3; Axis++)
//				{
//					MaxValue[Axis] = (MaxValue[Axis] < PointAfterRotation[Axis]) ? PointAfterRotation[Axis] : MaxValue[Axis];
//					MinValue[Axis] = (MinValue[Axis] > PointAfterRotation[Axis]) ? PointAfterRotation[Axis] : MinValue[Axis];
//				}
//			}
//
//	for (int i = 0; i < 3; i++)
//		for (int k = 0; k < 3; k++)
//		{
//			Result = Axis.col(i).isApprox(GTaxis.col(k)) || Axis.col(i).isApprox(GTaxis.col(k) * -1.0);
//			if (Result)
//			{
//				EXPECT_LT(std::get<2>(CubeOBB)[i] - MaxValue[k], 0.01);
//				EXPECT_LT(std::get<1>(CubeOBB)[i] - MinValue[k], 0.01);
//				break;
//			}
//		}
//	EXPECT_EQ(Result, true);
//}

TEST_F(TestOBB, CuboidRandom)
{
	initTest(CuboidRandomModelPath);
	std::vector<pcl::index_t> Indices;
	for (int i = 0; i < m_CloudSize; i++)
		Indices.push_back(i);

	std::tuple<Eigen::Matrix3f, Eigen::Vector3f, Eigen::Vector3f> CubeOBB;
	
	CubeOBB = pManager->calcOBBByIndices(Indices);

	bool Result = false;
	Eigen::Matrix3f Axis = std::get<0>(CubeOBB);
	Eigen::Matrix3f GTaxis;
	float AxisValue = std::sqrt(2) * 0.5;
	GTaxis << AxisValue, 0.0, -AxisValue,
			  0.5, AxisValue, 0.5,
			  0.5, -AxisValue, 0.5;
	
	Eigen::Vector3f MaxValue(5.00000, 4.00000, 3.00000);
	Eigen::Vector3f MinValue(-5.00000, -4.00000, -3.00000);

	for (int i = 0; i < 3; i++)
		for (int k = 0; k < 3; k++)
		{
			Result = Axis.row(i).dot(GTaxis.row(k)) > 0.9 || Axis.row(i).dot(GTaxis.row(k) * -1.0) > 0.9;
			if (Result)
			{
				EXPECT_LT(std::get<2>(CubeOBB)[i] - MaxValue[k], 0.01);
				EXPECT_LT(std::get<1>(CubeOBB)[i] - MinValue[k], 0.01);
				break;
			}
		}
	EXPECT_EQ(Result, true);

}

void CalcuPointsAfterRotation()
{
	Eigen::Vector3d AxisX(1, 0, 0);
	Eigen::Vector3d AxisY(0, 1, 0);
	Eigen::Vector3d AxisZ(0, 0, 1);
	Eigen::AngleAxisd RotationX(std::numbers::pi * 0.25, Eigen::Vector3d(1, 0, 0));
	Eigen::AngleAxisd RotationY(std::numbers::pi * 0.25, Eigen::Vector3d(0, 1, 0));
	auto Rotation = RotationY * RotationX;

	Eigen::Vector3f MaxValue(FLT_MIN, FLT_MIN, FLT_MIN);
	Eigen::Vector3f MinValue(FLT_MAX, FLT_MAX, FLT_MAX);
	for (int i = 0; i < 2; i++)
		for (int k = 0; k < 2; k++)
			for (int m = 0; m < 2; m++)
			{
				Eigen::Vector3d Point(5 * std::pow(-1, i), 4 * std::pow(-1, k), 3 * std::pow(-1, m));
				Eigen::Vector3d PointAfterRotation = RotationY * RotationX * Point;
				std::cout << "(" << Point[0] << ", " << Point[1] << ", " << Point[2] << "): " << PointAfterRotation[0] << ", " << PointAfterRotation[1] << ", " << PointAfterRotation[2] << std::endl;

				for (int Axis = 0; Axis < 3; Axis++)
				{
					MaxValue[Axis] = (MaxValue[Axis] < PointAfterRotation[Axis]) ? PointAfterRotation[Axis] : MaxValue[Axis];
					MinValue[Axis] = (MinValue[Axis] > PointAfterRotation[Axis]) ? PointAfterRotation[Axis] : MinValue[Axis];
				}
			}

	std::cout << MaxValue << std::endl << MinValue << std::endl;
}
