#include "pch.h"
#include "RayCastingBaker.h"
#include "SceneReconstructionConfig.h"

#include <pcl/io/obj_io.h>
#include <pcl/io/vtk_lib_io.h>
#include "ObliquePhotographyDataInterface.h"

//���������б�
//  * findTexelsPerFace: ����ÿ������Ƭ�����ǵ����ؼ���Ӧ���������Ƿ���ȷ
// 
//  * executeIntersection: ����ִ�й���Ͷ����������Ƭ�ཻ����Ƿ���ȷ
//  * TestExecuteIntersection_1: ���Ե��Ƶ�����Ƭ����һ����ཻ���
//  * TestExecuteIntersection_2: ���Ե��Ƶ�����Ƭ����������ཻ���
// 
//  * calcTexelColor: 
// 

using namespace hiveObliquePhotography::SceneReconstruction;

const auto PlaneMeshPath = TESTMODEL_DIR + std::string("/Test024_Model/Plane/Plane100.obj");
const auto CloudPath = TESTMODEL_DIR + std::string("");

class TestCastingTextureBaker : public testing::Test
{
protected:
	void SetUp() override
	{
		m_Mesh = _loadMesh(PlaneMeshPath);

		m_pCloud = hiveObliquePhotography::hiveInitPointCloudScene({ CloudPath });

		m_pTextureBaker = _createBaker(m_Mesh);
	}

	void TearDown() override
	{
		delete m_pTextureBaker;
	}

	hiveObliquePhotography::CMesh _loadMesh(const std::string& vPath)
	{
		pcl::TextureMesh TexMesh1, TexMesh2;
		pcl::io::loadOBJFile(vPath, TexMesh1);
		pcl::io::loadPolygonFileOBJ(vPath, TexMesh2);
		TexMesh2.tex_materials = TexMesh1.tex_materials;
		hiveObliquePhotography::CMesh Mesh(TexMesh2);
		bool EmptyFlag = Mesh.m_Vertices.empty() || Mesh.m_Faces.empty();
		EXPECT_FALSE(EmptyFlag);
		if (EmptyFlag)
			std::cerr << "mesh load error." << std::endl;
		return Mesh;
	}

	CRayCastingBaker* _createBaker(const hiveObliquePhotography::CMesh& vMesh)
	{
		auto pBaker =  hiveDesignPattern::hiveCreateProduct<CRayCastingBaker>(KEYWORD::RAYCASTING_TEXTUREBAKER, CSceneReconstructionConfig::getInstance()->getSubConfigByName("RayCasting"), vMesh);
		EXPECT_NE(pBaker, nullptr);
		if (!pBaker)
			std::cerr << "create baker error." << std::endl;
		return pBaker;
	}

	hiveObliquePhotography::CMesh m_Mesh;
	PointCloud_t::Ptr m_pCloud = nullptr;

	CRayCastingBaker* m_pTextureBaker = nullptr;
};

