#pragma once
#include <pcl/TextureMesh.h>
#include "ObliquePhotographyDataExport.h"

namespace hiveObliquePhotography
{
	using DataType = float;
	using IndexType = std::uint32_t;
	struct SVertex
	{
		DataType x  = 0;
		DataType y  = 0;
		DataType z  = 0;
		DataType nx = 0;
		DataType ny = 0;
		DataType nz = 0;
		DataType u  = -1;
		DataType v  = -1;

		Eigen::Vector3f xyz() const { return { x, y, z }; }
		Eigen::Vector3f normal() const { return { nx, ny, nz }; }
		Eigen::Vector2f uv() const { return { u, v }; }

		DataType operator[](IndexType i) const
		{
			if (i >= 0 && i < sizeof(SVertex) / sizeof(DataType))
				return *(reinterpret_cast<const DataType*>(this) + i);
			else
				return -1;
		}

		bool operator < (const SVertex& Vertex) const
		{
			if (this->xyz().norm() < Vertex.xyz().norm())
				return true;
			else 
				return false;
		}

		bool operator == (const SVertex& Vertex) const
		{
			for (int i = 0; i < sizeof(SVertex) / sizeof(DataType); i++)
				if ((*this)[i] != Vertex[i])
					return false;
			return true;
		}

		SVertex lerp(const SVertex& vOther, float vMix = 0.5f) const
		{
			using std::lerp;

			SVertex NewVertex;
			NewVertex.x = lerp(x, vOther.x, vMix);
			NewVertex.y = lerp(y, vOther.y, vMix);
			NewVertex.z = lerp(z, vOther.z, vMix);
			NewVertex.nx = lerp(nx, vOther.nx, vMix);
			NewVertex.ny = lerp(ny, vOther.ny, vMix);
			NewVertex.nz = lerp(nz, vOther.nz, vMix);
			NewVertex.u = lerp(u, vOther.u, vMix);
			NewVertex.v = lerp(v, vOther.v, vMix);
			return NewVertex;
		}
	};

	struct SFace
	{
		IndexType a;
		IndexType b;
		IndexType c;

		IndexType& operator[](IndexType i)
		{
			if (i >= 0 && i < sizeof(SFace) / sizeof(IndexType))
				return *(reinterpret_cast<IndexType*>(this) + i);
			else
				throw("[] out of range.");
		}

		const IndexType& operator[](IndexType i) const
		{
			if (i >= 0 && i < sizeof(SFace) / sizeof(IndexType))
				return *(reinterpret_cast<const IndexType*>(this) + i);
			else
				throw("[] out of range.");
		}

		IndexType& at(IndexType i) { return operator[](i); }
		const IndexType& at(IndexType i) const { return operator[](i); }
	};

	class OPDATA_DECLSPEC CMesh
	{
	public:
		CMesh() = default;
		CMesh(const pcl::PolygonMesh& vPolMesh);
		CMesh(const pcl::TextureMesh& vTexMesh);
		CMesh(const Eigen::MatrixXd& vVertexMatrix, const Eigen::MatrixXi& vFaceMatrix);

		pcl::PolygonMesh toPolMesh() const;
		pcl::TextureMesh toTexMesh(const pcl::TexMaterial& vMaterial) const;

		std::vector<SFace> findFacesByVertex(IndexType vVertex) const;
		std::pair<Eigen::Vector3f, Eigen::Vector3f> calcAABB() const;
		void calcModelPlaneAxis(std::pair<int, int>& vUV, int& vHeight) const;
		void setMaterial(const pcl::TexMaterial& vMaterial) { m_Material = vMaterial; }
		void saveMaterial(const std::string& vPath) const;
		const std::string& getMaterialName() const { return m_Material.tex_name; }
		
		Eigen::MatrixXd getVerticesMatrix() const;
		Eigen::MatrixXi getFacesMatrix() const;

		std::vector<SVertex> m_Vertices;
		std::vector<SFace> m_Faces;

	private:
		void __fillVertices(std::vector<SVertex>& vVertices, const pcl::PolygonMesh& vPolMesh) const;
		void __fillVertices(std::vector<SVertex>& vVertices, const pcl::TextureMesh& vTexMesh) const;
		void __fillVertices(std::vector<SVertex>& vVertices, const Eigen::MatrixXd& vVerticesMatrix) const;
		std::map<std::uint32_t, std::uint32_t> __getOffsetTable(const std::vector<pcl::PCLPointField>& vVertexAttributes) const;
		void __copyAttributes(std::vector<SVertex>& vVertices, const std::vector<uint8_t>& vData, const std::map<uint32_t, uint32_t>& vOffsetTable, int vPointStep) const;

		void __fillFaces(std::vector<SFace>& vFaces, const std::vector<pcl::Vertices>& vFaceData) const;
		void __fillFaces(std::vector<SFace>& vFaces, const Eigen::MatrixXi& vFacesMatrix) const;
		
		void __fillCloud(const std::vector<SVertex>& vVertices, pcl::PCLPointCloud2& vCloud) const;
		void __fillPolygons(const std::vector<SFace>& vFaces, std::vector<pcl::Vertices>& vPolygons) const;

		pcl::TexMaterial m_Material;
	};

	inline SVertex lerp(const SVertex& vLhs, const SVertex& vRhs, float vMix = 0.5f)
	{
		return vLhs.lerp(vRhs, vMix);
	}
}
