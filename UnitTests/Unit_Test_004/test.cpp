#include "pch.h"
#include "AutoRetouchInterface.h"
#include "VisualizationInterface.h"
#include "BinaryClassifierAlg.h"
#include "PointCluster4VFH.h"
#include "PointCluster4Score.h"
#include "PointCluster4NormalRatio.h"
#include "CompositeClassifier.h"
#include "PointCloudVisualizer.h"
#include <pcl/point_types.h>
#include "pcl/io/pcd_io.h"

#include <fstream>
#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

using namespace hiveObliquePhotography::AutoRetouch;

//���������б�
//  * Cluster_Overview_Test: ������ClusterӦ�������Լ��
//  * Cluster_Set_Test: ����cluster set�Ƿ���������
//  * BinaryAlg_Overview_Test: �Զ������㷨���������Լ��
// 
//  * Cluster_VFH_Test: �ܹ���ȷ����VFH��Cluster����ȷ�����Ϣ
//  * Cluster_Score_Test: �ܹ���ȷ����Score��Cluster����ȷ�����Ϣ
//  * Cluster_NormalRatio_Test: �ܹ���ȷ����NormalRatio��Cluster����ȷ�����Ϣ
// 
//  * Composite_BinaryAlg_Test: ��ÿ��ʹ�õ�һ����Cluster�Ķ�����ĸ����㷨�����Լ��
// 
//  * Composite_BinaryAlg_Expect_Test: �Ը����㷨�Ľ��Ԥ��
//  * DeathTest_Composite_BinaryAlg: �����㷨������ܰ���������Լ����

const bool bIsEnableVisualizer = true;

const std::string g_Folder = "test_tile16/";
const std::string g_CloudFile = "Scu_Tile16.pcd";
const std::string g_UnwantedTreePoints = "SomeBigTreePoints.txt";
const std::string g_KeptGroundPoints = "SomeGroundPoints.txt";
const std::string g_RestrictPoints1 = "SomeGroundPoints.txt";

class CBinaryTest : public testing::Test
{
protected:
	void SetUp() override
	{
		//init scene
		m_pCloud.reset(new pcl::PointCloud<pcl::PointSurfel>);
		pcl::io::loadPCDFile(g_Folder + g_CloudFile, *m_pCloud);

		hiveObliquePhotography::AutoRetouch::CPointCloudAutoRetouchScene::getInstance()->init(m_pCloud);

		if (bIsEnableVisualizer)
		{
			hiveObliquePhotography::Visualization::CPointCloudVisualizer::getInstance()->init(m_pCloud, false);
			hiveObliquePhotography::Visualization::CPointCloudVisualizer::getInstance()->refresh();
		}

		createTestcaseContext();
	}
	
	void TearDown() override
	{
		if (bIsEnableVisualizer)
		{
			hiveObliquePhotography::Visualization::CPointCloudVisualizer::getInstance()->refresh();
			system("pause");
		}
	}

	pcl::IndicesPtr loadPointIndices(const std::string& vPath)
	{
		std::vector<int> Indices;
		std::ifstream File(vPath.c_str());
		boost::archive::text_iarchive ia(File);
		ia >> BOOST_SERIALIZATION_NVP(Indices);
		File.close();	//ia����ܹر�
		pcl::IndicesPtr pIndices(new pcl::Indices(Indices));
		return pIndices;
	}

	void createTestcaseContext()
	{
		m_pUnwantedIndices = loadPointIndices(g_Folder + g_UnwantedTreePoints);
		m_pUnwantedCluster4VFH = new CPointCluster4VFH(m_pUnwantedIndices, m_Unwanted);
		m_pUnwantedCluster4Score = new CPointCluster4Score(m_pUnwantedIndices, m_Unwanted);
		m_pUnwantedCluster4NormalRatio = new CPointCluster4NormalRatio(m_pUnwantedIndices, m_Unwanted);

		m_pKeptIndices = loadPointIndices(g_Folder + g_KeptGroundPoints);
		m_pKeptCluster4VFH = new CPointCluster4VFH(m_pKeptIndices, m_Kept);
		m_pKeptCluster4Score = new CPointCluster4Score(m_pKeptIndices, m_Kept);
		m_pKeptCluster4NormalRatio = new CPointCluster4NormalRatio(m_pKeptIndices, m_Kept);

		//Լ����
		m_pRestrictIndices.push_back(loadPointIndices(g_Folder + g_RestrictPoints1));
	}

	pcl::IndicesPtr m_pUnwantedIndices;
	pcl::IndicesPtr m_pKeptIndices;
	EPointLabel m_Unwanted = EPointLabel::UNWANTED;
	EPointLabel m_Kept = EPointLabel::KEPT;

	std::vector<pcl::IndicesPtr> m_pRestrictIndices;

	CPointCluster4VFH* m_pUnwantedCluster4VFH = nullptr;
	CPointCluster4VFH* m_pKeptCluster4VFH = nullptr;
	CPointCluster4Score* m_pUnwantedCluster4Score = nullptr;
	CPointCluster4Score* m_pKeptCluster4Score = nullptr;
	CPointCluster4NormalRatio* m_pUnwantedCluster4NormalRatio = nullptr;
	CPointCluster4NormalRatio* m_pKeptCluster4NormalRatio = nullptr;

