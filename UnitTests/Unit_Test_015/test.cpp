#include "pch.h"

#include <common/MathInterface.h>
#include "ColorFeature.h"
#include "PlanarityFeature.h"
#include "PointCloudRetouchInterface.h"
#include "PointCloudRetouchConfig.h"

constexpr float EPSILON = 1e-4f;
constexpr float SPACE_SIZE = 100.0f;

//���������б�
//ColorFeatureBaseTest�����n������ɫ��Ҫ��K-Means��������ɵĴ�����ɫ���ڶ�Ӧ��ɫ������Χ�ڣ�����KΪn����
//  * Color_Feature_BaseTest_1:�������һ������ɫ��
//  * Color_Feature_BaseTest_2:������ɶ�������ɫ��
//  * Color_Feature_BaseTest_3:���������������ɫ��
//  * Color_Feature_BaseTest_4:���������������ɫ��
//ColorFeatureSpecialTest�ض�����µ���������ȷ
//  * Color_Feature_SpecialTest_1: ��

//PlaneFeatureBaseTest����ƽ�棬������ɴ����ĵ��ƣ�Ҫ���Ըõ�����ϵ�ƽ����ԭƽ�����ڹ涨��Χ�ڣ�
//  * Plane_Feature_BaseTest_1: ���ģ��ƽ��20%���ŵ㣬����ƽ��н�С��10�㣻
//  * Plane_Feature_BaseTest_2: ���ģ��ƽ��50%���ŵ㣬����ƽ��н�С��30�㣻
//PlaneFeatureSpecialTest�ض�����µ���������ȷ
//  * Plane_Feature_SpecialTest_1: ��


using namespace hiveObliquePhotography::PointCloudRetouch;
const std::string ConfigPath = "PointCloudRetouchConfig.xml";


PointCloud_t::PointType generateRandomPointByPlane(const Eigen::Vector4f& vPlane, bool vOnThePlane, float vNoise = 0.0f)
{
	Eigen::Vector4f Plane = vPlane;
	Plane /= Eigen::Vector3f(Plane.x(), Plane.y(), Plane.z()).eval().norm();

	auto RandomSet = hiveMath::hiveGenerateRandomRealSet(-SPACE_SIZE, SPACE_SIZE, 3);
	Eigen::Vector4f Point(RandomSet[0], RandomSet[1], RandomSet[2], 1.0f);

	if (!vOnThePlane)
	{
		float SignedDistance = Plane.dot(Point);
		if (abs(vNoise) > EPSILON)
			SignedDistance += hiveMath::hiveGenerateRandomReal(-vNoise, vNoise);
		//�˺�Point w����ʧЧ
		Point -= SignedDistance * Plane;
	}

	PointCloud_t::PointType Output;
	for (size_t i = 0; i < 3; i++)
		Output.data[i] = Point[i];
	return Output;
}

PointCloud_t::PointType generateNoisePoint()
{
	auto RandomSet = hiveMath::hiveGenerateRandomRealSet(-SPACE_SIZE, SPACE_SIZE, 3);
	PointCloud_t::PointType Output;
	for (size_t i = 0; i < 3; i++)
		Output.data[i] = RandomSet[i];
	return Output;
}

Eigen::Vector4f generateRandomPlane()
{
	auto RandomSet = hiveMath::hiveGenerateRandomRealSet(-SPACE_SIZE, SPACE_SIZE, 4);

	return { RandomSet[0], RandomSet[1], RandomSet[2], RandomSet[3] };
}

void generateRandomColorSet(std::vector<Eigen::Vector3i>& vioColorCluster, const Eigen::Vector3i& vMainColor, int vRange, int vNum)
{
	while (vNum--)
	{
		auto TempColor = vMainColor;
		auto RandomSet = hiveMath::hiveGenerateRandomRealSet(0.0f, SPACE_SIZE, 1);
		int Offset = static_cast<int>(RandomSet[0]) % vRange;
		int Value = static_cast<int>(RandomSet[0]) % vRange;
		TempColor[Offset] -= Value;

		vioColorCluster.push_back(TempColor);
	}
}

void generateNoiseColorSet(std::vector<Eigen::Vector3i>& vioColorCluster, int vNum)
{
	while (vNum--)
	{
		auto RandomSet = hiveMath::hiveGenerateRandomRealSet(0.0f, 255.0f, 3);
		Eigen::Vector3i TempColor{ static_cast<int>(RandomSet[0]),static_cast<int>(RandomSet[1]),static_cast<int>(RandomSet[2]) };
		vioColorCluster.push_back(TempColor);
	}
}