TEST_F(TestCastingTextureBaker, TestFindTexelsPerFace)
{
	//8 faces plane
	m_Mesh = _loadMesh(PlaneMeshPath);
	m_pTextureBaker = _createBaker(m_Mesh);

	std::vector<Eigen::Vector2i> ResolutionList = { {512, 512}, {10, 10} };
	std::vector<int> NumWholeTexels(ResolutionList.size(), 0);
	auto& Vertices = m_Mesh.m_Vertices;
	for (auto& Face : m_Mesh.m_Faces)
	{
		{
			auto& Resolution = ResolutionList[0];
			int NumTexels = Resolution.x() * Resolution.y();
			auto TexelInfos = m_pTextureBaker->findTexelsPerFace(Face, Resolution);
			ASSERT_TRUE(!TexelInfos.empty());
			EXPECT_NEAR(TexelInfos.size(), NumTexels * ((float)1 / 8), NumTexels * 0.1);
			NumWholeTexels[0] += TexelInfos.size();

			for (auto& Texel : TexelInfos)
			{
				Eigen::Vector2f PointUV = { (Texel.TexelPos.x() + 0.5f) / Resolution.x(), (Texel.TexelPos.y() + 0.5f) / Resolution.y() };
				Eigen::Vector2f FacesUV[3];
				for (int i = 0; i < 3; i++)
					FacesUV[i] = { Vertices[Texel.OriginFace[i]].u, Vertices[Texel.OriginFace[i]].v };

				Eigen::Vector2f DeltaPos = { PointUV.x() * 100.0f, -PointUV.y() * 100.0f };
				Eigen::Vector2f BeginPos{ -50.0f, 50.0f };

				Eigen::Vector3f TexelPosInPlane = { BeginPos.x() + DeltaPos.x(), 0.0f, BeginPos.y() + DeltaPos.y() };
				const float ErrorScope = 1.0f;
				for (int i = 0; i < 3; i++)
					ASSERT_NEAR(Texel.TexelPosInWorld.data()[i], TexelPosInPlane.data()[i], ErrorScope);
			}
		}

		{
			auto& Resolution = ResolutionList[1];
			auto TexelInfos = m_pTextureBaker->findTexelsPerFace(Face, Resolution);
			NumWholeTexels[1] += TexelInfos.size();
		}	
	}

	//�ܴ�������Ҫ�ӽ���������
	for (int i = 0; i < ResolutionList.size(); i++)
	{
		auto& Resolution = ResolutionList[i];
		int NumTexels = Resolution.x() * Resolution.y();
		EXPECT_NEAR(NumWholeTexels[i], NumTexels, NumTexels * 0.1);
	}

}

//TEST_F(TestCastingTextureBaker, TestExecuteIntersection_1)
//{
//	auto pCloud = hiveObliquePhotography::hiveInitPointCloudScene({ TESTMODEL_DIR + std::string("/Test024_Model/TestPointCloud.ply") });
//	m_pTextureBaker->setPointCloud(pCloud);
//	
//	STexelInfo TestTexel{ {98,98},{48.0f, 0.0f, 48.0f},2 };
//	auto CandidateSet = m_pTextureBaker->executeIntersection(TestTexel);
//	EXPECT_EQ(CandidateSet.size(), 3);
//	sort(CandidateSet.begin(), CandidateSet.end(), [](SCandidateInfo& vA, SCandidateInfo& vB) {return vA.PointIndex < vB.PointIndex; });
//	Eigen::Vector3f IntersectionOne{ 48.0f, 2.0f, 48.0f };
//	Eigen::Vector3f IntersectionTwo{ 48.0f, 3.0f, 48.0f };
//	EXPECT_EQ(CandidateSet[0].PointIndex, 0);
//	EXPECT_EQ(CandidateSet[0].Pos, IntersectionOne);
//	EXPECT_EQ(CandidateSet[1].PointIndex, 1);
//	EXPECT_EQ(CandidateSet[1].Pos, IntersectionTwo);
//	EXPECT_EQ(CandidateSet[2].PointIndex, 2);
//	EXPECT_EQ(CandidateSet[2].Pos, IntersectionOne);
//}
//
//TEST_F(TestCastingTextureBaker, TestExecuteIntersection_2)
//{
//	auto pCloud = hiveObliquePhotography::hiveInitPointCloudScene({ TESTMODEL_DIR + std::string("/Test024_Model/TestPointCloud.ply") });
//	m_pTextureBaker->setPointCloud(pCloud);
//
//	STexelInfo TestTexel{ {51,51},{1.5f, 0.0f, 1.5f},3 };
//	auto CandidateSet = m_pTextureBaker->executeIntersection(TestTexel);
//	EXPECT_EQ(CandidateSet.size(), 2);
//	sort(CandidateSet.begin(), CandidateSet.end(), [](SCandidateInfo& vA, SCandidateInfo& vB) {return vA.PointIndex < vB.PointIndex; });
//	Eigen::Vector3f IntersectionOne{ 1.5f, 2.0f, 1.5f };
//	Eigen::Vector3f IntersectionTwo{ 1.5f, -2.0f, 1.5f };
//	EXPECT_EQ(CandidateSet[0].PointIndex, 3);
//	EXPECT_EQ(CandidateSet[0].Pos, IntersectionOne);
//	EXPECT_EQ(CandidateSet[1].PointIndex, 4);
//	EXPECT_EQ(CandidateSet[1].Pos, IntersectionTwo);
//}

TEST_F(TestCastingTextureBaker, TestCalcTexelColor)
{

}

