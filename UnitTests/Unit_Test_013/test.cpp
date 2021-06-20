#include "pch.h"
#include "PointCloudRetouchInterface.h"
#include "PointLabelSet.h"

//	���������б�
//	* Uninitialized_Test������δ��ʼ��ʱ�ܷ���ȷ����������ܷ������ȷ����
//	* Illegal_Input_Test����������Ƿ�ʱ�ܷ���ȷ����������ܷ������ȷ����
//	* ����CPointLabelSetΪ�����࣬����û��Ҫ���й����ϵĲ���

using namespace hiveObliquePhotography::PointCloudRetouch;

TEST(TestPointLabelSet, Uninitialized_Test)
{
	CPointLabelSet PointLabelSet;

	EXPECT_ANY_THROW(PointLabelSet.tagPointLabel(999, {}, 999, {}));
	EXPECT_ANY_THROW(PointLabelSet.tagCoreRegion4Cluster({ 999 }, {}, 999));
	EXPECT_ANY_THROW(PointLabelSet.getLabelAt(999));
	EXPECT_ANY_THROW(PointLabelSet.getClusterIndexAt(999));
	EXPECT_ANY_THROW(PointLabelSet.getClusterBelongingProbabilityAt(999));

	EXPECT_NO_THROW(PointLabelSet.init(100));
	EXPECT_NO_THROW(PointLabelSet.tagPointLabel(50, {}, 50, 0.5));
	EXPECT_NO_THROW(PointLabelSet.tagCoreRegion4Cluster({ 50 }, {}, 50));
	EXPECT_NO_THROW(PointLabelSet.getLabelAt(50));
	EXPECT_NO_THROW(PointLabelSet.getClusterIndexAt(50));
	EXPECT_NO_THROW(PointLabelSet.getClusterBelongingProbabilityAt(50));
}

TEST(TestPointLabelSet, Illegal_Input_Test)
{
	CPointLabelSet PointLabelSet;
	EXPECT_ANY_THROW(PointLabelSet.init(-1));
	EXPECT_NO_THROW(PointLabelSet.init(100));

	EXPECT_ANY_THROW(PointLabelSet.tagPointLabel(-1, {}, -1, 0.5));
	EXPECT_ANY_THROW(PointLabelSet.tagPointLabel(999, {}, 999, 0.5));
	EXPECT_ANY_THROW(PointLabelSet.tagPointLabel(50, {}, 50, -1));
	EXPECT_ANY_THROW(PointLabelSet.tagPointLabel(50, {}, 50, 10));
	EXPECT_ANY_THROW(PointLabelSet.tagCoreRegion4Cluster({}, {}, 50));
	EXPECT_ANY_THROW(PointLabelSet.tagCoreRegion4Cluster({ -1 }, {}, -1));
	EXPECT_ANY_THROW(PointLabelSet.tagCoreRegion4Cluster({ 999 }, {}, 999));
	EXPECT_ANY_THROW(PointLabelSet.getLabelAt(-1));
	EXPECT_ANY_THROW(PointLabelSet.getLabelAt(999));
	EXPECT_ANY_THROW(PointLabelSet.getClusterIndexAt(-1));
	EXPECT_ANY_THROW(PointLabelSet.getClusterIndexAt(999));
	EXPECT_ANY_THROW(PointLabelSet.getClusterBelongingProbabilityAt(-1));
	EXPECT_ANY_THROW(PointLabelSet.getClusterBelongingProbabilityAt(999));

	EXPECT_NO_THROW(PointLabelSet.tagPointLabel(50, {}, 50, 0.5));
	EXPECT_NO_THROW(PointLabelSet.tagCoreRegion4Cluster({ 50 }, {}, 50));
	EXPECT_NO_THROW(PointLabelSet.getLabelAt(50));
	EXPECT_NO_THROW(PointLabelSet.getClusterIndexAt(50));
	EXPECT_NO_THROW(PointLabelSet.getClusterBelongingProbabilityAt(50));
}
