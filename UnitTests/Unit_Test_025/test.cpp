#include "pch.h"
#include "Image.h"

using hiveObliquePhotography::CColorTraits;

//ע������ʧ�ܵĲ������������ڱ����ڼ�����������޷�ͨ������
//���ｫ��Щ��������ע��

//���������б�
//  * GeneralTypeTest: �ܹ���ȷ����������ɫ����
//  * UnboundedTypeTest: ���Խ����䳤����ɫ���ͣ�ͨ�����᷵��0
//  * StructTypeTest: ���Խ����ṹ�������ɫ����
//  * DerivedTypeTest: ���Խ�����������ɫ���ͣ���cv��&*
//  * IllegalTypeTest: ���Խ��������Ƿ��ĵ���ɫ���ͣ���void��enum

template <class ColorType, class ScalarType, size_t ChannelNum>
void EXPECT_COLOR_TRAITS(int vLine)
{
	EXPECT_EQ(typeid(CColorTraits<ColorType>::Scalar_t).hash_code(), typeid(ScalarType).hash_code());
	if (typeid(CColorTraits<ColorType>::Scalar_t).hash_code() != typeid(ScalarType).hash_code())
		std::cout << "Line " << vLine << ": " << typeid(ColorType).name() << std::endl;

	EXPECT_EQ(CColorTraits<ColorType>::extractChannel(), ChannelNum);
	if (CColorTraits<ColorType>::extractChannel() != ChannelNum)
		std::cout << "Line " << vLine << ": " << typeid(ColorType).name() << std::endl;
}

TEST(Test_ColorTraits, GeneralTypeTest)
{
	EXPECT_COLOR_TRAITS<int, int, 1>(__LINE__);
	EXPECT_COLOR_TRAITS<int[3], int, 3>(__LINE__);
	EXPECT_COLOR_TRAITS<std::array<int, 3>, int, 3>(__LINE__);
	EXPECT_COLOR_TRAITS<Eigen::Vector3i, int, 3>(__LINE__);
	EXPECT_COLOR_TRAITS<Eigen::RowVector3i, int, 3>(__LINE__);
}

TEST(Test_ColorTraits, UnboundedTypeTest)
{
	//EXPECT_COLOR_TRAITS<int[], int, 0>(__LINE__);
	EXPECT_COLOR_TRAITS<std::vector<int>, int, 0>(__LINE__);
	EXPECT_COLOR_TRAITS<Eigen::VectorXi, int, 0>(__LINE__);
}

struct SRgb
{
	using value_type = int;
	int R, G, B;
};
struct SRgbFloat
{
	using value_type = int;
	int R, G, B;
	float A;
};
struct SRgbNoValueType
{
	int R, G, B;
};

struct SRgbWrongValueType
{
	using value_type = double;
	int R, G, B;
};
struct SRgbVariablelValueType
{
	int R, G, B, value_type;
};
struct SRgbVoidValueType
{
	using value_type = void;
	int R, G, B;
};
struct SVoid
{
	using value_type = int;
};

TEST(Test_ColorTraits, StructTypeTest)
{
	EXPECT_COLOR_TRAITS<SRgb, int, 3>(__LINE__);
	//EXPECT_COLOR_TRAITS<SRgbFloat, int, 0>(__LINE__);
	//EXPECT_COLOR_TRAITS<SRgbNoValueType, void, 0>(__LINE__);
	//EXPECT_COLOR_TRAITS<SRgbWrongValueType, double, 0>(__LINE__); //TODO����ʱ�޷�����������
	//EXPECT_COLOR_TRAITS<SRgbVariablelValueType, void, 0>(__LINE__);
	//EXPECT_COLOR_TRAITS<SRgbVoidValueType, void, 0>(__LINE__);
	//EXPECT_COLOR_TRAITS<SVoid, int, 0>(__LINE__);
}

TEST(Test_ColorTraits, DerivedTypeTest)
{
	//EXPECT_COLOR_TRAITS<SRgb&, int, 0>(__LINE__);
	//EXPECT_COLOR_TRAITS<SRgb&&, int, 0>(__LINE__);
	//EXPECT_COLOR_TRAITS<SRgb*, void, 0>(__LINE__);
	EXPECT_COLOR_TRAITS<const SRgb, int, 3>(__LINE__);
	//EXPECT_COLOR_TRAITS<volatile SRgb, void, 0>(__LINE__);
	//EXPECT_COLOR_TRAITS<const volatile SRgb, void, 0>(__LINE__);
}

enum EEnum
{
	R, G, B
};
enum class EEnumClass
{
	R, G, B
};

TEST(Test_ColorTraits, IllegalTypeTest)
{
	//EXPECT_COLOR_TRAITS<void, void, 0>(__LINE__);
	//EXPECT_COLOR_TRAITS<EEnum, void, 0>(__LINE__);
	//EXPECT_COLOR_TRAITS<EEnumClass, void, 0>(__LINE__);
}