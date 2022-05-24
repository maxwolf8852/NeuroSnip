#include "recognizer.h"
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include "alphabet.h"

Recognizer::Recognizer()
{
    api = new tesseract::TessBaseAPI();

    if (api->Init("data/", "eng", tesseract::OcrEngineMode::OEM_DEFAULT)) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }
    api->SetPageSegMode(tesseract::PSM_SINGLE_LINE);


    setDetector();




}

Recognizer::~Recognizer()
{
    api->End();
    delete api;
}

void Recognizer::setRecognizer()
{

}

void Recognizer::setDetector()
{

    tf_net = std::make_unique<cv::dnn::Net>(cv::dnn::readNetFromTensorflow("data/frozen_east_text_detection.pb"));

    outNames.push_back("feature_fusion/Conv_7/Sigmoid");
    outNames.push_back("feature_fusion/concat_3");
}

void Recognizer::recognizeBackend(const cv::Mat &img, std::vector<std::pair<QString, float> > &out)
{
    api->SetImage((uchar*)img.data, img.size().width, img.size().height, img.channels(), img.step1());
    api->Recognize(NULL);
    tesseract::ResultIterator* ri = api->GetIterator();
    tesseract::PageIteratorLevel level = tesseract::RIL_WORD;

    if(ri != 0) {
        do {
            auto word  = QString(ri->GetUTF8Text(level));
            float conf = ri->Confidence(level);

            qDebug()<<word<<conf;

            out.push_back(std::make_pair(word, conf));
        } while((ri->Next(level)));

        out.push_back(std::make_pair("\n", 1.));
    }
}

/* https://github.com/nazim1021/text-detection-and-recognition */
void Recognizer::extractBoxes(cv::Mat &scores, cv::Mat &geometry, std::vector<cv::RotatedRect> &boxes, std::vector<float> &confidences)
{
    boxes.clear();
    confidences.clear();

    const int height = scores.size[2];
    const int width = scores.size[3];
    for (int y = 0; y < height; ++y)
    {
        std::vector<cv::Rect> _boxes;
        std::vector<float> conf;
        const float *scoresData = scores.ptr<float>(0, 0, y);
        const float *x0_data = geometry.ptr<float>(0, 0, y);
        const float *x1_data = geometry.ptr<float>(0, 1, y);
        const float *x2_data = geometry.ptr<float>(0, 2, y);
        const float *x3_data = geometry.ptr<float>(0, 3, y);
        const float *anglesData = geometry.ptr<float>(0, 4, y);
        for (int x = 0; x < width; ++x)
        {
            float score = scoresData[x];
            if (score < 0.6f)
                continue;

            // Decode a prediction.
            // Multiple by 4 because feature maps are 4 time less than input image.
            float offsetX = x * 4.0f, offsetY = y * 4.0f;
            float angle = anglesData[x];
            float cosA = std::cos(angle);
            float sinA = std::sin(angle);
            float h = x0_data[x] + x2_data[x];
            float w = x1_data[x] + x3_data[x];

            cv::Point2f offset(offsetX + cosA * x1_data[x] + sinA * x2_data[x],
                               offsetY - sinA * x1_data[x] + cosA * x2_data[x]);
            cv::Point2f p1 = cv::Point2f(-sinA * h, -cosA * h) + offset;
            cv::Point2f p3 = cv::Point2f(-cosA * w, sinA * w) + offset;
            cv::RotatedRect r(0.5f * (p1 + p3), cv::Size2f(w, h), -angle * 180.0f / (float)CV_PI);

            boxes.push_back(r);
            confidences.push_back(score);

        }
    }
}


static cv::Mat qImage2Mat(QImage image){
    return cv::Mat(image.height(),image.width(),CV_8UC3,(void *)image.constBits(),image.bytesPerLine()).clone();
}

