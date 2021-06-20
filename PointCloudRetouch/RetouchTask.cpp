#include "pch.h"
#include "RetouchTask.h"
#include "PointClusterExpander.h"

using namespace hiveObliquePhotography::PointCloudRetouch;

//*****************************************************************
//FUNCTION: 
bool CRetouchTask::init(const hiveConfig::CHiveConfig* vConfig)
{
	_ASSERTE(vConfig);
	m_pConfig = vConfig;

//TODO: ���������ļ�ָ���ķ�������ǩ����ʹ�ù���ģʽ��������Ӧ���󲢳�ʼ��

	return true;
}

//*****************************************************************
//FUNCTION: 
const hiveConfig::CHiveConfig* CRetouchTask::getClusterConfig() const
{
	return m_pConfig->findSubconfigByName("CLUSTER");
}

//*****************************************************************
//FUNCTION: 
bool CRetouchTask::execute(const CPointCluster* vUserSpecifiedCluster)
{
	_ASSERTE(vUserSpecifiedCluster && m_pPointClusterExpander);

	return m_pPointClusterExpander->execute<CPointClusterExpander>(vUserSpecifiedCluster);
}