#include "pch.h"
#include "AutoRetouchInterface.h"
#include "VisualizationInterface.h"
#include "PointCloudAutoRetouchScene.h"
#include "PointCluster4VFH.h"
#include "PointCloudVisualizer.h"
#include "pcl/io/pcd_io.h"

#include <fstream>
#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

using namespace hiveObliquePhotography::AutoRetouch;

//���������б�
//  * Cluster_Overview_Test: ������ClusterӦ�������Լ��
//  * BinaryAlg_Overview_Test: �Զ������㷨���������Լ��
// 
//  * Cluster_VFH_TEST: �ܹ���ȷ����VFH��Cluster����ȷ�����Ϣ
//  * Cluster_Distribution_TEST: �ܹ���ȷ����Distribution��Cluster����ȷ�����Ϣ
//  * Cluster_Score_TEST: �ܹ���ȷ����Score��Cluster����ȷ�����Ϣ
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

class CTestBinary : public testing::Test
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

	pcl::Indices loadPointIndices(const std::string& vPath)
	{
		pcl::Indices Indices;
		std::ifstream File(vPath.c_str());
		boost::archive::text_iarchive ia(File);
		ia >> BOOST_SERIALIZATION_NVP(Indices);
		File.close();	//ia����ܹر�
		return Indices;
	}

	void createTestcaseContext()
	{
		auto UnwantedIndices = loadPointIndices(g_Folder + g_UnwantedTreePoints);
		m_pUnwantedIndices.reset(new pcl::Indices(UnwantedIndices));
		m_pUnwantedCluster = new CPointCluster4VFH(m_pUnwantedIndices, m_Unwanted);

		auto KeptIndicces = loadPointIndices(g_Folder + g_KeptGroundPoints);
		m_pKeptIndices.reset(new pcl::Indices(KeptIndicces));
		m_pKeptCluster = new CPointCluster4VFH(m_pKeptIndices, m_Kept);
	}

	pcl::IndicesPtr m_pUnwantedIndices;
	pcl::IndicesPtr m_pKeptIndices;
	EPointLabel m_Unwanted = EPointLabel::UNWANTED;
	EPointLabel m_Kept = EPointLabel::KEPT;

	IPointCluster* m_pUnwantedCluster = nullptr;
	IPointCluster* m_pKeptCluster = nullptr;

	pcl::PointCloud<pcl::PointSurfel>::ConstPtr const getCloud() const { return m_pCloud; }

private:
	pcl::PointCloud<pcl::PointSurfel>::Ptr m_pCloud = nullptr;
};

TEST_F(CTestBinary, Cluster_Overview_Test)
{
	ASSERT_EQ(m_pUnwantedCluster->getClusterLabel(), m_Unwanted);

	ASSERT_EQ(m_pKeptCluster->getClusterLabel(), m_Kept);

	pcl::index_t ErrorIndex1 = -1;
	pcl::index_t ErrorIndex2 = getCloud()->size() + 1;

	pcl::IndicesPtr ErrorIndices1(new pcl::Indices{ ErrorIndex1 });
	pcl::IndicesPtr ErrorIndices2(new pcl::Indices{ ErrorIndex2 });

	//��Ч�Ĵ���ʧ��
	EXPECT_DEATH(new CPointCluster4VFH(ErrorIndices1, m_Kept), ".*");
	EXPECT_DEATH(new CPointCluster4VFH(ErrorIndices2, m_Unwanted), ".*");

	//��Ч�������ʧ��
	EXPECT_DEATH(m_pUnwantedCluster->computeDistanceV(ErrorIndex1), ".*");
	EXPECT_DEATH(m_pUnwantedCluster->computeDistanceV(ErrorIndex2), ".*");
	EXPECT_DEATH(m_pKeptCluster->computeDistanceV(ErrorIndex1), ".*");
	EXPECT_DEATH(m_pKeptCluster->computeDistanceV(ErrorIndex2), ".*");

	//���еĵ����������Լ�
	for (int i = 0, step = 3; i < m_pUnwantedIndices->size() && i < m_pKeptIndices->size(); i += step)
	{
		EXPECT_GT(m_pUnwantedCluster->computeDistanceV((*m_pUnwantedIndices)[i]), m_pKeptCluster->computeDistanceV((*m_pUnwantedIndices)[i]));
		EXPECT_GT(m_pKeptCluster->computeDistanceV((*m_pKeptIndices)[i]), m_pUnwantedCluster->computeDistanceV((*m_pKeptIndices)[i]));
	}

}

TEST_F(CTestBinary, BinaryAlg_Overview_Test)
{
	std::vector<IPointCluster*> pClusters = { m_pUnwantedCluster, m_pKeptCluster };

	for (int i = 0; i < pClusters.size(); i++)
		CPointClusterSet::getInstance()->addPointCluster(std::to_string(i), pClusters[i]);

	auto* pClassifier = hiveDesignPattern::hiveGetOrCreateProduct<IPointClassifier>(CLASSIFIER_BINARY_VFH, CPointCloudAutoRetouchScene::getInstance()->fetchPointLabelSet());
	ASSERT_NE(pClassifier, nullptr);
	pClassifier->execute<CBinaryClassifierAlg>(true);

	auto LabelChanged = pClassifier->getResult();
	ASSERT_EQ(LabelChanged.size(), getCloud()->size() - m_pUnwantedIndices->size() - m_pKeptIndices->size());

}