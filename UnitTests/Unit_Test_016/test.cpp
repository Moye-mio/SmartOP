#include "pch.h"
#include "PointCloudRetouchInterface.h"
#include "NeighborhoodBuilder.h"
#include "PointCloudRetouchConfig.h"
#include "PointCloudRetouchManager.h"
#include "PointLabelSet.h"

//	���������б�
//	* Illegal_Input_Test����������Ƿ�ʱ�ܷ���ȷ����������ܷ������ȷ����
//	* Symmetry_Test���ھӹ�ϵ���жԳ��ԣ�����A��B���ھӣ���Bһ����A���ھ� FIXME: �ھӹ�ϵ�ƺ�������Ҳ����û�жԳ���
//	* Anti_Reflexive_Test���ھӹ�ϵ���з��Է��ԣ���Aһ������A���ھ�
//	* ��ʱֻ����INeighborhoodBuilder����Ĺ���

using namespace hiveObliquePhotography::PointCloudRetouch;

const auto Signature = KEYWORD::EUCLIDEAN_NEIGHBOR_BUILDER;

class CTestNeighborhoodBuilder :public testing::Test
{
protected:

	void SetUp() override
	{}

	void TearDown() override
	{}

	CPointCloudRetouchManager* generateRandomTestee(int vPointNum, int vFrom, int vTo)
	{
		PointCloud_t::Ptr pCloud(new PointCloud_t);
		for (int i = 0; i < vPointNum; i++)
		{
			PointCloud_t::PointType Point;
			Point.x = hiveMath::hiveGenerateRandomInteger(vFrom, vTo);
			Point.y = hiveMath::hiveGenerateRandomInteger(vFrom, vTo);
			Point.z = hiveMath::hiveGenerateRandomInteger(vFrom, vTo);
			pCloud->push_back(Point);
		}

		hiveConfig::CHiveConfig* pConfig = new CPointCloudRetouchConfig;
		hiveConfig::hiveParseConfig("PointCloudRetouchConfig.xml", hiveConfig::EConfigType::XML, pConfig);

		auto pManager = CPointCloudRetouchManager::getInstance();
		pManager->init(pCloud, pConfig->findSubconfigByName("Retouch"));

		return pManager;
	}
};

TEST_F(CTestNeighborhoodBuilder, Illegal_Input_Test)
{
	auto pManager = generateRandomTestee(1000, -100, 100);
	
	ASSERT_NO_THROW(pManager->buildNeighborhood(50, 1));
	ASSERT_ANY_THROW(pManager->buildNeighborhood(-1, 1));
	ASSERT_ANY_THROW(pManager->buildNeighborhood(1000000, 1));
}

TEST_F(CTestNeighborhoodBuilder, Symmetry_Test)
{
	auto pManager = generateRandomTestee(1000, -100, 100);

	for (size_t i = 0; i < 500; i++)
	{
		auto TestIndex = hiveMath::hiveGenerateRandomInteger(0, 999);
		
		auto Neighbor = pManager->buildNeighborhood(TestIndex, 1).back();
		
		auto Neighborhood = pManager->buildNeighborhood(Neighbor, 1);
		auto k = Neighborhood.begin();
		for (; k != Neighborhood.end(); ++k)
			if (*k == TestIndex)
				break;

		ASSERT_NE(k, Neighborhood.end());
	}
}

TEST_F(CTestNeighborhoodBuilder, Anti_Reflexive_Test)
{
	auto pManager = generateRandomTestee(1000, -100, 100);

	for (size_t i = 0; i < 500; i++)
	{
		auto TestIndex = hiveMath::hiveGenerateRandomInteger(0, 999);

		std::vector<pcl::index_t> Neighborhood = pManager->buildNeighborhood(TestIndex, 1);

		auto k = Neighborhood.begin();
		for (; k != Neighborhood.end(); k++)
			if (*k == TestIndex)
				break;

		ASSERT_EQ(k, Neighborhood.end());
	}
}