void Recognizer::recognize(const QPixmap& pix)
{

    std::vector<std::pair<QString, float>> outText;
    /*api->SetImage(qImage2PIX(pix.toImage()));

    auto outText = QString(api->GetUTF8Text());*/

    std::vector<cv::Rect> rects;

    auto cv_img = qImage2Mat(pix.toImage().convertToFormat(QImage::Format_BGR888));

    if(cv_img.rows < 32 && cv_img.cols < 32){
        emit recognized(std::nullopt);
        return;
    }


    cv::Mat blob;
    std::vector<cv::Mat> outs;

    int dcols = std::ceil(cv_img.cols/32.)*32 ;
    int drows = std::ceil(cv_img.rows/32.)*32 ;

    qDebug()<<dcols<<drows;

    cv::resize(cv_img, cv_img, cv::Size(dcols, drows));

    cv::dnn::blobFromImage(cv_img, blob, 1.0, cv::Size(dcols, drows), cv::Scalar(123.68, 116.78, 103.94), true, false);

    tf_net->setInput(blob);

    tf_net->forward(outs, outNames);

    qDebug()<<outs.size();

    cv::Mat scores = outs.at(0);
    cv::Mat geometry = outs.at(1);

    std::vector<cv::RotatedRect> boxes;
    std::vector<float> confidences;

    //detect->detect(cv_img, detections);

    try{
        extractBoxes(scores, geometry, boxes, confidences);
    } catch(cv::Exception &e){
        qDebug()<<e.what();
        emit recognized(std::nullopt);
        return;
    }

    std::vector<int> indicies;

    cv::dnn::NMSBoxes(boxes, confidences, 0.6f, 0.4f,indicies );

    qDebug()<<indicies.size();

    for(auto &j : indicies){
        auto box = boxes.at(j).boundingRect();

        if (box.x< 0){
            box.width += box.x;
            box.x = 0;
        }
        if (box.y< 0){
            box.height += box.y;
            box.y = 0;
        }

        if (box.x + box.width > cv_img.cols){
            box.width = cv_img.cols - box.x;
        }
        if (box.y + box.height > cv_img.rows){
            box.height = cv_img.rows - box.y;
        }

        rects.push_back(box);

    }

    if(rects.empty()){
        emit recognized(std::nullopt);
        return;
    }

    std::sort(rects.begin(), rects.end(), [](cv::Rect& a, cv::Rect &b){
        if(a.y >= b.y + b.height || b.y >= a.y + a.height){
            if(a.y < b.y)
                return 1;
            else if (a.y > b.y)
                return 0;
            return 1;
        }
        else{

            if(a.x < b.x)
                return 1;
            else if (a.x > b.x)
                return 0;
            return 1;
        }
    });

    for(auto &j: rects){
        std::cout<<j<<std::endl;
    }


    cv::Rect last;
    int i = 0;
    for(auto &j: rects){
        if(last.area() == 0){
            last = j;
            continue;
        }

        if(j.y + j.height / 2 < last.y + last.height){
            std::cout<<"1 "<<j.y + j.height / 2<<" "<<last.y + last.height<<std::endl;
            last.width = j.x +  j.width - last.x;
            last.height = std::max(last.height, j.height);

            if (last.x + last.width > cv_img.cols){
                last.width = cv_img.cols - last.x;
            }
            if (last.y + last.height > cv_img.rows){
                last.height = cv_img.rows - last.y;
            }
        }
        else{
            std::cout<<"2 "<<j.y + j.height / 2<<" "<<last.y + last.height<<std::endl;
            auto sub_img = cv_img(last).clone();
            //cv::imshow(std::to_string(i++), sub_img);
            try{
                recognizeBackend(sub_img, outText);
            }catch(cv::Exception &e){
                std::cout<<e.what()<<std::endl;
                last = cv::Rect();
                continue;
            }

            last = j;
        }

    }

    auto sub_img = cv_img(last).clone();
    //cv::imshow(std::to_string(i++), sub_img);
    recognizeBackend(sub_img, outText);


    if(outText.empty()){
        emit recognized(std::nullopt);
        return;
    }



    emit recognized(outText);
}
