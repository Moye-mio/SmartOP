#include "pch.h"
#include <common/UtilityInterface.h>
#include "Mesh.h"
#include "MeshLoader.h"
#include "MeshSaver.h"

using namespace hiveObliquePhotography;

//���������б�
//  * LoadOBJMesh: �ܹ�����������һ����ʽΪOBJ�������ļ�
//  * LoadIncompleteOBJMesh: �ܹ�����������һ����ʽΪOBJ�Ĳ�����Ϣȱʧ�������ļ�
//  * DeathTest_LoadInexistentMesh: ��������һ�������ڵ��ļ�
//  * DeathTest_LoadUnsupportedMesh: ��������һ����ʽ��֧�ֵ��ļ�
//  * SaveOBJMesh���ܹ������ر���һ����ʽΪOBJ�������ļ���һ����ʽΪMTL�Ĳ����ļ�
//  * SaveIncompleteOBJMesh���ܹ������ر���һ����ʽΪOBJ�Ĳ�����Ϣȱʧ�������ļ������Ĳ���

const std::string g_ValidOBJFilePath = TESTMODEL_DIR + std::string("General/Tile_078-082_low.obj");
const std::string g_ValidIncompleteOBJFilePath = TESTMODEL_DIR + std::string("Test023_Model/Cube.obj");
const std::string g_InexistentFilePath = TESTMODEL_DIR + std::string("Test023_Model/1.obj");
const std::string g_UnsupportedFilePath = TESTMODEL_DIR + std::string("General/Tile_078-082_low.FBX");
const std::string g_ValidSaveOBJFilePath = TESTMODEL_DIR + std::string("Test023_Model/Tile_078-082_low_save.obj");
const std::string g_ValidIncompleteSaveOBJFilePath = TESTMODEL_DIR + std::string("Test023_Model/Cube_save.obj");
const std::string g_ValidSaveMTLFilePath = TESTMODEL_DIR + std::string("Test023_Model/Tile_078-082_low_save.mtl");
const std::string g_ValidIncompleteSaveMTLFilePath = TESTMODEL_DIR + std::string("Test023_Model/Cube_save.mtl");

TEST(Test_LoadMeshModel, LoadOBJMesh)
{
	auto suffix = hiveUtility::hiveGetFileSuffix(g_ValidOBJFilePath);
	auto* pMeshLoader = hiveDesignPattern::hiveGetOrCreateProduct<IMeshLoader>(suffix);
	CMesh Mesh;
	auto result = pMeshLoader->loadDataFromFile(Mesh, g_ValidOBJFilePath);
	EXPECT_EQ(result, true);
	EXPECT_EQ(Mesh.m_Faces.size(), 499999);
	EXPECT_EQ(Mesh.m_Vertices.size(), 252996);
}

TEST(Test_LoadMeshModel, LoadIncompleteOBJMesh)
{
	auto* pMeshLoader = hiveDesignPattern::hiveGetOrCreateProduct<IMeshLoader>(hiveUtility::hiveGetFileSuffix(g_ValidIncompleteOBJFilePath));
	CMesh Mesh;
	EXPECT_NO_THROW(pMeshLoader->loadDataFromFile(Mesh, g_ValidIncompleteOBJFilePath));
	EXPECT_EQ(Mesh.m_Faces.size(), 499999);
	EXPECT_EQ(Mesh.m_Vertices.size(), 252996);
}

TEST(Test_LoadMeshModel, DeathTest_LoadInexistentMesh)
{
	auto* pMeshLoader = hiveDesignPattern::hiveGetOrCreateProduct<IMeshLoader>(hiveUtility::hiveGetFileSuffix(g_InexistentFilePath));
	CMesh Mesh;
	EXPECT_EQ(pMeshLoader->loadDataFromFile(Mesh, g_InexistentFilePath), false);
}

TEST(Test_LoadMeshModel, DeathTest_LoadUnsupportedMesh)
{
	auto* pMeshLoader = hiveDesignPattern::hiveGetOrCreateProduct<IMeshLoader>(hiveUtility::hiveGetFileSuffix(g_UnsupportedFilePath));
	CMesh Mesh;
	EXPECT_EQ(pMeshLoader, nullptr);	//��׺���Դ�������
	//EXPECT_EQ(pMeshLoader->loadDataFromFile(Mesh, g_UnsupportedFilePath), false);
}

TEST(Test_SaveMeshModel, SaveOBJMesh)
{
	auto FileSuffix = hiveUtility::hiveGetFileSuffix(g_ValidOBJFilePath);
	auto* pMeshLoader = hiveDesignPattern::hiveGetOrCreateProduct<IMeshLoader>(FileSuffix);
	CMesh Mesh;
	pMeshLoader->loadDataFromFile(Mesh, g_ValidOBJFilePath);

	auto* pMeshSaver = hiveDesignPattern::hiveGetOrCreateProduct<IMeshSaver>(FileSuffix + "_Save");
	EXPECT_NO_THROW(pMeshSaver->saveDataToFileV(Mesh, g_ValidSaveOBJFilePath));
	EXPECT_EQ(hiveUtility::hiveLocateFile(g_ValidSaveOBJFilePath).empty(), false);
	EXPECT_EQ(hiveUtility::hiveLocateFile(g_ValidSaveMTLFilePath).empty(), false);
	CMesh MeshReLoad;
	pMeshLoader->loadDataFromFile(MeshReLoad, g_ValidSaveOBJFilePath);
	EXPECT_EQ(Mesh.m_Faces.size(), MeshReLoad.m_Faces.size());
	EXPECT_EQ(Mesh.m_Vertices.size(), MeshReLoad.m_Vertices.size());
}

TEST(Test_SaveMeshModel, SaveIncompleteOBJMesh)
{
	auto FileSuffix = hiveUtility::hiveGetFileSuffix(g_ValidIncompleteOBJFilePath);
	auto* pMeshLoader = hiveDesignPattern::hiveGetOrCreateProduct<IMeshLoader>(FileSuffix);
	CMesh Mesh;
	pMeshLoader->loadDataFromFile(Mesh, g_ValidIncompleteOBJFilePath);

	auto* pMeshSaver = hiveDesignPattern::hiveGetOrCreateProduct<IMeshSaver>(FileSuffix + "_Save");
	EXPECT_NO_THROW(pMeshSaver->saveDataToFileV(Mesh, g_ValidIncompleteSaveOBJFilePath));
	EXPECT_EQ(hiveUtility::hiveLocateFile(g_ValidIncompleteSaveOBJFilePath).empty(), false);
	EXPECT_EQ(hiveUtility::hiveLocateFile(g_ValidIncompleteSaveMTLFilePath).empty(), false);
	CMesh MeshReLoad;
	pMeshLoader->loadDataFromFile(MeshReLoad, g_ValidIncompleteSaveOBJFilePath);
	EXPECT_EQ(Mesh.m_Faces.size(), MeshReLoad.m_Faces.size());
	EXPECT_EQ(Mesh.m_Vertices.size(), MeshReLoad.m_Vertices.size());
}