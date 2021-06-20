#include "pch.h"
#include <common/UtilityInterface.h>
#include "PointCloudLoader.h"

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
	auto* pTileLoader = hiveDesignPattern::hiveGetOrCreateProduct<IPointCloudLoader>(hiveUtility::hiveGetFileSuffix(g_ValidPLYFileName));
	auto pTile = pTileLoader->loadDataFromFile(g_ValidPLYFileName);
	ASSERT_NE(pTile, nullptr);
	GTEST_ASSERT_EQ(pTile->size(), 148701);
}

TEST(Test_LoadPointCloudTile, LoadTilePcd)
{
	auto* pTileLoader = hiveDesignPattern::hiveGetOrCreateProduct<IPointCloudLoader>(hiveUtility::hiveGetFileSuffix(g_ValidPCDFileName));
	auto pTile = pTileLoader->loadDataFromFile(g_ValidPCDFileName);
	ASSERT_NE(pTile, nullptr);
	GTEST_ASSERT_EQ(pTile->size(), 148701);
}

TEST(Test_LoadPointCloudTile, DeathTest_LoadInexistentTile)
{
	auto* pTileLoader = hiveDesignPattern::hiveGetOrCreateProduct<IPointCloudLoader>(hiveUtility::hiveGetFileSuffix(g_InexistentFileName));
	auto pTile = pTileLoader->loadDataFromFile(g_InexistentFileName);

	GTEST_ASSERT_EQ(pTile, nullptr);
}
