#include "translator.h"
#include <QDebug>


Translator::Translator(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<std::optional<QString>>("std::optional<QString>");
    qRegisterMetaType<std::optional<std::vector<std::pair<QString, float>>>>("std::optional<std::vector<std::pair<QString, float>>>");

    channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());

    stub = serv_g::ServG::NewStub(channel);



}

Translator::~Translator(){

}

void Translator::setText(QString eng)
{
    /*std::string obj;
    try{
    obj = libreTranslateAPI.translate(eng.toStdString(), "en", "ru")["translatedText"];
    } catch(const char* e){
        qDebug()<<e;
        emit translated(std::nullopt);
        return;
    }
    emit translated(QString::fromStdString(obj));*/

    grpc::ClientContext context;
    serv_g::TargetStr request;
    request.set_text(eng.toStdString());
    request.set_exit(0);
    serv_g::TranslatedString reply;
    grpc::Status status = stub->GetTranslate(&context, request, &reply);

    if (status.ok() && reply.success() && reply.has_translated()) {
        qDebug()<<reply.success()<<QString::fromStdString(reply.translated());
        emit translated(QString::fromStdString(reply.translated()));
    } else {
        qDebug()<<"grpc error"<<QString::fromStdString(status.error_message())
               <<QString::fromStdString(status.error_details());
        emit translated(std::nullopt);
    }

}
