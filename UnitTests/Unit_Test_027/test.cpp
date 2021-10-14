#include "pch.h"

#include "SceneReconstructionConfig.h"
#include "MeshPlaneIntersection.h"
#include "ObliquePhotographyDataInterface.h"

using namespace hiveObliquePhotography::SceneReconstruction;

//���������б�
//  * Test_NoIntersection: ����ƽ���������ཻ
//  * Test_SeveralIntersections: ����ƽ���������ж�����㣨�ڶ��㴦�ཻ���ڱߴ��ཻ��
//  * Test_TerrainModelIntersections: ����ƽ�����ض����ӵ��������ཻ
//  * Test_SortByVertexLoop: ���������loop������ȷ��

const auto SimpleMeshPath = TESTMODEL_DIR + std::string("Test027_Model/SimpleMesh.obj");
const auto SceneMeshPath = TESTMODEL_DIR + std::string("Test027_Model/005006.obj");

class TestMeshPlaneIntersection : public testing::Test
{
protected:
	void SetUp() override
	{
	}

	void LoadMesh(const std::string& vPath)
	{
		hiveObliquePhotography::hiveLoadMeshModel(m_Mesh, vPath);
	}

	hiveObliquePhotography::CMesh m_Mesh;
};

TEST_F(TestMeshPlaneIntersection, Test_NoIntersection)
{
	LoadMesh(SimpleMeshPath);

	hiveObliquePhotography::CMesh Mesh;
	Eigen::Vector4f Plane(1.0, 0.0, 0.0, 3.0);
	std::vector<hiveObliquePhotography::SVertex> IntersectionPoints;
	CMeshPlaneIntersection MeshPlaneIntersection;
	Mesh = m_Mesh;
	MeshPlaneIntersection.execute(Mesh, Plane);

	MeshPlaneIntersection.dumpIntersectionPoints(IntersectionPoints);
	EXPECT_EQ(IntersectionPoints.size(), 0);
}

TEST_F(TestMeshPlaneIntersection, Test_SeveralIntersections)
{
	LoadMesh(SimpleMeshPath);
	hiveObliquePhotography::CMesh Mesh;
	std::vector<hiveObliquePhotography::SVertex> IntersectionPoints;
	CMeshPlaneIntersection MeshPlaneIntersection;

	Eigen::Vector4f PlaneWithOneIntersection(1.0, 0.0, 0.0, -2.0);
	Mesh = m_Mesh;
	MeshPlaneIntersection.execute(Mesh, PlaneWithOneIntersection);
	MeshPlaneIntersection.dumpIntersectionPoints(IntersectionPoints);
	EXPECT_EQ(IntersectionPoints.size(), 1);

	Eigen::Vector4f PlaneWithSeveralIntersections(1.0, 0.0, 0.0, -1.5);
	Mesh = m_Mesh;
	MeshPlaneIntersection.execute(Mesh, PlaneWithSeveralIntersections);
	MeshPlaneIntersection.dumpIntersectionPoints(IntersectionPoints);
	EXPECT_EQ(IntersectionPoints.size(), 2);

	Eigen::Vector4f PlaneWithSeveralIntersectionsOverlapWithOrigin(-1.0, 0.0, 0.0, 0.0);
	Mesh = m_Mesh;
	MeshPlaneIntersection.execute(Mesh, PlaneWithSeveralIntersectionsOverlapWithOrigin);
	MeshPlaneIntersection.dumpIntersectionPoints(IntersectionPoints);
	EXPECT_EQ(IntersectionPoints.size(), 3);

}

TEST_F(TestMeshPlaneIntersection, Test_TerrainModel)
{
	LoadMesh(SceneMeshPath);
	hiveObliquePhotography::CMesh Mesh;
	std::vector<hiveObliquePhotography::SVertex> IntersectionPoints;
	CMeshPlaneIntersection MeshPlaneIntersection;

	Eigen::Vector4f Plane(1.0, 0.0, 0.0, 132.0);
	Mesh = m_Mesh;
	MeshPlaneIntersection.execute(Mesh, Plane);
	MeshPlaneIntersection.dumpIntersectionPoints(IntersectionPoints);
	EXPECT_EQ(IntersectionPoints.size(), 205);
}

TEST_F(TestMeshPlaneIntersection, Test_SortByVertexLoop)
{
	LoadMesh(SimpleMeshPath);
	hiveObliquePhotography::CMesh Mesh;
	CMeshPlaneIntersection MeshPlaneIntersection;
	std::vector<int> DissociatedIndices{ 0,3,4,2 };
	std::vector<hiveObliquePhotography::SVertex> VertexSet;
	for (auto Index : DissociatedIndices)
		VertexSet.push_back(m_Mesh.m_Vertices[Index]);
	MeshPlaneIntersection.sortByVertexLoop(DissociatedIndices, VertexSet);
	if(DissociatedIndices[0] == 0)
	{
		EXPECT_EQ(DissociatedIndices[1], 2);
		EXPECT_EQ(DissociatedIndices[2], 3);
		EXPECT_EQ(DissociatedIndices[3], 4);
	}
	else
	{
		EXPECT_EQ(DissociatedIndices[1], 3);
		EXPECT_EQ(DissociatedIndices[2], 2);
		EXPECT_EQ(DissociatedIndices[3], 0);	
	}
}
