#pragma once

namespace hiveObliquePhotography
{
	namespace PointCloudRetouch
	{
		class CBoundaryDetector;

		struct SLattice
		{
			std::vector<int> Indices;
			Eigen::Vector3f CenterPos{ 0.0f, 0.0f, 0.0f };
			Eigen::Vector3i Color{ 0, 0, 0 };
			Eigen::Matrix<float, 1, 1> Height{ 0.0f };
		};

		struct SPlaneInfos
		{
			Eigen::Vector3f Normal;
			std::tuple<Eigen::Matrix3f, Eigen::Vector3f, Eigen::Vector3f> BoundingBox;
			Eigen::Vector3f PlaneCenter;
			Eigen::Vector2f LatticeSize;
			std::vector<std::size_t> AxisOrder;
		};

		class CHoleRepairer
		{
		public:
			CHoleRepairer() = default;
			~CHoleRepairer() = default;

			bool init(const hiveConfig::CHiveConfig* vConfig);

			void setHoleRegion(const std::vector<pcl::index_t>& vHoleRegion);
			void setInput(const std::vector<pcl::index_t>& vInputIndices) { m_Input = vInputIndices; }
			void repairHole(std::vector<pcl::PointXYZRGBNormal>& voNewPoints);

			void repairHoleByBoundaryAndInput(const std::vector<pcl::index_t>& vBoundaryIndices, const std::vector<pcl::index_t>& vInputIndices, std::vector<pcl::PointXYZRGBNormal>& voNewPoints);

#ifdef _UNIT_TEST
			Eigen::Vector4f calcPlane(const std::vector<pcl::index_t>& vIndices, Eigen::Matrix3f& vRotationMatrix) { return __calculatePlaneByIndices(vIndices, vRotationMatrix); }
			void generateLattices(const Eigen::Vector4f& vPlane, const std::tuple<Eigen::Matrix3f, Eigen::Vector3f, Eigen::Vector3f>& vBox, const Eigen::Vector2i& vResolution, SPlaneInfos& voPlaneInfos, std::vector<std::vector<SLattice>>& voPlaneLattices) { __generatePlaneLattices(vPlane, vBox, vResolution, voPlaneInfos, voPlaneLattices); }
			void projectPoints(const std::vector<pcl::index_t>& vIndices, SPlaneInfos& vPlaneInfos, std::vector<std::vector<SLattice>>& vioPlaneLattices) { __projectPoints2PlaneLattices(vIndices, vPlaneInfos, vioPlaneLattices); }
			void outputImage(const Eigen::Matrix<Eigen::Vector3i, -1, -1>& vTexture, const std::string& vOutputImagePath) { __outputImage(vTexture, vOutputImagePath); }
			void outputImage(const Eigen::MatrixXi& vTexture, const std::string& vOutputImagePath) { __outputImage(vTexture, vOutputImagePath); }
			void outputImage(const Eigen::Matrix<Eigen::Matrix<float, 1, 1>, -1, -1>& vTexture, const std::string& vOutputImagePath) { __outputImage(vTexture, vOutputImagePath); }
			template<class T>
			Eigen::Matrix<T, -1, -1> extractMatrix(const std::vector<std::vector<SLattice>>& vLattices, int vOffset) { return __extractMatrixFromLattices<T>(vLattices, vOffset); }
#endif // _UNIT_TEST

