#include "pch.h"
#include "PointCloudScene.h"

using namespace hiveObliquePhotography;

//���������б�
//  * LoadScene: �ܹ�����������һ�ѵ����ļ�
//  * DeathTest_LoadDuplicatedFile������ĵ����ļ������ظ�
//  * DeathTest_LoadPartiallyIncorrectFileSet: ����ĵ����ļ��У��в����ļ����󣨲����ڣ���ʽ��֧��...��


TEST(Test_LoadPointCloudScene, LoadScene)
{
	const std::vector<std::string> FilePaths{
		TESTMODEL_DIR + std::string("General/slice 1.ply"), //148701
		TESTMODEL_DIR + std::string("General/slice 2.pcd"), //225016
		TESTMODEL_DIR + std::string("General/slice 3.pcd"), //227563
		TESTMODEL_DIR + std::string("General/slice 4.pcd")  //225220
	};
	auto PointCloud = CPointCloudScene::getInstance()->loadScene(FilePaths);
	auto num = CPointCloudScene::getInstance()->getNumTiles();
	GTEST_ASSERT_EQ(num, 4);
	GTEST_ASSERT_EQ(PointCloud->size(), 826500);
}

TEST(Test_LoadPointCloudScene, DeathTest_LoadDuplicatedFile1)
{
	const std::vector<std::string> FilePaths{
		TESTMODEL_DIR + std::string("General/slice 1.pcd"),
		TESTMODEL_DIR + std::string("General/slice 2.pcd"),
		TESTMODEL_DIR + std::string("General/slice 2.pcd"),
		TESTMODEL_DIR + std::string("General/slice 3.pcd")
	};
	
	auto PointCloud = CPointCloudScene::getInstance()->loadScene(FilePaths);
	
	GTEST_ASSERT_EQ(CPointCloudScene::getInstance()->getNumTiles(), 3);
	GTEST_ASSERT_EQ(PointCloud->size(), 601280);
}

TEST(Test_LoadPointCloudScene, DeathTest_LoadDuplicatedFile2)
{
	const std::vector<std::string> FilePaths{
		TESTMODEL_DIR + std::string("General/slice 1.pcd"),
		TESTMODEL_DIR + std::string("General/Slice 2.pcd"),
		TESTMODEL_DIR + std::string("General/slice 2.pcd"),
		TESTMODEL_DIR + std::string("General/slice 3.pcd")
	};

	auto PointCloud = CPointCloudScene::getInstance()->loadScene(FilePaths);

	GTEST_ASSERT_EQ(CPointCloudScene::getInstance()->getNumTiles(), 3);
	GTEST_ASSERT_EQ(PointCloud->size(), 601280);
}

TEST(Test_LoadPointCloudScene, DeathTest_LoadPartiallyIncorrectFileSet)
{
	std::vector<std::string> FilePaths{
		TESTMODEL_DIR + std::string("General/slice 1.pcd"),
		TESTMODEL_DIR + std::string("General/slice 2.pcd"),
		TESTMODEL_DIR + std::string("General/slice 3.pcd"),
		TESTMODEL_DIR + std::string("General/slice -1.pcd"),
		TESTMODEL_DIR + std::string("General/slice 1.txt"),
	};
	auto PointCloud = CPointCloudScene::getInstance()->loadScene(FilePaths);
	
	GTEST_ASSERT_EQ(CPointCloudScene::getInstance()->getNumTiles(), 3);
	GTEST_ASSERT_EQ(PointCloud->size(), 601280);
}

TEST(Test_LoadPointCloudScene, DeathTest_LoadIncorrectFileSet)
{
	std::vector<std::string> FilePaths{ "slice 1.pcd" };

	auto PointCloud = CPointCloudScene::getInstance()->loadScene(FilePaths);

	GTEST_ASSERT_EQ(CPointCloudScene::getInstance()->getNumTiles(), 0);
	GTEST_ASSERT_EQ(PointCloud->size(), 0);
}