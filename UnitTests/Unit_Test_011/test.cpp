#include "pch.h"
#include "PointCloudRetouchInterface.h"
#include "InitialClusterCreator.h"
//��ҪΧ��CInitialClusterCreator::createInitialCluster()���в���

//���������б�
//  * GenerateHardness4EveryPoint: �ܹ���������ʽ����Ӳ��
//  * ComputeClusterCenter: �ܹ���ȷ������ĵ�
//  * DivideUserSpecifiedRegion: �ܹ���������ʽ��������
//  * InitPointCluster: �ܹ�������ʼ��Cluster
// 
//  * CreateInitialCluster: �ܹ���ȷ������ʼ��

using namespace hiveObliquePhotography::PointCloudRetouch;

class CTestCreateInitialCluster : public testing::Test
{
protected:
	void SetUp() override
	{
		m_pCloud1.reset(new PointCloud_t);
		m_pCloud1->resize(100);


	}

	void TearDown() override
	{

	}

	PointCloud_t::Ptr m_pCloud1 = nullptr;
};

TEST(Test_CreateInitialCluster, GenerateHardness4EveryPoint)
{
	CInitialClusterCreator Creator;
	Creator.testGenerateHardness4EveryPoint()
}

TEST(Test_CreateInitialCluster, ComputeClusterCenter)
{
}

TEST(Test_CreateInitialCluster, DivideUserSpecifiedRegion)
{
}

TEST(Test_CreateInitialCluster, CreateInitialCluster)
{
}