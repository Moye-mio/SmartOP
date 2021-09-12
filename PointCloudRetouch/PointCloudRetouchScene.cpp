#include "pch.h"
#include "PointCloudRetouchScene.h"

using namespace hiveObliquePhotography::PointCloudRetouch;

CPointCloudScene::CPointCloudScene() 
{

}

CPointCloudScene::~CPointCloudScene()
{
}

//*****************************************************************
//FUNCTION: 
void CPointCloudScene::init(PointCloud_t::Ptr vPointCloudScene)
{
	//_ASSERTE(vPointCloudScene);

//FIXME-010��������߼���������init()�������������Ҫ��֤����init()��ʱ��m_pPointCloudSceneΪ��

	if (vPointCloudScene == nullptr)   //FIXME-010: Ϊʲô����Բ�����Ч�Լ�鲻��_ASSERTE����Ҫ���׳��쳣�ķ�ʽ����ʼ��ʧ�ܷ���false�ͺã�ΪʲôҪ���쳣��
		throw "vPointCloudScene is nullptr or undefined!";

	m_pPointCloudScene = vPointCloudScene;
}

//*****************************************************************
//FUNCTION: 
Eigen::Vector4f CPointCloudScene::getPositionAt(pcl::index_t vIndex) const
{
	_ASSERTE(vIndex < m_pPointCloudScene->size());  //FIXME-010: ����Ҫ��m_pPointCloudScene����Ч�������
	return { m_pPointCloudScene->points[vIndex].x, m_pPointCloudScene->points[vIndex].y, m_pPointCloudScene->points[vIndex].z, 1.0 };
}

//*****************************************************************
//FUNCTION: 
Eigen::Vector4f CPointCloudScene::getNormalAt(pcl::index_t vIndex) const
{//FIXME-010: Ϊʲô������Ҫ������ά������
	_ASSERTE(vIndex < m_pPointCloudScene->size());
	return { m_pPointCloudScene->points[vIndex].normal_x, m_pPointCloudScene->points[vIndex].normal_y, m_pPointCloudScene->points[vIndex].normal_z, 0.0 };
}

//*****************************************************************
//FUNCTION: 
Eigen::Vector3i CPointCloudScene::getColorAt(pcl::index_t vIndex) const
{
	_ASSERTE(vIndex < m_pPointCloudScene->size());
	return __extractRgba(m_pPointCloudScene->points[vIndex].rgb);
}

//*****************************************************************
//FUNCTION: 
std::pair<Eigen::Vector3f, Eigen::Vector3f> CPointCloudScene::getBoundingBox(const std::vector<pcl::index_t>& vIndices) const
{//FIXME-010����������Ĺ�����compute������get
	Eigen::Vector3f Min{ FLT_MAX, FLT_MAX, FLT_MAX };
	Eigen::Vector3f Max{ -FLT_MAX, -FLT_MAX, -FLT_MAX };

	auto update = [&](const Eigen::Vector4f& vPos)  //FIXME-010��ΪʲôҪ������ά������
	{
		for (int i = 0; i < 3; i++)
		{
			if (vPos.data()[i] < Min.data()[i]) Min.data()[i] = vPos.data()[i];
			if (vPos.data()[i] > Max.data()[i]) Max.data()[i] = vPos.data()[i];
		}
	};

	if (vIndices.empty())
	{
		Eigen::Vector4f Pos;
		for (auto& Point : *m_pPointCloudScene)
		{
			Pos.x() = Point.x;
			Pos.y() = Point.y;
			Pos.z() = Point.z;
			update(Pos);
		}
	}
	else
	{
		for (auto& Index : vIndices) update(getPositionAt(Index));
	}

	return { Min, Max };
}

//*****************************************************************
//FUNCTION: 
std::vector<pcl::index_t> CPointCloudScene::getPointsInBox(const std::pair<Eigen::Vector3f, Eigen::Vector3f>& vBox, const Eigen::Matrix3f& vRotationMatrix) const
{
	std::vector<pcl::index_t> TempPoints;  //FIXME-010����������� PointsInBox ���Ǹ�����
	for (auto Index = 0; Index < m_pPointCloudScene->size(); Index++)
	{
		auto& Point = m_pPointCloudScene->points[Index];
		Eigen::Vector3f Pos{ Point.x, Point.y, Point.z };  //FIXME-010��ȷ�������ʱ�����᲻����ÿ��ѭ���󶼱�������debug��release��ʽ�¶�Ҫ���ԣ������Ҫ��Ƶ�����������������Ҫ�ŵ�ѭ������
		Pos = vRotationMatrix * Pos;
		
		int i = 0;
		for (i = 0; i < 3; i++)
		{
			if (Pos.data()[i] < vBox.first.data()[i] || Pos.data()[i] > vBox.second.data()[i]) break;
		}
		if (i == 3) TempPoints.push_back(Index);
	}
	return TempPoints;
}

//*****************************************************************
//FUNCTION: 
Eigen::Vector3i CPointCloudScene::__extractRgba(float vRgba) const
{//FIXME-010: ������˵Ҫ��ȡrgb�������ص���rgb���������
	union ColorLayout
	{
		struct
		{
			std::uint8_t b;
			std::uint8_t g;
			std::uint8_t r;
			std::uint8_t a;
		};
		float rgb;
	};
	ColorLayout Color;  
	Color.rgb = vRgba;    //FIXME-010��������ɫ��ת����������֣�����˵���ĵ�

	Eigen::Vector3i Rgb;
	Rgb.x() = Color.r;
	Rgb.y() = Color.g;
	Rgb.z() = Color.b;

	return Rgb;
}