	pcl::PointCloud<pcl::PointSurfel>::ConstPtr const getCloud() const { return m_pCloud; }

private:
	pcl::PointCloud<pcl::PointSurfel>::Ptr m_pCloud = nullptr;
};

TEST_F(CBinaryTest, Cluster_Overview_Test)
{
	ASSERT_EQ(m_pUnwantedCluster4VFH->getClusterLabel(), m_Unwanted);

	ASSERT_EQ(m_pKeptCluster4VFH->getClusterLabel(), m_Kept);

	pcl::index_t ErrorIndex1 = -1;
	pcl::index_t ErrorIndex2 = getCloud()->size() + 1;

	pcl::IndicesPtr ErrorIndices1(new pcl::Indices{ ErrorIndex1 });
	pcl::IndicesPtr ErrorIndices2(new pcl::Indices{ ErrorIndex2 });

	//��Ч�Ĵ���ʧ�ܵ������׳��쳣
	EXPECT_NO_THROW(new CPointCluster4VFH(ErrorIndices1, m_Kept));
	EXPECT_NO_THROW(new CPointCluster4VFH(ErrorIndices2, m_Unwanted));

	//��Ч�������ʧ�ܵ������׳��쳣
	EXPECT_NO_THROW(m_pUnwantedCluster4VFH->computeSimilarityV(ErrorIndex1));
	EXPECT_NO_THROW(m_pUnwantedCluster4VFH->computeSimilarityV(ErrorIndex2));
	EXPECT_NO_THROW(m_pKeptCluster4VFH->computeSimilarityV(ErrorIndex1));
	EXPECT_NO_THROW(m_pKeptCluster4VFH->computeSimilarityV(ErrorIndex2));

	//���еĵ����������Լ�
	for (int i = 0, step = 3; i < m_pUnwantedIndices->size() && i < m_pKeptIndices->size(); i += step)
	{
		EXPECT_GT(m_pUnwantedCluster4VFH->computeSimilarityV((*m_pUnwantedIndices)[i]), m_pKeptCluster4VFH->computeSimilarityV((*m_pUnwantedIndices)[i]));
		EXPECT_GT(m_pKeptCluster4VFH->computeSimilarityV((*m_pKeptIndices)[i]), m_pUnwantedCluster4VFH->computeSimilarityV((*m_pKeptIndices)[i]));
	}

}

TEST_F(CBinaryTest, Cluster_Set_Test)
{
	auto pClusterSet = CPointClusterSet::getInstance();
	std::vector<IPointCluster*> pClusters = { m_pUnwantedCluster4VFH, m_pKeptCluster4VFH, m_pUnwantedCluster4Score, m_pKeptCluster4Score };

	for (int i = 0; i < pClusters.size(); i++)
	{
		pClusterSet->addPointCluster("test_" + std::to_string(i), pClusters[i]);
	}

	auto pGlobalClusters = pClusterSet->getGlobalClusterSet("test");
	ASSERT_EQ(pGlobalClusters.size(), pClusters.size());

}

TEST_F(CBinaryTest, BinaryAlg_Overview_Test)
{
	std::vector<IPointCluster*> pClusters = { m_pUnwantedCluster4VFH, m_pKeptCluster4VFH };

	for (int i = 0; i < pClusters.size(); i++)
		CPointClusterSet::getInstance()->addPointCluster(std::to_string(i), pClusters[i]);

	auto* pClassifier = hiveDesignPattern::hiveCreateProduct<IPointClassifier>(CLASSIFIER_BINARY, CPointCloudAutoRetouchScene::getInstance()->fetchPointLabelSet());
	ASSERT_NE(pClassifier, nullptr);
	pClassifier->execute<CBinaryClassifierAlg>(true, "test");

	//�����Ŀ
	auto LabelChanged = pClassifier->getResult();
	ASSERT_GT(LabelChanged.size(), 0);
	ASSERT_LT(LabelChanged.size(), getCloud()->size() - m_pUnwantedIndices->size() - m_pKeptIndices->size());

	pcl::index_t Index;
	auto compare = [&](SPointLabelChange vLabelChange) -> bool
	{
		return (vLabelChange.Index == Index);
	};

	//����в����д��еĵ�
	int i = 0, step = 5;
	for (; i < m_pUnwantedIndices->size(); i += step)
	{
		Index = (*m_pUnwantedIndices)[i];
		std::vector<SPointLabelChange>::iterator it = find_if(LabelChanged.begin(), LabelChanged.end(), compare);
		ASSERT_EQ(it, LabelChanged.end());
	}
	for (i = 0; i < m_pKeptIndices->size(); i += step)
	{
		Index = (*m_pKeptIndices)[i];
		std::vector<SPointLabelChange>::iterator it = find_if(LabelChanged.begin(), LabelChanged.end(), compare);
		ASSERT_EQ(it, LabelChanged.end());
	}

	//����в����и���Լ����
	for (auto& pRestrictIndices : m_pRestrictIndices)
	{
		for (auto& RestrictIndex : *pRestrictIndices)
		{
			Index = RestrictIndex;
			std::vector<SPointLabelChange>::iterator it = find_if(LabelChanged.begin(), LabelChanged.end(), compare);
			ASSERT_EQ(it, LabelChanged.end());
		}
	}
}

