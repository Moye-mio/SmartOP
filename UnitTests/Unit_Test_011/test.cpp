#include "pch.h"
#include "ObliquePhotographyDataInterface.h"
#include "PointCloudRetouchInterface.h"
#include "PointCloudRetouchManager.h"
#include "PointCloudRetouchConfig.h"
#include "VisualizationInterface.h"
#include "InitialClusterCreator.h"
#include "PointCluster.h"
//��ҪΧ��CInitialClusterCreator::createInitialCluster()���в���

//���������б�
//  * BaseTest1_Create_Cluster: ����Բ�ڵ㼯����ͬӲ�ȣ��ܴ��������зֽ��GenerationSet, ValidationSet��CPointCluster

//	* DeathTest1_User_Caused_Error: �����û����µĴ��󣺿��������������Ӳ�ȣ������label���ܲ��׳��κ��쳣���ı�����������
//	* DeathTest2_User_Caused_Error: ���������µĴ��󣺴���������������PV���󣬴���config�����׳��쳣

using namespace hiveObliquePhotography;

#define PI 3.141592653589793
#define radians(x) (x * PI / 180)

const std::string ConfigPath = TESTMODEL_DIR + std::string("Config/Test011_PointCloudRetouchConfig.xml");
const std::string CameraPath = TESTMODEL_DIR + std::string("Test011_Model/VirtualCircleCameraInfo.txt");

class CTestCreateInitialCluster : public testing::Test
{ 
protected:
	void SetUp() override
	{
		m_pCloud = std::make_shared<PointCloud_t>();
		float AngleStep = 10.0f, RadiusStep = 0.05f, Epsilon = 0.0005f;
		for (float Angle = 0.0f; Angle < 360.0f; Angle += AngleStep)
		{

			for (float Radius = RadiusStep; Radius - m_CircleRadius <= Epsilon; Radius += RadiusStep)
			{
				pcl::PointXYZRGBNormal Point;
				Point.x = Radius * cos(radians(Angle));
				Point.y = Radius * sin(radians(Angle));
				Point.z = 0.0f;
				Point.curvature = Radius;
				Point.rgb = -1;
				m_pCloud->push_back(Point);
			}
		}
		for (int i = 0; i < m_pCloud->size(); i++)
			m_Indices.push_back(i);

		m_pConfig = new PointCloudRetouch::CPointCloudRetouchConfig;
		if (hiveConfig::hiveParseConfig(ConfigPath, hiveConfig::EConfigType::XML, m_pConfig) != hiveConfig::EParseResult::SUCCEED)
		{
			_HIVE_OUTPUT_WARNING(_FORMAT_STR1("Failed to parse config file [%1%].", ConfigPath));
			return;
		}

		PointCloudRetouch::hiveInit({ m_pCloud }, m_pConfig);

		m_pVisualizer = std::make_shared<pcl::visualization::PCLVisualizer>();
		m_pVisualizer->addPointCloud<pcl::PointXYZRGBNormal>(m_pCloud, "Cloud2Show");
		m_pVisualizer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "Cloud2Show");
		m_pVisualizer->loadCameraParameters(CameraPath);
		m_pVisualizer->updateCamera();

		pcl::visualization::Camera Camera;
		m_pVisualizer->getCameraParameters(Camera);
		Eigen::Matrix4d Proj, View;
		Camera.computeProjectionMatrix(Proj);
		Camera.computeViewMatrix(View);
		m_PV = Proj * View;
		m_WindowSize = { Camera.window_size[0], Camera.window_size[1] };

		Eigen::Vector4d Pos = { 0.0, m_CircleRadius, 0.0f, 1.0 };
		Pos = m_PV * Pos;
		Pos /= Pos.eval().w();

		m_RadiusInWindow = Pos.y() * m_WindowSize.y() * 0.5f;

		int i = 0;
		m_pHardnessFunc = [=](const Eigen::Vector2d& vPos) -> double
		{
			if (m_CurrentHardness <= 0)
				return 0.0;
			else if (m_CurrentHardness >= 1)
				return 1.0;

			Eigen::Vector2d PosOnWindow((vPos.x() + 1) * m_WindowSize.x() / 2, (vPos.y() + 1) * m_WindowSize.y() / 2);

			double X = (PosOnWindow - 0.5 * m_WindowSize).norm() / m_RadiusInWindow;
			if (X <= 1.0)
			{
				X -= m_CurrentHardness;
				if (X < 0)
					return 1.0;
				X /= (1 - m_CurrentHardness);
				X *= X;

				return X * (X - 2) + 1;
			}
			else
				return 0;
		};

	}

	void TearDown() override
	{
		//while (!m_pVisualizer->wasStopped())
		//{
		//	m_pVisualizer->spinOnce(16);
		//}

		delete m_pConfig;
	}

	PointCloud_t::Ptr m_pCloud = nullptr;
	hiveConfig::CHiveConfig* m_pConfig = nullptr;
	pcl::visualization::PCLVisualizer::Ptr m_pVisualizer = nullptr;

	std::function<double(Eigen::Vector2d)> m_pHardnessFunc;

	std::vector<int> m_Indices;
	Eigen::Matrix4d m_PV;

	float m_CircleRadius = 1.0f;
	Eigen::Vector2d m_WindowSize;
	float m_RadiusInWindow = -1.0f;
	float m_CurrentHardness = 0.0f;

	PointCloudRetouch::CInitialClusterCreator m_Creator;
};