TEST(Color_Feature_BaseTest_1, Test_1)
{
	hiveConfig::CHiveConfig* pConfig = new CPointCloudRetouchConfig;
	if (hiveConfig::hiveParseConfig(ConfigPath, hiveConfig::EConfigType::XML, pConfig) != hiveConfig::EParseResult::SUCCEED)
	{
		_HIVE_OUTPUT_WARNING(_FORMAT_STR1("Failed to parse config file [%1%].", ConfigPath));
		return;
	}
	std::vector<Eigen::Vector3i> Data;
	Eigen::Vector3i MainColor{ 255,3,3 };

	generateRandomColorSet(Data, MainColor, 3, 50);
	generateNoiseColorSet(Data, 5);

	std::vector<Eigen::Vector3i> MainColorSet;
	auto* pTileLoader = hiveDesignPattern::hiveGetOrCreateProduct<CColorFeature>(KEYWORD::COLOR_FEATURE, pConfig);
	MainColorSet = pTileLoader->adjustKMeansCluster(Data, 6);
	
	int Sum = 0;
	for (auto& Color : MainColorSet)
		if ((Color - MainColor).norm() > 8)
			Sum++;
	GTEST_ASSERT_EQ(Sum, 0);
}

TEST(Color_Feature_BaseTest_2, Test_2)
{
	hiveConfig::CHiveConfig* pConfig = new CPointCloudRetouchConfig;
	if (hiveConfig::hiveParseConfig(ConfigPath, hiveConfig::EConfigType::XML, pConfig) != hiveConfig::EParseResult::SUCCEED)
	{
		_HIVE_OUTPUT_WARNING(_FORMAT_STR1("Failed to parse config file [%1%].", ConfigPath));
		return;
	}
	std::vector<Eigen::Vector3i> Data;
	Eigen::Vector3i MainColor{ 255,3,3 };
	Eigen::Vector3i OtherColor{ 3,3,255 };
	
	generateRandomColorSet(Data, MainColor, 3, 50);
	generateRandomColorSet(Data, OtherColor, 3, 50);
	generateNoiseColorSet(Data, 8);
	
	std::vector<Eigen::Vector3i> MainColorSet;
	auto* pTileLoader = hiveDesignPattern::hiveGetOrCreateProduct<CColorFeature>(KEYWORD::COLOR_FEATURE, pConfig);
	MainColorSet = pTileLoader->adjustKMeansCluster(Data, 6);
	
	int Sum = 0;
	for (auto& Color : MainColorSet)
		if ((Color - MainColor).norm() < 8 || (Color - OtherColor).norm() < 8)
			Sum++;
	GTEST_ASSERT_GE(Sum, 2);
}

TEST(Color_Feature_BaseTest_3, Test_3)
{
	hiveConfig::CHiveConfig* pConfig = new CPointCloudRetouchConfig;
	if (hiveConfig::hiveParseConfig(ConfigPath, hiveConfig::EConfigType::XML, pConfig) != hiveConfig::EParseResult::SUCCEED)
	{
		_HIVE_OUTPUT_WARNING(_FORMAT_STR1("Failed to parse config file [%1%].", ConfigPath));
		return;
	}
	std::vector<Eigen::Vector3i> Data;
	Eigen::Vector3i MainColor{ 255,3,3 };
	Eigen::Vector3i OtherColor{ 3,3,255 };
	Eigen::Vector3i AnotherColor{ 3,255,3 };
	
	generateRandomColorSet(Data, MainColor, 3, 50);
	generateRandomColorSet(Data, OtherColor, 3, 50);
	generateRandomColorSet(Data, AnotherColor, 3, 50);
	generateNoiseColorSet(Data, 20);

	std::vector<Eigen::Vector3i> MainColorSet;
	auto* pTileLoader = hiveDesignPattern::hiveGetOrCreateProduct<CColorFeature>(KEYWORD::COLOR_FEATURE, pConfig);
	MainColorSet = pTileLoader->adjustKMeansCluster(Data, 6);

	int Sum = 0;
	for (auto& Color : MainColorSet)
		if ((Color - MainColor).norm() < 8 || (Color - OtherColor).norm() < 8 || (Color - AnotherColor).norm() < 8)
			Sum++;
	GTEST_ASSERT_GE(Sum, 3);
}

