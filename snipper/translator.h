#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QObject>

//#include "LT/LibreTranslate.h"
#include <windows.h>
#include "grpc_cli/cli_g.grpc.pb.h"
#include "grpc_cli/cli_g.pb.h"
#include <grpc++/grpc++.h>

class Translator : public QObject
{
    Q_OBJECT
public:
    explicit Translator(QObject *parent = nullptr);
    ~Translator();

signals:
    void translated(std::optional<QString>);

public slots:
    void setText(QString);

private:
    //LibreTranslateAPI libreTranslateAPI;

    std::shared_ptr<grpc::Channel> channel = nullptr;
    std::unique_ptr< serv_g::ServG::Stub> stub = nullptr;

};

#endif // TRANSLATOR_H
