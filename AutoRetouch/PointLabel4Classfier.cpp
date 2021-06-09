#include "pch.h"
#include "PointLabel4Classfier.h"
#include "PointCloudAutoRetouchScene.h"
#include "PointLabelChangeRecord.h"

using namespace hiveObliquePhotography::AutoRetouch;

//*****************************************************************
//FUNCTION: 
bool CLocalPointLabelSet::changePointLabel(std::uint64_t vPointIndex, EPointLabel vDstLabel)
{
	if (!m_IsRecording) return false;
//TODO: ���vPointIndex�Ƿ���Ч����鵱ǰ��label��ΪvDstLabel�Ƿ�Ϸ�

	SPointLabelChange t = { vPointIndex, m_PointLabelSet[vPointIndex], vDstLabel };
	m_PointLabelChangeRecord.emplace_back(t);
	m_PointLabelSet[vPointIndex] = vDstLabel;
	return true;
}

//*****************************************************************
//FUNCTION: 
bool CLocalPointLabelSet::startRecord()
{
	if (m_IsRecording) return false;

	m_PointLabelChangeRecord.clear();
	m_IsRecording = true;
	return true;
}

//*****************************************************************
//FUNCTION: 
bool CLocalPointLabelSet::stopRecord()
{
	if (!m_IsRecording) return false;
	m_IsRecording = false;
	__cleanPointLabelChangeRecord();
	__sortPointLabelChangeRecordByIndex();
	return true;
}

//*****************************************************************
//FUNCTION: 
void CLocalPointLabelSet::__sortPointLabelChangeRecordByIndex()
{
	std::sort(m_PointLabelChangeRecord.begin(), m_PointLabelChangeRecord.end());
}

//*****************************************************************
//FUNCTION: 
bool CPointLabel4Classfier::dumpPointLabelChangeRecord(std::vector<SPointLabelChange>& voResult) const
{
	if (!__isReady2DumpChangeRecordV()) return false;

	voResult.clear();
	voResult = m_PointLabelChangeRecord;
	return true;
}

//*****************************************************************
//FUNCTION: 
void CLocalPointLabelSet::__cleanPointLabelChangeRecord()
{
//TODO: ���ܴ��ڶ�һ�����θ���label�������������ʵ��ֻ��Ҫ���յĸ��ģ���һ������m_PointLabelChangeRecord��ֻ����һ����¼,
//      ��������¼��Ҫ��¼�������������յı�ǩ
}

//*****************************************************************
//FUNCTION: 
 bool CLocalPointLabelSet::reset(const std::vector<SPointLabelChange>& vChangeRecord)
 {
 	_HIVE_EARLY_RETURN(m_IsRecording, "Fail to undo the specified operation because another operation is active.", false);
 	return _reset(vChangeRecord);
 }

//*****************************************************************
//FUNCTION: 
bool CPointLabel4Classfier::_reset(const std::vector<SPointLabelChange>& vChangeRecord)
{

//TODO: ��鵱ǰ��label�Ƿ��vChangeRecord�е�DstLabel�Ƿ�һ�£�����һ�£��ܾ�reset

	for (const auto& e : vChangeRecord) m_PointLabelSet[e.Index] = e.SrcLabel;

	return true;
}

//*****************************************************************
//FUNCTION: 
bool CPointLabel4Classfier::reset()
{
	_HIVE_EARLY_RETURN(!__isReady2ResetV(), "Fail to undo the specified operation because another operation is active.", false);
	return _reset(m_PointLabelChangeRecord);
}

//*****************************************************************
//FUNCTION: 
bool CLocalPointLabelSet::update(CGlobalPointLabelSet* vGlobalLabelSet)
{
	if (m_IsRecording) return false;

	_ASSERTE(vGlobalLabelSet);
	m_PointLabelSet = vGlobalLabelSet->getPointLabelSet();
	m_Timestamp = vGlobalLabelSet->getTimestamp();
	return true;
}

//*****************************************************************
//FUNCTION: 
void CGlobalPointLabelSet::applyPointLabelChange(const std::vector<SPointLabelChange>& vChangeRecord)
{
	for (const auto& e : vChangeRecord)
	{
		_ASSERTE(e.Index < m_PointLabelSet.size());
		m_PointLabelSet[e.Index] = e.DstLabel;
	}

	m_PointLabelChangeRecord = vChangeRecord;
	m_Timestamp = hiveCommon::hiveGetGlobalTimestamp();
	CPointCloudAutoRetouchScene::getInstance()->recordCurrentOp(new CPointLabelChangeRecord(vChangeRecord));
}

//*****************************************************************
//FUNCTION: 
void CGlobalPointLabelSet::init(std::size_t vSize, EPointLabel vLabel)
{
	_ASSERTE(vSize != 0);
	m_PointLabelSet.resize(vSize, vLabel);
}

//*****************************************************************
//FUNCTION: 
CLocalPointLabelSet* CGlobalPointLabelSet::clone() const
{
	CLocalPointLabelSet* pCloned = new CLocalPointLabelSet;
	pCloned->m_PointLabelSet = m_PointLabelSet;
	pCloned->m_Timestamp = m_Timestamp;
	return pCloned;
}