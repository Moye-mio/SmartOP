#include "pch.h"
#include "PointCloudRetouchInterface.h"
#include "NeighborhoodBuilder.h"
#include "PointLabelSet.h"

//	���������б�
//	* Illegal_Input_Test����������Ƿ�ʱ�ܷ���ȷ����������ܷ������ȷ����
//	* Symmetry_Test���ھӹ�ϵ���жԳ��ԣ�����A��B���ھӣ���Bһ����A���ھ�
//	* Anti_Reflexive_Test���ھӹ�ϵ���з��Է��ԣ���Aһ������A���ھ�
//	* ��ʱֻ����INeighborhoodBuilder����Ĺ���

using namespace hiveObliquePhotography::PointCloudRetouch;

constexpr char Signature[] = "TODO: add signature here";

INeighborhoodBuilder* generateRandomTestee(int vPointNum, int vFrom, int vTo)
{
	PointCloud_t::Ptr pScene(new PointCloud_t);
	CPointLabelSet PointLabelSet;

	for (int i = 0; i < vPointNum; i++)
	{
		PointCloud_t::PointType Point;
		Point.x = hiveMath::hiveGenerateRandomInteger(vFrom, vTo);
		Point.y = hiveMath::hiveGenerateRandomInteger(vFrom, vTo);
		Point.z = hiveMath::hiveGenerateRandomInteger(vFrom, vTo);
		pScene->push_back(Point);
	}
	PointLabelSet.init(pScene->size());
	return hiveDesignPattern::hiveGetOrCreateProduct<INeighborhoodBuilder>(Signature, pScene, &PointLabelSet);
}


TEST(TestNeighborhoodBuilder, Illegal_Input_Test)
{
	INeighborhoodBuilder* pNeighborhoodBuilder;
	PointCloud_t::Ptr pScene(new PointCloud_t);
	CPointLabelSet PointLabelSet;

	ASSERT_ANY_THROW(pNeighborhoodBuilder = hiveDesignPattern::hiveGetOrCreateProduct<INeighborhoodBuilder>(Signature, nullptr, nullptr));
	for (size_t i = 0; i < 100; i++)
	{
		PointCloud_t::PointType Point;
		Point.x = Point.y = Point.z = i;
		pScene->push_back(Point);
	}
	ASSERT_ANY_THROW(pNeighborhoodBuilder = hiveDesignPattern::hiveGetOrCreateProduct<INeighborhoodBuilder>(Signature, pScene, PointLabelSet));
	PointLabelSet.init(pScene->size());
	ASSERT_NO_THROW(pNeighborhoodBuilder = hiveDesignPattern::hiveGetOrCreateProduct<INeighborhoodBuilder>(Signature, pScene, &PointLabelSet));

	std::vector<pcl::index_t> Neighborhood;
	ASSERT_ANY_THROW(pNeighborhoodBuilder->buildNeighborhood(-1, Neighborhood));
	ASSERT_ANY_THROW(pNeighborhoodBuilder->buildNeighborhood(999, Neighborhood));
	ASSERT_NO_THROW(pNeighborhoodBuilder->buildNeighborhood(50, Neighborhood));
}

TEST(TestNeighborhoodBuilder, Symmetry_Test)
{
	auto pNeighborhoodBuilder = generateRandomTestee(1000, -100, 100);

	for (size_t i = 0; i < 500; i++)
	{
		auto TestIndex = hiveMath::hiveGenerateRandomInteger(0, 999);

		std::vector<pcl::index_t> Neighborhood;
		pNeighborhoodBuilder->buildNeighborhood(TestIndex, Neighborhood);
		auto Neighbor = Neighborhood.front();

		pNeighborhoodBuilder->buildNeighborhood(Neighbor, Neighborhood);
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
		pNeighborhoodBuilder->buildNeighborhood(TestIndex, Neighborhood);

		auto k = Neighborhood.begin();
		for (; k != Neighborhood.end(); k++)
			if (*k == TestIndex)
				break;

		ASSERT_EQ(k, Neighborhood.end());
	}
}
