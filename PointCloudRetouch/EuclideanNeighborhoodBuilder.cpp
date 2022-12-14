#include "pch.h"
#include "EuclideanNeighborhoodBuilder.h"
#include "PointCloudRetouchManager.h"   //FIXME-014: 用到这个头文件了吗？

using namespace hiveObliquePhotography::PointCloudRetouch;

_REGISTER_NORMAL_PRODUCT(CEuclideanNeighborhoodBuilder, KEYWORD::EUCLIDEAN_NEIGHBOR_BUILDER)

//*****************************************************************
//FUNCTION: 
void CEuclideanNeighborhoodBuilder::__extraInitV(const hiveConfig::CHiveConfig* vConfig)
{
	m_TreeSet.resize(m_TileSet.size());
	for (int i = 0; i < m_TileSet.size(); i++)
	{
		m_TreeSet[i] = std::make_shared<pcl::search::KdTree<PointCloud_t::PointType>>();;
		m_TreeSet[i]->setInputCloud(m_TileSet[i]);
	}

	m_SearchMode = *vConfig->getAttribute<std::string>("SEARCH_MODE");
	m_NearestN = *vConfig->getAttribute<int>("NEAREST_N");
	m_Radius = *vConfig->getAttribute<float>("RADIUS");
}

//*****************************************************************
//FUNCTION: 
std::vector<pcl::index_t> CEuclideanNeighborhoodBuilder::__buildNeighborhoodV(pcl::index_t vSeed, std::string& vType, float vPara) const  //FIXME-014：两个_buildNeighborhoodV()函数copy/paste的吧，有必要做两个虚函数吗？
{//FIXME-014：输入参数vType为什么不加const修饰，你要改它吗？
//FIXME-014：为什么vType要用字符串？大小写敏感吗？传入了非法字符串怎么办？
//FIXME-014：为什么不对m_pTree做有效性检查？
//FIXME-014: 为什么在基类采用了工厂模式后，这里还要vType这个参数？
	std::vector<pcl::index_t> Neighborhood;
	std::vector<float> Distance;
	int WhichTile = 0;
	while (WhichTile + 1 < m_OffsetSet.size() && m_OffsetSet[WhichTile + 1] <= vSeed)
		WhichTile++;
	vSeed -= m_OffsetSet[WhichTile];

	if (vType == "NEAREST")
		m_TreeSet[WhichTile]->nearestKSearch(m_TileSet[WhichTile]->points[vSeed], static_cast<int>(vPara), Neighborhood, Distance);
	else if (vType == "RADIUS")
		m_TreeSet[WhichTile]->radiusSearch(m_TileSet[WhichTile]->points[vSeed], vPara, Neighborhood, Distance);
	//发生NRVO
	for (auto& Index : Neighborhood)
		Index += m_OffsetSet[WhichTile];
	return Neighborhood;
}

//*****************************************************************
//FUNCTION: 
std::vector<pcl::index_t> CEuclideanNeighborhoodBuilder::__buildNeighborhoodV(pcl::index_t vSeed) const
{
	std::vector<pcl::index_t> Neighborhood;
	std::vector<float> Distance;
	int WhichTile = 0;
	while (WhichTile + 1 < m_OffsetSet.size() && m_OffsetSet[WhichTile + 1] <= vSeed)
		WhichTile++;
	vSeed -= m_OffsetSet[WhichTile];

	if (m_SearchMode == "NEAREST")
		m_TreeSet[WhichTile]->nearestKSearch(m_TileSet[WhichTile]->points[vSeed], m_NearestN, Neighborhood, Distance);
	else if (m_SearchMode == "RADIUS")
		m_TreeSet[WhichTile]->radiusSearch(m_TileSet[WhichTile]->points[vSeed], m_Radius, Neighborhood, Distance);
	//发生NRVO
	for (auto& Index : Neighborhood)
		Index += m_OffsetSet[WhichTile];
	return Neighborhood;
}

//*****************************************************************
//FUNCTION: 
void CEuclideanNeighborhoodBuilder::reset()
{
	m_OffsetSet.clear();
	m_OffsetSet.shrink_to_fit();
	m_TileSet.clear();
	m_TileSet.shrink_to_fit();
	m_TreeSet.clear();
	m_TreeSet.shrink_to_fit();
}