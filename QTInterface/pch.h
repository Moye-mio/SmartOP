// pch.h: ����Ԥ�����ͷ�ļ���
// �·��г����ļ�������һ�Σ�����˽������ɵ��������ܡ�
// �⻹��Ӱ�� IntelliSense ���ܣ�����������ɺ�������������ܡ�
// ���ǣ�����˴��г����ļ��е��κ�һ��������֮���и��£�����ȫ�����������±��롣
// �����ڴ˴����ҪƵ�����µ��ļ����⽫ʹ������������Ч��
#pragma once

#ifndef PCH_H
#define PCH_H

// ���Ҫ�ڴ˴�Ԥ����ı�ͷ
#include "framework.h"

#include <vector>
#include <string>
#include <map>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "pcl/visualization/pcl_visualizer.h"
#include <pcl/search/kdtree.h>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>

#include "common/Product.h"
#include "common/Singleton.h"
#include "common/CommonMicro.h"
#include "common/CommonInterface.h"
#include "common/EventLoggerInterface.h"
#include "common/DesignPatternInterface.h"
#include "common/HiveConfig.h"
#include "common/ConfigInterface.h"
#include "common/UtilityInterface.h"
#include "common/FileSystem.h"


using PointCloud_t = pcl::PointCloud<pcl::PointSurfel>;

#endif //PCH_H