TEST(Color_Feature_BaseTest_4, Test_4)
{
	hiveConfig::CHiveConfig* pConfig = new CPointCloudRetouchConfig;
	if (hiveConfig::hiveParseConfig(ConfigPath, hiveConfig::EConfigType::XML, pConfig) != hiveConfig::EParseResult::SUCCEED)
	{
		_HIVE_OUTPUT_WARNING(_FORMAT_STR1("Failed to parse config file [%1%].", ConfigPath));
		return;
	}
	std::vector<Eigen::Vector3i> Data;
	Eigen::Vector3i MainColor{ 255,3,3 };
	Eigen::Vector3i OtherColor{ 3,3,255 };
	Eigen::Vector3i AnotherColor{ 3,255,3 };
	Eigen::Vector3i ForthColor{ 125,125,125 };

	generateRandomColorSet(Data, MainColor, 3, 50);
	generateRandomColorSet(Data, OtherColor, 3, 50);
	generateRandomColorSet(Data, AnotherColor, 3, 50);
	generateRandomColorSet(Data, ForthColor, 3, 50);
	generateNoiseColorSet(Data, 30);

	std::vector<Eigen::Vector3i> MainColorSet;
	auto* pTileLoader = hiveDesignPattern::hiveGetOrCreateProduct<CColorFeature>(KEYWORD::COLOR_FEATURE, pConfig);
	MainColorSet = pTileLoader->adjustKMeansCluster(Data, 6);

	int Sum = 0;
	for (auto& Color : MainColorSet)
		if ((Color - MainColor).norm() < 8 || (Color - OtherColor).norm() < 8 || (Color - AnotherColor).norm() < 8 || (Color - ForthColor).norm() < 8)
			Sum++;
	GTEST_ASSERT_GE(Sum, 4);
}

TEST(Plane_Feature_BaseTest_1, Test_5)
{
	hiveConfig::CHiveConfig* pConfig = new CPointCloudRetouchConfig;
	if (hiveConfig::hiveParseConfig(ConfigPath, hiveConfig::EConfigType::XML, pConfig) != hiveConfig::EParseResult::SUCCEED)
	{
		_HIVE_OUTPUT_WARNING(_FORMAT_STR1("Failed to parse config file [%1%].", ConfigPath));
		return;
	}
	
	PointCloud_t::Ptr pCloud(new PointCloud_t);
	auto Plane = generateRandomPlane();

	for (size_t k = 0; k < 100; k++)
		pCloud->push_back(generateRandomPointByPlane(Plane, true));

	constexpr float OutlierFactor = 0.01f;
	for (size_t k = 0; k < OutlierFactor * 100; k++)
		//pCloud->push_back(generateRandomPointByPlane(Plane, false));
		pCloud->push_back(generateNoisePoint());

	auto* pTileLoader = hiveDesignPattern::hiveGetOrCreateProduct<CPlanarityFeature>(KEYWORD::PLANARITY_FEATURE, pConfig);
	auto FittingPlane = pTileLoader->fitPlane(pCloud);

	Eigen::Vector3f PlaneNormal{ Plane[0],Plane[1],Plane[2]};
	PlaneNormal /= PlaneNormal.norm();
	Eigen::Vector3f FittingPlaneNormal{ FittingPlane[0],FittingPlane[1],FittingPlane[2] };
	
	GTEST_ASSERT_GE(abs(PlaneNormal.dot(FittingPlaneNormal)), 0.8);
}

TEST(Plane_Feature_BaseTest_2, Test_6)
{
	hiveConfig::CHiveConfig* pConfig = new CPointCloudRetouchConfig;
	if (hiveConfig::hiveParseConfig(ConfigPath, hiveConfig::EConfigType::XML, pConfig) != hiveConfig::EParseResult::SUCCEED)
	{
		_HIVE_OUTPUT_WARNING(_FORMAT_STR1("Failed to parse config file [%1%].", ConfigPath));
		return;
	}

	PointCloud_t::Ptr pCloud(new PointCloud_t);
	auto Plane = generateRandomPlane();

	for (size_t k = 0; k < 100; k++)
		pCloud->push_back(generateRandomPointByPlane(Plane, true));

	constexpr float OutlierFactor = 40.0f;
	for (size_t k = 0; k < OutlierFactor * 100; k++)
		//pCloud->push_back(generateRandomPointByPlane(Plane, false));
		pCloud->push_back(generateNoisePoint());

	auto* pTileLoader = hiveDesignPattern::hiveGetOrCreateProduct<CPlanarityFeature>(KEYWORD::PLANARITY_FEATURE, pConfig);
	auto FittingPlane = pTileLoader->fitPlane(pCloud);

	Eigen::Vector3f PlaneNormal{ Plane[0],Plane[1],Plane[2] };
	PlaneNormal /= PlaneNormal.norm();
	Eigen::Vector3f FittingPlaneNormal{ FittingPlane[0],FittingPlane[1],FittingPlane[2] };
	
	GTEST_ASSERT_GE(abs(PlaneNormal.dot(FittingPlaneNormal)), 0.8);
}