//����Բ�ڵ㼯����ͬӲ�ȣ��ܴ��������зֽ��GenerationSet, ValidationSet��CPointCluster
TEST_F(CTestCreateInitialCluster, BaseTest1_Create_Cluster)
{
	//��Ӳ��
	{
		float ZeroHardness = 0.0f;
		m_CurrentHardness = ZeroHardness;
		PointCloudRetouch::CPointCluster* pCluster = nullptr;
		ASSERT_NO_THROW(pCluster = m_Creator.createInitialCluster(m_Indices, m_PV, m_pHardnessFunc, PointCloudRetouch::EPointLabel::UNWANTED, PointCloudRetouch::CPointCloudRetouchManager::getInstance()->getClusterConfig(true)));
		ASSERT_NE(pCluster, nullptr);
		if (pCluster)
		{
			auto GenerationSet = pCluster->getCoreRegion();
			ASSERT_TRUE(GenerationSet.empty());
		}
	}

	//Ӳ����
	{
		float FullHardness = 1.0f;
		m_CurrentHardness = FullHardness;
		auto pCluster = m_Creator.createInitialCluster(m_Indices, m_PV, m_pHardnessFunc, PointCloudRetouch::EPointLabel::UNWANTED, PointCloudRetouch::CPointCloudRetouchManager::getInstance()->getClusterConfig(true));
		ASSERT_NE(pCluster, nullptr);
		if (pCluster)
		{
			auto GenerationSet = pCluster->getCoreRegion();
			ASSERT_EQ(GenerationSet.size(), m_Indices.size());
		}
	}

	//���������
	for (double Hardness = 0.1f; Hardness < 1.0f; Hardness += 0.2f)
	{
		m_CurrentHardness = Hardness;
		auto pCluster = m_Creator.createInitialCluster(m_Indices, m_PV, m_pHardnessFunc, PointCloudRetouch::EPointLabel::UNWANTED, PointCloudRetouch::CPointCloudRetouchManager::getInstance()->getClusterConfig(true));
		ASSERT_NE(pCluster, nullptr);
		if (pCluster)
		{
			auto GenerationSet = pCluster->getCoreRegion();

			//��GenerationSet��������
			float MaxGenerationRadius = -FLT_MAX;
			int MaxIndex = 0;
			for (auto Index : GenerationSet)
			{
				auto Temp = m_pCloud->points[Index].curvature;
				if (Temp > MaxGenerationRadius)
				{
					MaxGenerationRadius = Temp;
					MaxIndex = Index;
				}
			}

			std::cerr << "CurrentHardness: " << Hardness << std::endl;
			std::cerr << "MaxIndex: " << MaxIndex << std::endl;
			ASSERT_NE(MaxGenerationRadius, m_CircleRadius);
			ASSERT_NE(GenerationSet.size(), m_Indices.size());

			//GenerationSet��߽�һȦ�ϵĵ㶼����
			int NumGenerationBoundPoints = 0;
			for (auto Index : GenerationSet)
			{
				auto Temp = m_pCloud->points[Index].curvature;
				if (Temp == MaxGenerationRadius)
					NumGenerationBoundPoints++;
			}
			ASSERT_NE(NumGenerationBoundPoints, 0);
			ASSERT_LE(NumGenerationBoundPoints, m_Indices.size());

			//ValidationSet�ĵ������붼����
			std::vector<int> ValidationSet;
			std::set_difference(m_Indices.begin(), m_Indices.end(),
				GenerationSet.begin(), GenerationSet.end(),
				std::inserter(ValidationSet, ValidationSet.begin()));
			ASSERT_TRUE(!ValidationSet.empty());

			float MinValidationRadius = FLT_MAX;
			for (auto Index : ValidationSet)
			{
				auto Temp = m_pCloud->points[Index].curvature;
				if(Temp <= MaxGenerationRadius)
					std::cerr << "Index: " << Index << std::endl;
				//ASSERT_GT(Temp, MaxGenerationRadius);

				if (Temp < MinValidationRadius)
					MinValidationRadius = Temp;
			}

			//ValidationSet�߽�һȦ�ϵĵ㶼����
			int NumValidationBoundPoints = 0;
			for (auto Index : ValidationSet)
			{
				auto Temp = m_pCloud->points[Index].curvature;
				if (Temp == MinValidationRadius)
					NumValidationBoundPoints++;
			}
			ASSERT_NE(NumValidationBoundPoints, 0);
			ASSERT_LE(NumValidationBoundPoints, m_Indices.size());
		}
	}

	//KEPTҲ�ܳɹ�����
	{
		m_CurrentHardness = 0.5f;
		auto pCluster = m_Creator.createInitialCluster(m_Indices, m_PV, m_pHardnessFunc, PointCloudRetouch::EPointLabel::KEPT, PointCloudRetouch::CPointCloudRetouchManager::getInstance()->getClusterConfig(false));
		ASSERT_NE(pCluster, nullptr);
		if (pCluster)
		{
			auto GenerationSet = pCluster->getCoreRegion();
			ASSERT_TRUE(!GenerationSet.empty());
			ASSERT_LT(GenerationSet.size(), m_Indices.size());
		}
	}

}

