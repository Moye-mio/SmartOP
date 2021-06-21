#include "pch.h"
#include "PointCloudRetouchInterface.h"
#include <common/HiveConfig.h>
#include "PointCluster.h"

//���������б�
//  * DeathTest_Uninitialized: δ���г�ʼ���ͽ���Probability����
//  * DeathTest_InvalidIndex: ����Probabilityʱ����Խ��
//  * FalseProbability_Test: �����Probability���Ϲ淶

const std::string ConfigPath = "Configs/QTInterfaceConfig.xml";
hiveConfig::CHiveConfig* Config;
hiveConfig::hiveParseConfig(ConfigPath, hiveConfig::EConfigType::XML, Config);

TEST(DeathTest_Uninitialized, Uninitialized)
{
	pcl::index_t TestIndex = 1;
	hiveObliquePhotography::PointCloudRetouch::CPointCluster PointCluster;
	ASSERT_ANY_THROW(double Res = PointCluster.evaluateProbability(TestIndex));
}


TEST(DeathTest_InvalidIndex, InvalidIndex)
{
	pcl::index_t TestIndex = -1;
	hiveObliquePhotography::PointCloudRetouch::CPointCluster PointCluster;
	PointCluster.init(Config, );
	ASSERT_ANY_THROW(double Res = PointCluster.evaluateProbability(TestIndex));
}


TEST(FalseProbability_Test, FalseProbability)
{
	pcl::index_t TestIndex = 1;
	hiveObliquePhotography::PointCloudRetouch::CPointCluster PointCluster;
	PointCluster.init(Config, );
	double Res = PointCluster.evaluateProbability(TestIndex);
	EXPECT_LE(Res, 1.0);
	EXPECT_GE(Res, 0.0);
}
