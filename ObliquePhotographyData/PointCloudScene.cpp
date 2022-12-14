#include "pch.h"
#include "PointCloudScene.h"

#include <common/FileSystem.h>
#include <string>
#include "PointCloudLoader.h"
#include "PointCloudSaver.h"

using namespace hiveObliquePhotography;

CPointCloudScene::~CPointCloudScene()
{}

//*****************************************************************
//FUNCTION: 
std::vector<PointCloud_t::Ptr> CPointCloudScene::loadScene(const std::vector<std::string>& vFileNameSet)
{
	if (vFileNameSet.empty()) return {};

	std::vector<std::string> LoadedFileSet;
	std::vector<PointCloud_t::Ptr> TileSet;
	for (const auto& FileName : vFileNameSet)
	{
		std::string LowerFileName = hiveUtility::hiveLocateFile(FileName);
		if(LowerFileName.empty())
		{
			_HIVE_OUTPUT_WARNING(_FORMAT_STR1("Fail to load file [%1%] because it does not exist.", FileName));
			continue;
		}
		std::ranges::transform(LowerFileName, LowerFileName.begin(), ::tolower);

		if (std::ranges::find(LoadedFileSet, LowerFileName) != LoadedFileSet.end())
		{
			_HIVE_OUTPUT_WARNING(_FORMAT_STR1("[%1%] has already been loaded.", FileName));
			continue;
		}
		
		auto* pTileLoader = hiveDesignPattern::hiveGetOrCreateProduct<IPointCloudLoader>(hiveUtility::hiveGetFileSuffix(FileName));
		if (pTileLoader)
		{
			std::shared_ptr<PointCloud_t> pTile = nullptr;
			try
			{
				pTile = pTileLoader->loadDataFromFile(FileName);
			}
			catch (...) { }

			if(!pTile) continue;
			TileSet.push_back(pTile);
			LoadedFileSet.emplace_back(LowerFileName);
		}
		else
		{
			_HIVE_OUTPUT_WARNING(_FORMAT_STR1("Fail to load tile [%1%] due to unknown format.", FileName));
		}
	}
	return TileSet;
}

//*****************************************************************
//FUNCTION: 
bool CPointCloudScene::saveScene(PointCloud_t::Ptr vPointCloud, std::string vFileName)
{
	auto* pSceneSaver = hiveDesignPattern::hiveGetOrCreateProduct<IPointCloudSaver>(hiveUtility::hiveGetFileSuffix(vFileName) + "_Save");
	if (pSceneSaver)
	{
		pSceneSaver->saveDataToFileV(vPointCloud, vFileName);
	}
	else
	{
		_HIVE_OUTPUT_WARNING(_FORMAT_STR1("Fail to save tile [%1%] due to unknown format.", vFileName));
	}
	return true;
}
