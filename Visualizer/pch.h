// pch.h: ����Ԥ�����ͷ�ļ���
// �·��г����ļ�������һ�Σ�����˽������ɵ��������ܡ�
// �⻹��Ӱ�� IntelliSense ���ܣ�����������ɺ�������������ܡ�
// ���ǣ�����˴��г����ļ��е��κ�һ��������֮���и��£�����ȫ�����������±��롣
// �����ڴ˴����ҪƵ�����µ��ļ����⽫ʹ������������Ч��

#ifndef PCH_H
#define PCH_H

// ���Ҫ�ڴ˴�Ԥ����ı�ͷ
#include "framework.h"

#include <vector>
#include <string>
#include <map>

#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/search/kdtree.h>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "common/Product.h"
#include "common/Factory.h"
#include "common/Singleton.h"
#include "common/CommonInterface.h"
#include "common/DesignPatternInterface.h"
#include "common/EventLoggerInterface.h"

using PointCloud_t = pcl::PointCloud<pcl::PointSurfel>;

#endif // PCH_H
