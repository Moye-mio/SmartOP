#pragma once
#include "ui_SliderSizeDockWidget.h"
#include "QTInterface.h"
#include <QObject>
#include <qdockwidget.h>
#include <qwidget.h>
#include <qstyle.h>

namespace hiveObliquePhotography
{
	namespace QTInterface
	{
		class CSliderSizeDockWidget : public QDockWidget
		{
			Q_OBJECT

		public:
			CSliderSizeDockWidget(QWidget* vParent, QDockWidget*& vThisInParent, hiveConfig::CHiveConfig* vPointCloudRetouchConfig)
				: QDockWidget(vParent),
				m_pThisInParent(vThisInParent),
				m_pVisualizationConfig(vPointCloudRetouchConfig),
				m_pUi(std::make_shared<Ui::CSliderSizeDockWidget>())
			{
				m_pUi->setupUi(this);
				this->setAttribute(Qt::WA_DeleteOnClose);
				__setSize();
				__setPosition(vParent);
				__initialSliderRadius();
				__initialSliderHardness();

				QObject::connect(m_pUi->SliderRadius, SIGNAL(valueChanged(int)), this, SLOT(onActionSliderRadiusChange()));
				QObject::connect(m_pUi->SliderHardness, SIGNAL(valueChanged(int)), this, SLOT(onActionSliderHardnessChange()));
			}

			~CSliderSizeDockWidget()
			{
				//通知主窗口该窗口已经关闭
				m_pThisInParent = nullptr;
			}


		private:
			std::shared_ptr<Ui::CSliderSizeDockWidget> m_pUi = nullptr;
			hiveConfig::CHiveConfig* m_pVisualizationConfig = nullptr;
			QDockWidget*& m_pThisInParent;

			void __setSize();
			void __setPosition(QWidget* vParent);
			void __initialSliderRadius();
			void __initialSliderHardness();

		public slots:
			void onActionSliderRadiusChange();
			void onActionSliderHardnessChange();
		};
	}
}