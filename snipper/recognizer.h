#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include <QObject>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

class Recognizer : public QObject
{
    Q_OBJECT

signals:
    void recognized(std::optional<std::vector<std::pair<QString, float>>>);

public slots:
    void recognize(const QPixmap&);

public:
    Recognizer();
    ~Recognizer();

private:
    tesseract::TessBaseAPI *api = nullptr;

    //cv::dnn::TextDetectionModel_DB *detect = nullptr;

    //cv::dnn::TextRecognitionModel *rec = nullptr;

    std::unique_ptr<cv::dnn::Net> tf_net = nullptr;
    std::vector<std::string> outNames;

    void setRecognizer();
    void setDetector();

    void recognizeBackend(const cv::Mat &img,std::vector<std::pair<QString, float>>&);
    void extractBoxes(cv::Mat &scores, cv::Mat& geometry, std::vector<cv::RotatedRect>& boxes, std::vector<float> &confidences);

};

#endif // RECOGNIZER_H