TEST_F(CBinaryTest, Cluster_VFH_TEST)
{
	auto& UnwantedVFHMatrix = m_pUnwantedCluster4VFH->getVFHMatrix();
	auto& KeptVFHMatrix = m_pKeptCluster4VFH->getVFHMatrix();

	//�Ƿ�Ϊ0��1
	//ASSERT_TRUE((UnwantedVFHMatrix.array() == 0).any());
	//ASSERT_TRUE((KeptVFHMatrix.array() == 0).any());

	ASSERT_EQ(UnwantedVFHMatrix.minCoeff(), 0);
	ASSERT_GT(UnwantedVFHMatrix.squaredNorm(), 0);

	ASSERT_EQ(KeptVFHMatrix.minCoeff(), 0);
	ASSERT_GT(KeptVFHMatrix.squaredNorm(), 0);

}

TEST_F(CBinaryTest, Cluster_Score_Test)
{
	auto UnwantedNormal = m_pUnwantedCluster4Score->getNormal();
	auto UnwantedColor = m_pUnwantedCluster4Score->getColor();
	auto UnwantedPosition = m_pUnwantedCluster4Score->getPosition();

	auto KeptNormal = m_pKeptCluster4Score->getNormal();
	auto KeptColor = m_pKeptCluster4Score->getColor();
	auto KeptPosition = m_pKeptCluster4Score->getPosition();

	//���߹�һ��
	ASSERT_LE(UnwantedNormal.squaredNorm(), 1);
	ASSERT_LE(KeptNormal.squaredNorm(), 1);

	//��ɫ������׼��
	Eigen::Vector3i MinColor{ 0, 0, 0 };
	Eigen::Vector3i MaxColor{ 255, 255, 255 };
	ASSERT_TRUE((UnwantedColor.array() > MinColor.array()).all());
	ASSERT_TRUE((UnwantedColor.array() < MaxColor.array()).all());
	ASSERT_TRUE((KeptColor.array() > MinColor.array()).all());
	ASSERT_TRUE((KeptColor.array() < MaxColor.array()).all());

	//λ�ò�����Χ��
	auto SceneAABB = CPointCloudAutoRetouchScene::getInstance()->getSceneAABB();
	ASSERT_TRUE((UnwantedPosition.array() > SceneAABB.Min.array()).all());
	ASSERT_TRUE((UnwantedPosition.array() < SceneAABB.Max.array()).all());
	ASSERT_TRUE((KeptPosition.array() > SceneAABB.Min.array()).all());
	ASSERT_TRUE((KeptPosition.array() < SceneAABB.Max.array()).all());
}

TEST_F(CBinaryTest, Cluster_NormalRatio_Test)
{

}

TEST_F(CBinaryTest, Composite_BinaryAlg_Test)
{
	//auto* pClassifier = hiveDesignPattern::hiveGetOrCreateProduct<IPointClassifier>(CLASSIFIER_BINARY, CPointCloudAutoRetouchScene::getInstance()->fetchPointLabelSet());
	//ASSERT_NE(pClassifier, nullptr);
	//pClassifier->execute<CBinaryClassifierAlg>(true);

	//auto Results = pClassifier->getResults();

	//for (auto& Result : Results)
	//{
	//	//�����Ŀ
	//	auto LabelChanged = Result;
	//	ASSERT_GT(LabelChanged.size(), 0);
	//	ASSERT_LT(LabelChanged.size(), getCloud()->size());
	//	ASSERT_EQ(LabelChanged.size(), getCloud()->size() - m_pUnwantedIndices->size() - m_pKeptIndices->size());

	//	pcl::index_t Index;
	//	auto compare = [&](SPointLabelChange vLabelChange) -> bool
	//	{
	//		return (vLabelChange.Index == Index);
	//	};

	//	//����в����д��еĵ�
	//	int i = 0, step = 5;
	//	for (; i < m_pUnwantedIndices->size(); i += step)
	//	{
	//		Index = (*m_pUnwantedIndices)[i];
	//		std::vector<SPointLabelChange>::iterator it = find_if(LabelChanged.begin(), LabelChanged.end(), compare);
	//		ASSERT_NE(it, LabelChanged.end());
	//	}
	//	for (i = 0; i < m_pKeptIndices->size(); i += step)
	//	{
	//		Index = (*m_pKeptIndices)[i];
	//		std::vector<SPointLabelChange>::iterator it = find_if(LabelChanged.begin(), LabelChanged.end(), compare);
	//		ASSERT_NE(it, LabelChanged.end());
	//	}
	//}
}

TEST_F(CBinaryTest, Composite_BinaryAlg_Expect_Test)
{

}

TEST_F(CBinaryTest, DeathTest_Composite_BinaryAlg)
{

}