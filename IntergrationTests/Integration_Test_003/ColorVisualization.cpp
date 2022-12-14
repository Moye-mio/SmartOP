#include "pch.h"
#include "ColorVisualization.h"
#include "PointCloudVisualizer.h"
#include <omp.h>

#define gamma(x) (x > 0.04045 ? pow((x + 0.055f) / 1.055f, 2.4f) : x / 12.92)
#define deg2Rad(deg) (deg * (M_PI / 180.0))
#define rad2Deg(rad) ((180.0 / M_PI) * rad)

using namespace hiveObliquePhotography::Feature;

CColorVisualization::CColorVisualization()
{

}

CColorVisualization::~CColorVisualization()
{

}

void CColorVisualization::init(PointCloud_t::Ptr vPointCloud)
{
	_ASSERT(vPointCloud);
	m_pCloud = vPointCloud;
}

void CColorVisualization::run(const std::vector<pcl::index_t>& vPoints)
{
    std::vector<Eigen::Vector3i> PointCloudColors;
    for (auto Index : vPoints)
        PointCloudColors.push_back({ m_pCloud->points[Index].r, m_pCloud->points[Index].g, m_pCloud->points[Index].b });

    m_MainBaseColors = __adaptiveKMeansCluster(PointCloudColors, m_MaxNumMainColors);

    std::map<pcl::index_t, Eigen::Vector3i> IndexAndColor;

    std::mutex Mutex;

#pragma omp parallel for
    for (int i = 0; i < m_pCloud->size(); i++)
    {
        float MinColorDifference = FLT_MAX;
        int Index = -1;
        Eigen::Vector3i Color{ m_pCloud->points[i].r, m_pCloud->points[i].g, m_pCloud->points[i].b };
        for (int k = 0; k < m_MainBaseColors.size(); k++)
        {
            auto& MainColor = m_MainBaseColors[k];
            float TempColorDifference = __calcColorDifferences(Color, MainColor);
            if (TempColorDifference < MinColorDifference)
            {
                MinColorDifference = TempColorDifference;
                Index = k;
            }
        }

        if (MinColorDifference < m_ColorThreshold)
        {
            //Mutex.lock();
            //IndexAndColor[i] = m_MainBaseColors[Index];
            //Mutex.unlock();
        }
        else
        {
            Mutex.lock();
            IndexAndColor[i] = { 0, 0, 0 };
            Mutex.unlock();
        }
    }

    for (auto& Pair : IndexAndColor)
    {
        Visualization::CPointCloudVisualizer::getInstance()->addUserColoredPoints({ Pair.first }, Pair.second);
    }
}

