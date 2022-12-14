#pragma once

namespace hiveObliquePhotography
{
	namespace PointCloudRetouch
	{
		class CNeighborhood;   //FIXME-014：这个在什么地方用了？
		class CPointLabelSet;

		class INeighborhoodBuilder : public hiveDesignPattern::IProduct
		{
		public:
			INeighborhoodBuilder() = default;
			virtual ~INeighborhoodBuilder();

			virtual bool onProductCreatedV(const hiveConfig::CHiveConfig* vConfig, const std::vector<PointCloud_t::Ptr>& vTileSet, const CPointLabelSet* vPointLabelSet);

			std::vector<pcl::index_t> buildNeighborhood(pcl::index_t vSeed, std::string& vType, float vPara) const;  //FIXME-014：对任何一种建立邻域的方法，如果要传入参数，只需要传入一个参数vPara就可以？
			std::vector<pcl::index_t> buildNeighborhood(pcl::index_t vSeed) const;
			virtual void reset() {};

		protected:
			std::vector<std::size_t> m_OffsetSet;
			std::vector<PointCloud_t::Ptr> m_TileSet;

		private:
			const CPointLabelSet* m_pPointLabelSet = nullptr;
			std::size_t m_NumPoints = 0;

			virtual void __extraInitV(const hiveConfig::CHiveConfig* vConfig) {}
			virtual std::vector<pcl::index_t> __buildNeighborhoodV(pcl::index_t vSeed, std::string& vType, float vPara) const = 0;  
			virtual std::vector<pcl::index_t> __buildNeighborhoodV(pcl::index_t vSeed) const = 0;
		};
	}
}