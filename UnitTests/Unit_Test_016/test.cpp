#include "pch.h"
#include "PointCloudRetouchInterface.h"
#include "NeighborhoodBuilder.h"
#include "PointCloudRetouchConfig.h"
#include "PointCloudRetouchManager.h"
#include "PointLabelSet.h"

//	���������б�
//	* Illegal_Input_Test����������Ƿ�ʱ�ܷ���ȷ����������ܷ������ȷ����
//	* Symmetry_Test���ھӹ�ϵ���жԳ��ԣ�����A��B���ھӣ���Bһ����A���ھ� FIXME: �ھӹ�ϵ�ƺ�������Ҳ����û�жԳ��� 2021.7.12���£�K����û�жԳ��ԣ��뾶�����жԳ��ԣ��˴�ʹ�ð뾶����
//	* Reflexive_Test���ھӹ�ϵ�����Է��ԣ���Aһ����A���ھ�
//	* ��ʱֻ����INeighborhoodBuilder����Ĺ���

using namespace hiveObliquePhotography::PointCloudRetouch;

const auto Signature = KEYWORD::EUCLIDEAN_NEIGHBOR_BUILDER;

class CTestNeighborhoodBuilder :public testing::Test
{
public:
	const std::string RadiusConfigFilePath = TESTMODEL_DIR + std::string("Config/Test016_PointCloudRetouchConfig_RADIUS.xml");
	const std::string NearestConfigFilePath = TESTMODEL_DIR + std::string("Config/Test016_PointCloudRetouchConfig_NEAREST.xml");

protected:
	void SetUp() override
	{}

	void TearDown() override
	{}

	CPointCloudRetouchManager* generateRandomTestee(int vPointNum, int vFrom, int vTo, const std::string& vConfigFilePath)
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
		hiveConfig::hiveParseConfig(vConfigFilePath, hiveConfig::EConfigType::XML, pConfig);

		auto pManager = CPointCloudRetouchManager::getInstance();
		pManager->init(pCloud, pConfig->findSubconfigByName("Retouch"));

		return pManager;
	}
};

TEST_F(CTestNeighborhoodBuilder, Radius_Illegal_Input_Test)
{
	auto pManagerRadius = generateRandomTestee(1000, -100, 100, NearestConfigFilePath);

	ASSERT_NO_THROW(pManagerRadius->buildNeighborhood(50, 1));
	ASSERT_ANY_THROW(pManagerRadius->buildNeighborhood(-1, 1));
	ASSERT_ANY_THROW(pManagerRadius->buildNeighborhood(1000000, 1));
}

TEST_F(CTestNeighborhoodBuilder, Nearest_Illegal_Input_Test)
{
	auto pManagerNearest = generateRandomTestee(1000, -100, 100, NearestConfigFilePath);

	ASSERT_NO_THROW(pManagerNearest->buildNeighborhood(50, 1));
	ASSERT_ANY_THROW(pManagerNearest->buildNeighborhood(-1, 1));
	ASSERT_ANY_THROW(pManagerNearest->buildNeighborhood(1000000, 1));
}

TEST_F(CTestNeighborhoodBuilder, Radius_Symmetry_Test)
{
	auto pManager = generateRandomTestee(1000, -100, 100, RadiusConfigFilePath);
	int SeedClusterIndex = 0;

	for (size_t i = 0; i < 500; i++)
	{
		auto TestIndex = hiveMath::hiveGenerateRandomInteger(0, 999);
		
		auto TestIndexNeighborhood = pManager->buildNeighborhood(TestIndex, ++SeedClusterIndex);
		if (TestIndexNeighborhood.size() == 0)
			continue;

		auto Neighbor = TestIndexNeighborhood.back();
		
		auto Neighborhood = pManager->buildNeighborhood(Neighbor, ++SeedClusterIndex);
		auto k = Neighborhood.begin();
		for (; k != Neighborhood.end(); ++k)
			if (*k == TestIndex)
				break;

		ASSERT_NE(k, Neighborhood.end());
	}
}