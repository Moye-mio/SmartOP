#include "pch.h"
#include "RetouchTask.h"
#include "PointClassifier.h"
#include "PointClusterExpanderBase.h"
#include "PointClusterExpander.h"  //FIXME: ���ͷ�ļ��б�Ҫinclude��
#include "PointClusterExpanderMultithread.h"

using namespace hiveObliquePhotography::PointCloudRetouch;

//*****************************************************************
//FUNCTION: 
bool CRetouchTask::init(const hiveConfig::CHiveConfig* vConfig)
{
	_ASSERTE(vConfig);

	try 
	{
		auto NumSubconfig = vConfig->getNumSubconfig();  //FIXME��Ϊʲô����ôһ�仰����Ҫtry/catch��
	}
	catch (...)
	{
		throw "vConfig Error";
		return false;
	}
//FIXME����ĳ����߼������ظ�����init()�������������Ҫ��֤m_pConfigΪ��
	m_pConfig = vConfig;  
	m_pClusterConfig = nullptr;

//FIXME: �����кü����ط���������_HIVE_EARLY_RETURN��������XML����ʧ�ܣ���ʧ�ܺ�m_pConfig��Ȼ������Чֵ��
//        ����˵���������ʧ�ܣ�m_pConfig�͸ø�ԭ
	for (auto i = 0; i < vConfig->getNumSubconfig(); i++)
	{
		const hiveConfig::CHiveConfig* pConfig = vConfig->getSubconfigAt(i);
		if (_IS_STR_IDENTICAL(pConfig->getSubconfigType(), std::string("CLUSTER")))
		{
			if (!m_pClusterConfig)
			{
				m_pClusterConfig = pConfig;
			}
			else
			{
				_HIVE_OUTPUT_WARNING("It is NOT allowed to define cluster twice.");  //FIXME����Ҫ��һ��������������֤ȷʵ�ܴ���������������CLUSTER�����
			}
			continue;
		}
		if (_IS_STR_IDENTICAL(pConfig->getSubconfigType(), std::string("CLASSIFIER")))
		{
			std::optional<std::string> ClassifierSig = pConfig->getAttribute<std::string>("SIG");
			_ASSERTE(ClassifierSig.has_value());  //FIXME������ط���_ASSERTE()̫���ˣ�����ⲿ�����ļ�����û����SIG����release״̬�¾ͻ����

//FIXME: m_pPointClusterExpander����������ã�����һ��retouch task��˵�����m_pPointClusterExpander���񳶲��Ϲ�ϵ
			if (_IS_STR_IDENTICAL(ClassifierSig.value(), std::string("CLUSTER_EXPANDER_MULTITHREAD")))
				m_pPointClusterExpander = dynamic_cast<CPointClusterExpanderMultithread*>(hiveDesignPattern::hiveCreateProduct<IPointClassifier>(ClassifierSig.value()));
			else if (_IS_STR_IDENTICAL(ClassifierSig.value(), std::string("CLUSTER_EXPANDER")))
				m_pPointClusterExpander = dynamic_cast<CPointClusterExpander*>(hiveDesignPattern::hiveCreateProduct<IPointClassifier>(ClassifierSig.value()));
//FIMXE����Ҫ��һ������������֤���ܴ���m_pPointClusterExpanderû�д����ɹ������
			_HIVE_EARLY_RETURN(!m_pPointClusterExpander, _FORMAT_STR1("Fail to initialize retouch due to the failure of creating point classifier [%1%].", ClassifierSig.value()), false);
			continue;
		}
		_HIVE_OUTPUT_WARNING(_FORMAT_STR1("Unknown subconfiguration type [%1%].", pConfig->getSubconfigType()));
	}

	_HIVE_EARLY_RETURN(!m_pPointClusterExpander, _FORMAT_STR1("Fail to initialize the retouch task [%1%] due to missing point classifier.", vConfig->getName()), false);

	return true;
}

void CRetouchTask::reset()
{
	m_pConfig = nullptr;
	m_pClusterConfig = nullptr;
}

//*****************************************************************
//FUNCTION: 
bool CRetouchTask::execute(const CPointCluster* vUserSpecifiedCluster) const
{
	_ASSERTE(vUserSpecifiedCluster && m_pPointClusterExpander);

//FIXME: ���������е�֣��Ƿ���ö��̣߳���Ȼ����������ȫ��ͬ����
	if (_IS_STR_IDENTICAL(m_pPointClusterExpander->getProductSig(), std::string("CLUSTER_EXPANDER_MULTITHREAD")))
		m_pPointClusterExpander->execute<CPointClusterExpanderMultithread>(vUserSpecifiedCluster);
	else if (_IS_STR_IDENTICAL(m_pPointClusterExpander->getProductSig(), std::string("CLUSTER_EXPANDER")))
		m_pPointClusterExpander->execute<CPointClusterExpander>(vUserSpecifiedCluster);

	return m_pPointClusterExpander->getExpandPoints().size();  //FIXME����Ҫǿ��ȥ��һ������ת��Ϊbool���ɶ��Բ���
//FIMXE������ֵ�е���֣��������û�б���չ���᷵��һ��false��
}

//*****************************************************************
//FUNCTION: 
//FIXME: ������˵����MarkedPoints��ʵ�ʷ��ص���ExpandPoints
//FIXME: execute()����Ϊʲô�����ر���չ�����ĵ㣬��Ҫ��execute()����������ֿ���
void CRetouchTask::dumpTaskMarkedPoints(std::vector<pcl::index_t>& voMarkedPoints) const
{
	voMarkedPoints = m_pPointClusterExpander->getExpandPoints();
}