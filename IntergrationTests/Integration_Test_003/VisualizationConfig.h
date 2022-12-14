#pragma once
#include <common/HiveConfig.h>
#include <common/Singleton.h>
#include <common/ConfigInterface.h>

namespace hiveObliquePhotography
{
	namespace Visualization
	{
		const std::string SWITCH_UNWANTED_DISCARD = "SWITCH_UNWANTED_DISCARD";
		const std::string SWITCH_UNWANTED_KEPT_MODE = "SWITCH_UNWANTED_KEPT_MODE";
		const std::string CLUSTER_EXPANDER_MODE = "CLUSTER_EXPANDER_MODE";
		const std::string REMOVE_OUTLIER = "REMOVE_OUTLIER";
		const std::string UNDO = "UNDO";

		const std::string POINT_SHOW_SIZE = "POINT_SHOW_SIZE";
		const std::string SCREEN_CIRCLE_RADIUS = "SCREEN_CIRCLE_RADIUS";
		const std::string SCREEN_CIRCLE_HARDNESS = "SCREEN_CIRCLE_HARDNESS";

		const std::string CIRCLE_MODE = "CIRCLE_MODE";
		const std::string RUBBER_MODE = "RUBBER_MODE";
		const std::string UNWANTED_MODE = "UNWANTED_MODE";
		const std::string REFRESH_IMMEDIATELY = "REFRESH_IMMEDIATELY";

		class CVisualizationConfig : public hiveConfig::CHiveConfig, public hiveDesignPattern::CSingleton<CVisualizationConfig>
		{
		public:
			~CVisualizationConfig() override = default;

		private:
			CVisualizationConfig()
			{
				CVisualizationConfig::__defineAttributesV();
			
				const std::string ConfigPath = "VisualizationConfig.xml";

				if (hiveConfig::hiveParseConfig(/*RelativePath + */ConfigPath, hiveConfig::EConfigType::XML, this) != hiveConfig::EParseResult::SUCCEED)
				{
					_HIVE_OUTPUT_WARNING(_FORMAT_STR1("Failed to parse config file [%1%].", ConfigPath));
					return;
				}
			}

			void __defineAttributesV() override
			{
				_defineAttribute(SWITCH_UNWANTED_DISCARD, hiveConfig::EConfigDataType::ATTRIBUTE_STRING);
				_defineAttribute(SWITCH_UNWANTED_KEPT_MODE, hiveConfig::EConfigDataType::ATTRIBUTE_STRING);
				_defineAttribute(CLUSTER_EXPANDER_MODE, hiveConfig::EConfigDataType::ATTRIBUTE_STRING);
				_defineAttribute(REMOVE_OUTLIER, hiveConfig::EConfigDataType::ATTRIBUTE_STRING);
				_defineAttribute(UNDO, hiveConfig::EConfigDataType::ATTRIBUTE_STRING);

				_defineAttribute(CIRCLE_MODE, hiveConfig::EConfigDataType::ATTRIBUTE_BOOL);
				_defineAttribute(RUBBER_MODE, hiveConfig::EConfigDataType::ATTRIBUTE_BOOL);
				_defineAttribute(UNWANTED_MODE, hiveConfig::EConfigDataType::ATTRIBUTE_BOOL);
				_defineAttribute(REFRESH_IMMEDIATELY, hiveConfig::EConfigDataType::ATTRIBUTE_BOOL);
				
				_defineAttribute(SCREEN_CIRCLE_RADIUS, hiveConfig::EConfigDataType::ATTRIBUTE_DOUBLE);
				_defineAttribute(SCREEN_CIRCLE_HARDNESS, hiveConfig::EConfigDataType::ATTRIBUTE_DOUBLE);
				_defineAttribute(POINT_SHOW_SIZE, hiveConfig::EConfigDataType::ATTRIBUTE_DOUBLE);

			}

			friend class hiveDesignPattern::CSingleton<CVisualizationConfig>;
		};
	}
}