		private:
			void __generatePlaneLattices(const Eigen::Vector4f& vPlane, const std::tuple<Eigen::Matrix3f, Eigen::Vector3f, Eigen::Vector3f>& vBox, const Eigen::Vector2i& vResolution, SPlaneInfos& voPlaneInfos, std::vector<std::vector<SLattice>>& voPlaneLattices);
			void __projectPoints2PlaneLattices(const std::vector<pcl::index_t>& vIndices, SPlaneInfos& vioPlaneInfos, std::vector<std::vector<SLattice>>& vioPlaneLattices);
			void __fillLatticesOriginInfos(const Eigen::Vector3f& vPlaneNormal, std::vector<std::vector<SLattice>>& vioPlaneLattices);
			void  __fixTextureColorAndHeight(std::vector<std::vector<SLattice>>& vioPlaneLattices, int vKernelSize);
			void __inputHeightCorrection(std::vector<std::vector<SLattice>>& vInput, const std::vector<std::vector<SLattice>>& vBoundary);
			void __generateNewPointsFromLattices(const Eigen::Vector3f& vPlaneNormal, const Eigen::MatrixXi& vMask, const std::vector<std::vector<SLattice>>& vPlaneLattices, std::vector<pcl::PointXYZRGBNormal>& voNewPoints);

			Eigen::Vector4f __calculatePlaneByIndices(const std::vector<pcl::index_t>& vIndices, Eigen::Matrix3f& vRotationMatrix);
			std::vector<std::size_t> __calcAxisOrder(const Eigen::Vector4f& vPlane);
			void __gaussBlurbyHeightMatrix(const Eigen::Matrix<Eigen::Matrix<float, 1, 1>, -1, -1>& vHeightMatrix, std::vector<std::vector<SLattice>>& vPlaneLattices);
			Eigen::MatrixXi __genMask(const Eigen::Vector2i& vResolution, const std::vector<std::vector<SLattice>>& vPlaneLattices);
			void __restoreWorldCoord(SPlaneInfos& vioPlaneInfos, std::vector<std::vector<SLattice>>& vioPlaneLattices, Eigen::Matrix3f& vRotationMatrix);

			template<class T>
			Eigen::Matrix<T, -1, -1> __extractMatrixFromLattices(const std::vector<std::vector<SLattice>>& vLattices, int vOffset)
			{
				_ASSERTE(!vLattices.empty());
				Eigen::Vector2i Resolution{ vLattices.front().size(), vLattices.size() };
				Eigen::Matrix<T, -1, -1> Matrix;
				Matrix.resize(Resolution.y(), Resolution.x());

				for (int Y = 0; Y < Resolution.y(); Y++)
				{
					for (int X = 0; X < Resolution.x(); X++)
					{
						const SLattice& Lattice = vLattices[Y][X];
						void* Ptr = (bool*)(&Lattice) + vOffset;
						T* Item = static_cast<T*>(Ptr);
						Matrix(Y, X) = *Item;
					}
				}

				return Matrix;
			}
			template<class T>
			void __fillLatticesByMatrix(const Eigen::Matrix<T, -1, -1>& vMatrix, std::vector<std::vector<SLattice>>& vLattices, int vOffset)
			{
				Eigen::Vector2i Resolution{ vLattices.front().size(), vLattices.size() };
				_ASSERTE(vMatrix.cols() == Resolution.x() && vMatrix.rows() == Resolution.y());

				for (int Y = 0; Y < Resolution.y(); Y++)
				{
					for (int X = 0; X < Resolution.x(); X++)
					{
						SLattice& Lattice = vLattices[Y][X];
						void* LatticePtr = (bool*)(&Lattice) + vOffset;
						T* ItemInLattice = static_cast<T*>(LatticePtr);
						*ItemInLattice = vMatrix(Y, X);
					}
				}
			}

			void __outputImage(const Eigen::Matrix<Eigen::Vector3i, -1, -1>& vTexture, const std::string& vOutputImagePath);
			void __outputImage(const Eigen::MatrixXi& vTexture, const std::string& vOutputImagePath);
			void __outputImage(const Eigen::Matrix<Eigen::Matrix<float, 1, 1>, -1, -1>& vTexture, const std::string& vOutputImagePath);
			void __reset();

			std::vector<std::vector<pcl::index_t>> m_BoundarySet;
			std::vector<pcl::index_t> m_Input;

			CBoundaryDetector* m_pBoundaryDetector = nullptr;
			const hiveConfig::CHiveConfig* m_pConfig = nullptr;
			const hiveConfig::CHiveConfig* m_pTextureConfig = nullptr;
		};
	}
}
