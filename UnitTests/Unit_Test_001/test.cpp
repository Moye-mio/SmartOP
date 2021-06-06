#include "pch.h"
#include <common/UtilityInterface.h>
#include "PointCloudLoader.h"
#include "PointCloudPLYLoader.h"

using namespace hiveObliquePhotography;

//���������б�
//  * LoadTile: �ܹ�����������һ����ʽΪ�����ĵ����ļ�
//  * DeathTest_LoadInexistentTile����������һ�������ڵĵ����ļ�
//  * DeathTest_LoadUnsupportedFormat: ��������һ����֧�ָ�ʽ�ĵ����ļ�

const std::string g_ValidPLYFileName = "TestModel/slice 1.ply";
const std::string g_ValidPCDFileName = "TestModel/slice 1.pcd";
const std::string g_InexistentFileName = "TestModel/slice 5.pcd";
const std::string g_UnsupportedFileName = "TestModel/slice 1.txt";

TEST(Test_LoadPointCloudTile, LoadTilePly)
{
	CPointCloudPLYLoader Temp;
	
	auto* pTileLoader = hiveDesignPattern::hiveGetOrCreateProduct<IPointCloudLoader>(hiveUtility::hiveGetFileSuffix(g_ValidPLYFileName));
	pcl::PointCloud<pcl::PointSurfel>* pTile = pTileLoader->loadDataFromFile(g_ValidPLYFileName);
	GTEST_ASSERT_EQ(pTile->size(), 148701);

}

TEST(Test_LoadPointCloudTile, LoadTilePcd)
{
	auto* pTileLoader = hiveDesignPattern::hiveGetOrCreateProduct<IPointCloudLoader>(hiveUtility::hiveGetFileSuffix(g_ValidPCDFileName));
	pcl::PointCloud<pcl::PointSurfel>* pTile = pTileLoader->loadDataFromFile(g_ValidPCDFileName);
	GTEST_ASSERT_EQ(pTile->size(), 148701);


}

TEST(Test_LoadPointCloudTile, DeathTest_LoadInexistentTile)
{
	auto* pTileLoader = hiveDesignPattern::hiveGetOrCreateProduct<IPointCloudLoader>(hiveUtility::hiveGetFileSuffix(g_InexistentFileName));
	pcl::PointCloud<pcl::PointSurfel>* pTile = pTileLoader->loadDataFromFile(g_InexistentFileName);

	GTEST_ASSERT_EQ(pTile, nullptr);
}

TEST(Test_LoadPointCloudTile, DeathTest_LoadUnsupportedFormat)
{
	auto* pTileLoader = hiveDesignPattern::hiveGetOrCreateProduct<IPointCloudLoader>(hiveUtility::hiveGetFileSuffix(g_UnsupportedFileName));
	pcl::PointCloud<pcl::PointSurfel>* pTile = pTileLoader->loadDataFromFile(g_UnsupportedFileName);

	GTEST_ASSERT_EQ(pTile, nullptr);

}