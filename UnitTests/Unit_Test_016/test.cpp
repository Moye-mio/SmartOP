#include "pch.h"
#include "PointCloudRetouchInterface.h"
#include "NeighborhoodBuilder.h"
#include "PointCloudRetouchManager.h"
#include "PointLabelSet.h"

//	���������б�
//	* Illegal_Input_Test����������Ƿ�ʱ�ܷ���ȷ����������ܷ������ȷ����
//	* Symmetry_Test���ھӹ�ϵ���жԳ��ԣ�����A��B���ھӣ���Bһ����A���ھ�
//	* Anti_Reflexive_Test���ھӹ�ϵ���з��Է��ԣ���Aһ������A���ھ�
//	* ��ʱֻ����INeighborhoodBuilder����Ĺ���

using namespace hiveObliquePhotography::PointCloudRetouch;

const auto Signature = KEYWORD::EUCLIDEAN_NEIGHBOR_BUILDER;

INeighborhoodBuilder* generateRandomTestee(int vPointNum, int vFrom, int vTo)
{
	PointCloud_t::Ptr pScene(new PointCloud_t);
	CPointLabelSet PointLabelSet;

	hiveConfig::CHiveConfig Config;
	CPointCloudRetouchManager::getInstance()->init(pScene, &Config);
	
	for (int i = 0; i < vPointNum; i++)
	{
		PointCloud_t::PointType Point;
		Point.x = hiveMath::hiveGenerateRandomInteger(vFrom, vTo);
		Point.y = hiveMath::hiveGenerateRandomInteger(vFrom, vTo);
		Point.z = hiveMath::hiveGenerateRandomInteger(vFrom, vTo);
		pScene->push_back(Point);
	}
	PointLabelSet.init(pScene->size());
	return hiveDesignPattern::hiveCreateProduct<INeighborhoodBuilder>(Signature, pScene, &PointLabelSet);
}


TEST(TestNeighborhoodBuilder, Illegal_Input_Test)
{
	auto pNeighborhoodBuilder = generateRandomTestee(1000, -100, 100);
	/*ASSERT_NO_THROW(pNeighborhoodBuilder = hiveDesignPattern::hiveGetOrCreateProduct<INeighborhoodBuilder>(Signature, pScene, &PointLabelSet));*/

	std::vector<pcl::index_t> Neighborhood;
	ASSERT_ANY_THROW(pNeighborhoodBuilder->buildNeighborhood(-1, -1, Neighborhood));
	ASSERT_ANY_THROW(pNeighborhoodBuilder->buildNeighborhood(999, 999, Neighborhood));
	ASSERT_ANY_THROW(pNeighborhoodBuilder->buildNeighborhood(50, 50, Neighborhood));
}

TEST(TestNeighborhoodBuilder, Symmetry_Test)
{
	auto pNeighborhoodBuilder = generateRandomTestee(1000, -100, 100);

	for (size_t i = 0; i < 500; i++)
	{
		auto TestIndex = hiveMath::hiveGenerateRandomInteger(0, 999);

		std::vector<pcl::index_t> Neighborhood;
		pNeighborhoodBuilder->buildNeighborhood(TestIndex, 0, Neighborhood);
		auto Neighbor = Neighborhood.front();

		pNeighborhoodBuilder->buildNeighborhood(Neighbor, 0, Neighborhood);
		auto k = Neighborhood.begin();
		for (; k != Neighborhood.end(); k++)
			if (*k == TestIndex)
				break;

		ASSERT_NE(k, Neighborhood.end());
	}
}

TEST(TestNeighborhoodBuilder, Anti_Reflexive_Test)
{
	auto pNeighborhoodBuilder = generateRandomTestee(1000, -100, 100);

	for (size_t i = 0; i < 500; i++)
	{
		auto TestIndex = hiveMath::hiveGenerateRandomInteger(0, 999);

		std::vector<pcl::index_t> Neighborhood;
		pNeighborhoodBuilder->buildNeighborhood(TestIndex, 0, Neighborhood);

		auto k = Neighborhood.begin();
		for (; k != Neighborhood.end(); k++)
			if (*k == TestIndex)
				break;

		ASSERT_EQ(k, Neighborhood.end());
	}
}