//���������������Ӳ�ȣ������label(�紴��DISCARDED��cluster)
TEST_F(CTestCreateInitialCluster, DeathTest1_User_Caused_Error)
{
	//������
	m_CurrentHardness = 0.5f;
	std::vector<int> EmptyIndices;
	ASSERT_NO_THROW(m_Creator.createInitialCluster(EmptyIndices, m_PV, m_pHardnessFunc, PointCloudRetouch::EPointLabel::UNWANTED, PointCloudRetouch::CPointCloudRetouchManager::getInstance()->getClusterConfig(true)));
	
	//��Ӳ�Ⱥͳ���1��
	float MinusHardness = -1.0f;
	m_CurrentHardness = MinusHardness;
	ASSERT_NO_THROW(m_Creator.createInitialCluster(m_Indices, m_PV, m_pHardnessFunc, PointCloudRetouch::EPointLabel::UNWANTED, PointCloudRetouch::CPointCloudRetouchManager::getInstance()->getClusterConfig(true)));
	float BigHardness = 2.0f;
	m_CurrentHardness = BigHardness;
	ASSERT_NO_THROW(m_Creator.createInitialCluster(m_Indices, m_PV, m_pHardnessFunc, PointCloudRetouch::EPointLabel::UNWANTED, PointCloudRetouch::CPointCloudRetouchManager::getInstance()->getClusterConfig(true)));

	//����Label: Discard, Undetermined
	m_CurrentHardness = 0.5f;
	ASSERT_NO_THROW(m_Creator.createInitialCluster(m_Indices, m_PV, m_pHardnessFunc, PointCloudRetouch::EPointLabel::DISCARDED, PointCloudRetouch::CPointCloudRetouchManager::getInstance()->getClusterConfig(true)));
	ASSERT_NO_THROW(m_Creator.createInitialCluster(m_Indices, m_PV, m_pHardnessFunc, PointCloudRetouch::EPointLabel::UNDETERMINED, PointCloudRetouch::CPointCloudRetouchManager::getInstance()->getClusterConfig(true)));
}

//����������������PV���󣬴���config
TEST_F(CTestCreateInitialCluster, DeathTest2_User_Caused_Error)
{
	m_CurrentHardness = 0.5f;

	//������
	std::vector<int> MinusIndices;
	MinusIndices.push_back(-1);
	ASSERT_ANY_THROW(m_Creator.createInitialCluster(MinusIndices, m_PV, m_pHardnessFunc, PointCloudRetouch::EPointLabel::UNWANTED, PointCloudRetouch::CPointCloudRetouchManager::getInstance()->getClusterConfig(true)));

	//������
	std::vector<int> GreatIndices;
	GreatIndices.push_back(m_Indices.size());
	ASSERT_ANY_THROW(m_Creator.createInitialCluster(GreatIndices, m_PV, m_pHardnessFunc, PointCloudRetouch::EPointLabel::UNWANTED, PointCloudRetouch::CPointCloudRetouchManager::getInstance()->getClusterConfig(true)));

	//��PV����
	//ASSERT_ANY_THROW(m_Creator.createInitialCluster(m_Indices, Eigen::Matrix4d{}, m_pHardnessFunc, PointCloudRetouch::EPointLabel::UNWANTED, PointCloudRetouch::CPointCloudRetouchManager::getInstance()->getClusterConfig(true)));

	//��config
	ASSERT_ANY_THROW(m_Creator.createInitialCluster(m_Indices, m_PV, m_pHardnessFunc, PointCloudRetouch::EPointLabel::UNWANTED, nullptr));

}