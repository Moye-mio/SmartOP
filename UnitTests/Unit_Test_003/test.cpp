#include "pch.h"
#include "PointCloudScene.h"

using namespace hiveObliquePhotography;

//���������б�
//  * SaveScene4PLY: �ܹ������ı���ΪPLY�����ļ�
//  * SaveScene4PLY���ܹ������ı���ΪPCD�����ļ�
//  * Save4NonexistentPath: ���泡��Ϊ�����ڵ��ļ���

const std::string g_ValidPLYFilePath = TESTMODEL_DIR + std::string("Test003_Model/slice 1.ply");
const std::string g_ValidPCDFilePath = TESTMODEL_DIR + std::string("Test003_Model/slice 1.pcd");
const std::string g_NonexistentFormatFileName = TESTMODEL_DIR + std::string("Test003_Model/slice 2.ply");

TEST(Test_SavePointCloudScene, SaveScene4PLY)
{
	
}

TEST(Test_SavePointCloudScene, SaveScene4PCD)
{
	
}

TEST(Test_SavePointCloudScene, DeathTest_Save4NonexistentFormat)
{
	
}