//*****************************************************************
//FUNCTION: 
std::vector<Eigen::Vector3i> CColorVisualization::__adaptiveKMeansCluster(const std::vector<Eigen::Vector3i>& vColorSet, std::size_t vMaxK) const
{
    _ASSERTE(!vColorSet.empty());
    _ASSERTE(vMaxK != 0);

    std::vector<std::vector<Eigen::Vector3i>> ClusterResults;

    for (int CurrentK = 1; CurrentK <= vMaxK; CurrentK++)
    {
        std::vector<std::pair<Eigen::Vector3i*, Eigen::Vector3i>> TagAndColorSet(vColorSet.size(), { nullptr, Eigen::Vector3i() });
        for (size_t i = 0; i < TagAndColorSet.size(); i++)
        {
            auto& [Tag, Color] = TagAndColorSet[i];
            Color = vColorSet[i];
        }

        std::vector<Eigen::Vector3i> ClusterCentroids;
        for (int i = 0; i < CurrentK; i++)
        {
            Eigen::Vector3i SeedColor;
            int Num = 0;
            float MinColorDifference = FLT_MAX;
            const int MaxAttemptNum = 30;

            do
            {
                MinColorDifference = FLT_MAX;
                SeedColor = TagAndColorSet[hiveMath::hiveGenerateRandomInteger(std::size_t(0), TagAndColorSet.size() - 1)].second;
                for (int k = 0; k < i; k++)
                {
                    float TempDifference = __calcColorDifferences(SeedColor, ClusterCentroids[k]);
                    if (TempDifference < MinColorDifference)
                        MinColorDifference = TempDifference;
                }
                Num++;

            } while (MinColorDifference < 2 * m_ColorThreshold && Num < MaxAttemptNum);

            ClusterCentroids.push_back(SeedColor);
        }

        const int NumIteration = 30;

        for (std::size_t i = 0; i < NumIteration; i++)
        {
            for (auto& [Tag, Color] : TagAndColorSet)
            {
                std::pair<float, Eigen::Vector3i*> MinPair(FLT_MAX, nullptr);
                for (auto& Centroid : ClusterCentroids)
                {
                    float ColorDifference = __calcColorDifferences(Color, Centroid);

                    if (ColorDifference < 0.5 * m_ColorThreshold)
                        MinPair = std::min(MinPair, std::make_pair(ColorDifference, &Centroid));
                }
                Tag = MinPair.second;
            }

            auto calcentroid = [](const std::vector<Eigen::Vector3i>& vClusterColorSet) -> Eigen::Vector3i
            {
                Eigen::Vector3i Centroid(0, 0, 0);
                if (vClusterColorSet.empty())
                    return Centroid;

                for (auto& Color : vClusterColorSet)
                    Centroid += Color;
                Centroid /= vClusterColorSet.size();
                return Centroid;
            };

            for (auto& Centroid : ClusterCentroids)
            {
                std::vector<Eigen::Vector3i> ClusterPointsData;
                for (auto& [Tag, Color] : TagAndColorSet)
                    if (Tag == &Centroid)
                        ClusterPointsData.push_back(Color);
                Centroid = calcentroid(ClusterPointsData);
            }
        }

        ClusterResults.push_back(ClusterCentroids);
    }

    std::pair<float, std::size_t> AverageDifferenceAndIndex(FLT_MAX, -1);

    for (int i = 0; i < ClusterResults.size(); i++)
    {
        auto& ClusterCentroids = ClusterResults[i];
        float SumDifference = 0.0f;

        for (auto& Color : vColorSet)
        {
            float MinDifference = FLT_MAX;
            for (auto& Centroid : ClusterCentroids)
            {
                float TempDifference = __calcColorDifferences(Color, Centroid);
                if (TempDifference < MinDifference)
                    MinDifference = TempDifference;
            }

            SumDifference += MinDifference;
        }

        float AverageDifference = SumDifference / vColorSet.size();
        if (AverageDifference < AverageDifferenceAndIndex.first && AverageDifference / AverageDifferenceAndIndex.first < m_MinReduceRatio)
            AverageDifferenceAndIndex = { AverageDifference, i };
        else
            break;
    }

    return ClusterResults[AverageDifferenceAndIndex.second];
}

//*****************************************************************
//FUNCTION: 
float CColorVisualization::__calcColorDifferences(const Eigen::Vector3i& vLColor, const Eigen::Vector3i& vRColor) const
{
    Eigen::Vector3f LRGBColor{ static_cast<float>(vLColor[0]),static_cast<float>(vLColor[1]),static_cast<float>(vLColor[2]) };
    Eigen::Vector3f RRGBColor{ static_cast<float>(vRColor[0]),static_cast<float>(vRColor[1]),static_cast<float>(vRColor[2]) };
    return __calculateCIEDE2000(__RGB2LAB(LRGBColor), __RGB2LAB(RRGBColor));
}

