#include "pch.h"
#include "SingleStepWindow.h"
#include "SliderSizeDockWidget.h"
#include "SingleStepConfig.h"
#include <QtWidgets/qmdisubwindow.h>
#include <QSlider>
#include <QtWidgets/QFileDialog>
#include <qobject.h>
#include <vtkRenderWindow.h>
#include <vtkAutoInit.h>

#include "ObliquePhotographyDataInterface.h"
#include "PointCloudRetouchInterface.h"
#include "VisualizationInterface.h"
#include "PointCloudRetouchConfig.h"

#include <fstream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);

using namespace hiveObliquePhotography;

CSingleStepWindow::CSingleStepWindow(QWidget * vParent)
    : QMainWindow(vParent)
{
    Visualization::hiveGetVisualizationConfig(m_pVisualizationConfig);

    m_WindowUI.setupUi(this);

    __connectSignals();
    __parseConfigFile();
}

CSingleStepWindow::~CSingleStepWindow()
{
}

void CSingleStepWindow::__connectSignals()
{
    QObject::connect(m_WindowUI.actionOpen, SIGNAL(triggered()), this, SLOT(__onActionOpen()));
    QObject::connect(m_WindowUI.actionMark, SIGNAL(triggered()), this, SLOT(__onActionMark()));
    QObject::connect(m_WindowUI.actionShow, SIGNAL(triggered()), this, SLOT(__onActionShow()));
    QObject::connect(m_WindowUI.actionClear, SIGNAL(triggered()), this, SLOT(__onActionClear()));
}

void CSingleStepWindow::__initialVTKWidget()
{
    auto pViewer = static_cast<pcl::visualization::PCLVisualizer*>(Visualization::hiveGetPCLVisualizer());
    m_WindowUI.VTKWidget->SetRenderWindow(pViewer->getRenderWindow());
    pViewer->setupInteractor(m_WindowUI.VTKWidget->GetInteractor(), m_WindowUI.VTKWidget->GetRenderWindow());
    m_WindowUI.VTKWidget->update();
}

void CSingleStepWindow::__initialSlider()
{
    auto pSubWindow = new QMdiSubWindow(m_WindowUI.VTKWidget);

    m_pPointSizeSlider = new QSlider(Qt::Horizontal);
    m_pPointSizeSlider->setMinimum(1);
    m_pPointSizeSlider->setMaximum(7);
    m_pPointSizeSlider->setSingleStep(1);
    m_pPointSizeSlider->setTickInterval(1);
    m_pPointSizeSlider->setTickPosition(QSlider::TicksAbove);
    m_pPointSizeSlider->setValue(*m_pVisualizationConfig->getAttribute<double>(Visualization::POINT_SHOW_SIZE));

    connect(m_pPointSizeSlider, &QSlider::valueChanged, [&]()
        {
            m_PointSize = m_pPointSizeSlider->value();
            auto OverwriteSuccess = m_pVisualizationConfig->overwriteAttribute(Visualization::POINT_SHOW_SIZE, static_cast<double>(m_PointSize));
            if (OverwriteSuccess)
            {
                Visualization::hiveRefreshVisualizer(m_PointLabel);
            }
        }
    );

    pSubWindow->setWidget(m_pPointSizeSlider);
    pSubWindow->resize(200, 50);                // magic
    pSubWindow->setWindowFlag(Qt::FramelessWindowHint);
    pSubWindow->show();
}

void CSingleStepWindow::__parseConfigFile()
{
    const std::string RetouchConfigPath = "PointCloudRetouchConfig.xml";
    m_pPointCloudRetouchConfig = new hiveObliquePhotography::PointCloudRetouch::CPointCloudRetouchConfig;
    if (hiveConfig::hiveParseConfig(RetouchConfigPath, hiveConfig::EConfigType::XML, m_pPointCloudRetouchConfig) != hiveConfig::EParseResult::SUCCEED)
    {
        _HIVE_OUTPUT_WARNING(_FORMAT_STR1("Failed to parse config file [%1%].", RetouchConfigPath));
        return;
    }

    const std::string StepConfigPath = "SingleStepConfig.xml";
    if (hiveConfig::hiveParseConfig(StepConfigPath, hiveConfig::EConfigType::XML, CSingleStepConfig::getInstance()) != hiveConfig::EParseResult::SUCCEED)
    {
        _HIVE_OUTPUT_WARNING(_FORMAT_STR1("Failed to parse config file [%1%].", StepConfigPath));
        return;
    }
}

std::string CSingleStepWindow::__getFileName(const std::string& vFilePath)
{
    return vFilePath.substr(vFilePath.find_last_of('/') + 1, vFilePath.find_last_of('.') - vFilePath.find_last_of('/') - 1);
}

std::string CSingleStepWindow::__getDirectory(const std::string& vFilePath)
{
    return vFilePath.substr(0, vFilePath.find_last_of('/'));
}

