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

#define PI 3.141592653589793
#define radians(x) (x * PI / 180)

class CTestCreateInitialCluster : public testing::Test
{
protected:
	void SetUp() override
	{
		m_pCloud1.reset(new PointCloud_t);
		m_pCloud1->resize(100);
		
		float Angle = 0.0f, AngleStep = 30.0f, Radius = 0.1f, RadiusStep = 0.1f;
		while (Angle < 360.0f && Radius <= 1.0f)
		{
			pcl::PointSurfel Point;
			Point.x = Radius * cos(radians(Angle));
			Point.y = Radius * sin(radians(Angle));
			Point.z = 1.0f;
			Point.rgba = -1;
			m_pCloud1->push_back(Point);
		}

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