//*****************************************************************
//FUNCTION: 
float CColorVisualization::__calculateCIEDE2000(const LAB& lab1, const LAB& lab2) const
{
    const float k_L = 1.0, k_C = 1.0, k_H = 1.0;
    const float deg360InRad = deg2Rad(360.0);
    const float deg180InRad = deg2Rad(180.0);
    const float pow25To7 = 6103515625.0;

    float C1 = sqrt((lab1.a * lab1.a) + (lab1.b * lab1.b));
    float C2 = sqrt((lab2.a * lab2.a) + (lab2.b * lab2.b));

    float barC = (C1 + C2) / 2.0;

    float G = 0.5 * (1 - sqrt(pow(barC, 7) / (pow(barC, 7) + pow25To7)));

    float a1Prime = (1.0 + G) * lab1.a;
    float a2Prime = (1.0 + G) * lab2.a;

    float CPrime1 = sqrt((a1Prime * a1Prime) + (lab1.b * lab1.b));
    float CPrime2 = sqrt((a2Prime * a2Prime) + (lab2.b * lab2.b));

    float hPrime1;
    if (lab1.b == 0 && a1Prime == 0)
        hPrime1 = 0.0;
    else {
        hPrime1 = atan2(lab1.b, a1Prime);

        if (hPrime1 < 0)
            hPrime1 += deg360InRad;
    }
    float hPrime2;
    if (lab2.b == 0 && a2Prime == 0)
        hPrime2 = 0.0;
    else {
        hPrime2 = atan2(lab2.b, a2Prime);

        if (hPrime2 < 0)
            hPrime2 += deg360InRad;
    }

    float deltaLPrime = lab2.l - lab1.l;

    float deltaCPrime = CPrime2 - CPrime1;

    float deltahPrime;
    float CPrimeProduct = CPrime1 * CPrime2;
    if (CPrimeProduct == 0)
        deltahPrime = 0;
    else {

        deltahPrime = hPrime2 - hPrime1;
        if (deltahPrime < -deg180InRad)
            deltahPrime += deg360InRad;
        else if (deltahPrime > deg180InRad)
            deltahPrime -= deg360InRad;
    }

    float deltaHPrime = 2.0 * sqrt(CPrimeProduct) *
        sin(deltahPrime / 2.0);

    float barLPrime = (lab1.l + lab2.l) / 2.0;

    float barCPrime = (CPrime1 + CPrime2) / 2.0;

    float barhPrime, hPrimeSum = hPrime1 + hPrime2;
    if (CPrime1 * CPrime2 == 0) {
        barhPrime = hPrimeSum;
    }
    else {
        if (fabs(hPrime1 - hPrime2) <= deg180InRad)
            barhPrime = hPrimeSum / 2.0;
        else {
            if (hPrimeSum < deg360InRad)
                barhPrime = (hPrimeSum + deg360InRad) / 2.0;
            else
                barhPrime = (hPrimeSum - deg360InRad) / 2.0;
        }
    }

    float T = 1.0 - (0.17 * cos(barhPrime - deg2Rad(30.0))) +
        (0.24 * cos(2.0 * barhPrime)) +
        (0.32 * cos((3.0 * barhPrime) + deg2Rad(6.0))) -
        (0.20 * cos((4.0 * barhPrime) - deg2Rad(63.0)));

    float deltaTheta = deg2Rad(30.0) *
        exp(-pow((barhPrime - deg2Rad(275.0)) / deg2Rad(25.0), 2.0));

    float R_C = 2.0 * sqrt(pow(barCPrime, 7.0) /
        (pow(barCPrime, 7.0) + pow25To7));

    float S_L = 1 + ((0.015 * pow(barLPrime - 50.0, 2.0)) /
        sqrt(20 + pow(barLPrime - 50.0, 2.0)));

    float S_C = 1 + (0.045 * barCPrime);

    float S_H = 1 + (0.015 * barCPrime * T);

    float R_T = (-sin(2.0 * deltaTheta)) * R_C;

    float deltaE = sqrt(
        pow(deltaLPrime / (k_L * S_L), 2.0) +
        pow(deltaCPrime / (k_C * S_C), 2.0) +
        pow(deltaHPrime / (k_H * S_H), 2.0) +
        (R_T * (deltaCPrime / (k_C * S_C)) * (deltaHPrime / (k_H * S_H))));

    return (deltaE);
}

//*****************************************************************
//FUNCTION: 
LAB CColorVisualization::__RGB2LAB(const Eigen::Vector3f& vRGBColor) const
{
    float B = gamma(vRGBColor[2] / 255.0f);
    float G = gamma(vRGBColor[1] / 255.0f);
    float R = gamma(vRGBColor[0] / 255.0f);

    float X = 0.412453 * R + 0.357580 * G + 0.180423 * B;
    float Y = 0.212671 * R + 0.715160 * G + 0.072169 * B;
    float Z = 0.019334 * R + 0.119193 * G + 0.950227 * B;

    X /= 0.95047;
    Y /= 1.0;
    Z /= 1.08883;

    float FX = X > 0.008856f ? pow(X, 1.0f / 3.0f) : (7.787f * X + 0.137931f);
    float FY = Y > 0.008856f ? pow(Y, 1.0f / 3.0f) : (7.787f * Y + 0.137931f);
    float FZ = Z > 0.008856f ? pow(Z, 1.0f / 3.0f) : (7.787f * Z + 0.137931f);

    LAB Lab;
    Lab.l = Y > 0.008856f ? (116.0f * FY - 16.0f) : (903.3f * Y);
    Lab.a = 500.f * (FX - FY);
    Lab.b = 200.f * (FY - FZ);

    return Lab;
}