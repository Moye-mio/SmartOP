#pragma once
#include <common/HiveConfig.h>
#include <common/Singleton.h>
#include <common/ConfigInterface.h>

namespace hiveObliquePhotography
{
	namespace Visualization
	{
		const std::string VIEW_BINARY_RESULT = "VIEW_BINARY_RESULT";
		const std::string SWITCH_UNWANTED_DISCARD = "SWITCH_UNWANTED_DISCARD";
		const std::string SWITCH_BINARY_GROWING = "SWITCH_BINARY_GROWING";
		const std::string SWITCH_BINARY_CLUSTER_LABEL = "SWITCH_BINARY_CLUSTER_LABEL";
		const std::string POINT_SHOW_SIZE = "POINT_SHOW_SIZE";
		const std::string SWITCH_LINEPICK = "SWITCH_LINEPICK";
		const std::string LINEWIDTH = "LINEWIDTH";
		const std::string UNDO = "UNDO";

		class CVisualizationConfig : public hiveConfig::CHiveConfig, public hiveDesignPattern::CSingleton<CVisualizationConfig>
		{
		public:
			~CVisualizationConfig() override = default;

		private:
			CVisualizationConfig()
			{
				CVisualizationConfig::__defineAttributesV();
			
				std::string RelativePath = "../";
#ifdef _UNIT_TEST
				RelativePath = "../../";
#endif // _UNIT_TEST

				const std::string ConfigPath = "Configs/VisualizationConfig.xml";

				if (hiveConfig::hiveParseConfig(RelativePath + ConfigPath, hiveConfig::EConfigType::XML, this) != hiveConfig::EParseResult::SUCCEED)
				{
					_HIVE_OUTPUT_WARNING(_FORMAT_STR1("Failed to parse config file [%1%].", ConfigPath));
					return;
				}
			}

			void __defineAttributesV() override
			{
				_defineAttribute(VIEW_BINARY_RESULT, hiveConfig::EConfigDataType::ATTRIBUTE_STRING);
				_defineAttribute(SWITCH_UNWANTED_DISCARD, hiveConfig::EConfigDataType::ATTRIBUTE_STRING);
				_defineAttribute(SWITCH_BINARY_GROWING, hiveConfig::EConfigDataType::ATTRIBUTE_STRING);
				_defineAttribute(SWITCH_BINARY_CLUSTER_LABEL, hiveConfig::EConfigDataType::ATTRIBUTE_STRING);
				_defineAttribute(SWITCH_LINEPICK, hiveConfig::EConfigDataType::ATTRIBUTE_STRING);
				_defineAttribute(UNDO, hiveConfig::EConfigDataType::ATTRIBUTE_STRING);
				_defineAttribute(LINEWIDTH, hiveConfig::EConfigDataType::ATTRIBUTE_FLOAT);
				
				_defineAttribute(POINT_SHOW_SIZE, hiveConfig::EConfigDataType::ATTRIBUTE_DOUBLE);
				_defineAttribute(POINT_SHOW_SIZE, hiveConfig::EConfigDataType::ATTRIBUTE_INT);

			}

			friend class hiveDesignPattern::CSingleton<CVisualizationConfig>;
		};
	}
}