void CSingleStepWindow::__onActionOpen()
{
    QStringList FilePathList = QFileDialog::getOpenFileNames(this, tr("Open PointCloud"), QString::fromStdString(m_CloudPath), tr("Open PointCloud files(*.pcd)"));
    std::vector<std::string> FilePathSet;

    if (FilePathList.empty())
        return;

    foreach(QString FilePathQString, FilePathList)
    {
        std::string FilePathString = FilePathQString.toStdString();
        FilePathSet.push_back(FilePathString);
    }

    if (FilePathSet.empty())
        return;

    __loadCloud(FilePathSet);
}

void CSingleStepWindow::__onActionMark()
{
    if (!m_pCloud.empty())
    {
        if (m_WindowUI.actionMark->isChecked())
        {
            m_pPointPickingDockWidget = new CSliderSizeDockWidget(m_WindowUI.VTKWidget, m_pVisualizationConfig, CSingleStepConfig::getInstance());
            m_pPointPickingDockWidget->setWindowTitle(QString("Point Picking"));
            m_pPointPickingDockWidget->show();
        }
        else
        {
            Visualization::hiveRefreshVisualizer(m_PointLabel);
            m_pPointPickingDockWidget->close();
            delete m_pPointPickingDockWidget;
            m_pPointPickingDockWidget = nullptr;
        }

        if (m_pVisualizationConfig)
            m_pVisualizationConfig->overwriteAttribute(Visualization::CIRCLE_MODE, m_WindowUI.actionMark->isChecked());
    }
}

void CSingleStepWindow::__onActionShow()
{
    if (!m_pCloud.empty())
    {
        if (m_pPointPickingDockWidget)
        {
            delete m_pPointPickingDockWidget;
            m_pPointPickingDockWidget = nullptr;
        }


        static std::size_t CurrentPoint = 0;
        Eigen::Vector3i DeltaColor = (m_pVisualizationConfig->getAttribute<bool>(Visualization::UNWANTED_MODE).value() ? m_LitterEndColor : m_BackgroundEndColor) - m_BeginColor;

        if (m_WindowUI.actionMark->isChecked())
        {
            PointCloudRetouch::hiveDumpExpandResult(m_ExpandedPointSet, m_pVisualizationConfig->getAttribute<bool>(Visualization::UNWANTED_MODE).value());
            _ASSERTE(!m_ExpandedPointSet.empty());
            CurrentPoint = 0;
            {
                m_WindowUI.actionMark->setChecked(false);
                m_pVisualizationConfig->overwriteAttribute(Visualization::CIRCLE_MODE, m_WindowUI.actionMark->isChecked());
            }
        }

        std::vector<int> SingleStepPoints;
        int End = CurrentPoint + CSingleStepConfig::getInstance()->getAttribute<float>(STEP_RATIO).value() * 0.01f * m_ExpandedPointSet.size();
        if (End > m_ExpandedPointSet.size())
            End = m_ExpandedPointSet.size();
        while (CurrentPoint < End)
            SingleStepPoints.push_back(m_ExpandedPointSet[CurrentPoint++]);

        float ChangeRate = (float)CurrentPoint / m_ExpandedPointSet.size();

        Visualization::hiveHighlightPointSet(SingleStepPoints, 
            {
            m_BeginColor.x() + (int)(DeltaColor.x() * ChangeRate),
            m_BeginColor.y() + (int)(DeltaColor.y() * ChangeRate),
            m_BeginColor.z() + (int)(DeltaColor.z() * ChangeRate)
            });

        if (CurrentPoint == m_ExpandedPointSet.size())
            PointCloudRetouch::hiveDumpPointLabel(m_PointLabel);
        Visualization::hiveRefreshVisualizer(m_PointLabel);
    }
}

void CSingleStepWindow::__onActionClear()
{
    if (!m_pCloud.empty())
    {
        PointCloudRetouch::hiveClearMark();
        Visualization::hiveCancelAllHighlighting();
        PointCloudRetouch::hiveDumpPointLabel(m_PointLabel);
        Visualization::hiveRefreshVisualizer(m_PointLabel);
    }
}

void CSingleStepWindow::__loadCloud(const std::vector<std::string>& vFilePathSet)
{
    m_pCloud = hiveInitPointCloudScene(vFilePathSet);

    if (!m_pCloud.empty())
    {
        PointCloudRetouch::hiveInit(m_pCloud, m_pPointCloudRetouchConfig);
        Visualization::hiveInitVisualizer(m_pCloud, true);
        CSingleStepWindow::__initialVTKWidget();
        PointCloudRetouch::hiveDumpPointLabel(m_PointLabel);
        Visualization::hiveRefreshVisualizer(m_PointLabel, true);
        CSingleStepWindow::__initialSlider();
    }
}
