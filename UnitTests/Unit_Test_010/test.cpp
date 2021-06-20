#include "pch.h"
#include "PointCloudRetouchInterface.h"
#include "PointCloudRetouchScene.h"

//��ҪΧ��CPointCloudRetouchManager::init()���в���
//ע�ⲻҪֱ��һ����ֱ�Ӳ�������������������������ʵ�֣�Ŀǰ�����ڲ�����������������Ĺ��к�����Ҫ�ֱ������Щ���к���
//���в��ԣ������ǲ���CPointCloudRetouchManager::init()

//���������б�
//  * InitPointCloudRetouchScene: �ܹ�������ʼ��PointCloudRetouchScene
//  * DeathTest_InitSceneWithErrorptr: �����ô�ָ���ʼ��Scene
//  * DeathTest_InitSceneWithErrorPtr: ������δ��ʼ������ָ���ʼ��Scene
//  * InitPointLabelSet: �ܹ�������ʼ��PointLabelSet
//  * 
//  * DeathTest_LoadInexistentTile����������һ�������ڵĵ����ļ�
//  * DeathTest_LoadUnsupportedFormat: ��������һ����֧�ָ�ʽ�ĵ����ļ�

using namespace hiveObliquePhotography::PointCloudRetouch;

const std::string g_CloudPath = "Panda.pcd";

TEST(Test_InitPointCloudRetouch, InitPointCloudRetouchScene)
{
	PointCloud_t::Ptr pCloud(new PointCloud_t);
	pcl::io::loadPCDFile(g_CloudPath, *pCloud);

	CPointCloudRetouchScene Scene;
	Scene.init(pCloud);

	ASSERT_EQ(Scene.getNumPoint(), 16145);
	ASSERT_EQ(Scene.getPointCloudScene(), pCloud);
	ASSERT_EQ(Scene.getGlobalKdTree()->getInputCloud(), Scene.getPointCloudScene());
}

TEST(Test_InitPointCloudRetouch, DeathTest_InitSceneWithErrorPtr)
{
	//��ָ��
	{
		CPointCloudRetouchScene Scene;
		ASSERT_ANY_THROW(Scene.init(nullptr));
	}

	//δ�������ָ��
	{
		CPointCloudRetouchScene Scene;
		PointCloud_t::Ptr pCloud;
		ASSERT_ANY_THROW(Scene.init(pCloud));
	}

	//
	{
		CPointCloudRetouchScene Scene;
		PointCloud_t::ConstPtr pCloud(new PointCloud_t);
		pcl::io::loadPCDFile(g_CloudPath, *pCloud);
		Scene.init(pCloud);
	}

}


