#include "pch.h"
#include "../../ObliquePhotographyData/PointCloudPLYLoader.h"

//���������б�
//  * LoadTile: �ܹ�����������һ����ʽΪ�����ĵ����ļ�
//  * DeathTest_LoadInexistentTile����������һ�������ڵĵ����ļ�
//  * DeathTest_LoadUnsupportedFormat: ��������һ����֧�ָ�ʽ�ĵ����ļ�

const std::string g_ValidFileName;
const std::string g_InvalidFileName;

TEST(Test_LoadPointCloudTile, LoadTile) 
{
	hiveObliquePhotography::CPointCloudPLYLoader Loader;
	Loader.loadDataFromFile(g_ValidFileName);
}

TEST(Test_LoadPointCloudTile, DeathTest_LoadInexistentTile)
{
}
