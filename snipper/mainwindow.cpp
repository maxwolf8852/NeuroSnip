#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QClipboard>
#include <QMessageBox>
#include <iostream>

#include "../ColorConfidence/ColorConfidence.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/Nico.png"));

    maxwolf8852::setBounds(30.0f, 100.0f);
    maxwolf8852::setValue(180);

    scrs = new ScreenShot();
    rec = new Recognizer();
    trans = new Translator();

    recTh = new QThread();
    trTh = new QThread();

    trans->moveToThread(trTh);
    rec->moveToThread(recTh);

    connect(scrs, &ScreenShot::takeRegion, this, &MainWindow::getRegion);
    connect(rec, &Recognizer::recognized, this, &MainWindow::recognized);
    connect(this, &MainWindow::recognizeTh, rec, &Recognizer::recognize);

    connect(ui->actionNew_screenshot, &QAction::triggered, this, &MainWindow::takeScreenshot);
    connect(ui->actionExit, &QAction::triggered, [this](){ this->hide(); QApplication::quit();});
    connect(ui->actionCopy, &QAction::triggered, [this](){
        if(last.has_value()){
            QClipboard *clipboard = QGuiApplication::clipboard();
            clipboard->setPixmap( last.value(), QClipboard::Clipboard);
        }
    });
    ui->actionCopy->setEnabled(0);

    connect(this, &MainWindow::translateTh, trans, &Translator::setText);
    connect(trans, &Translator::translated, this, &MainWindow::translated);

    ui->actionNew_screenshot->setShortcut(QKeySequence((Qt::CTRL | Qt::SHIFT) + Qt::Key_S));

    this->setMinimumWidth(256);
    this->resize(256, this->height());

    this->setWindowTitle("NeuroSnip");

    this->hide();

    scrs->hide();

    takeScreenshot();


}

MainWindow::~MainWindow()
{
    if(recTh->isRunning()){
        recTh->quit();
    }

    if(trTh->isRunning()){
        trTh->quit();
    }


    recTh->wait();
    trTh->wait();


    delete recTh;
    delete trTh;

    delete trans;
    delete ui;
    delete rec;
    delete scrs;
}

void MainWindow::getRegion(QRect r)
{
    scrs->hide();

    auto screenshot = ScreenShot::getScreenshot(r);

    if(!screenshot.has_value()){
        QApplication::quit();
    }

    if(screenshot->height() / screenshot->width() > 5){
        this->hide();
        QMessageBox msgBox;
        msgBox.setText("Ошибка!");
        msgBox.setInformativeText("Слишком большое соотношение сторон\n (высотка / ширина > 5)");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int res = msgBox.exec();
        QApplication::quit();
    }

    if(screenshot->size().width()< 320){
        auto dw =  320 / screenshot->width();
        screenshot = screenshot->scaled(320, dw * screenshot->height());
    }

    ui->picture->setPixmap(screenshot.value());
    auto w = screenshot.value().width() + 20;

    setFixedWidth(w);
    resize(w, height());

    last = screenshot;
    ui->actionCopy->setEnabled(1);
    //resize(ui->picture->size());

    recTh->wait();
    recTh->start();
    emit recognizeTh(screenshot.value());

    this->show();
}

void MainWindow::recognized(std::optional<std::vector<std::pair<QString, float>>> eng)
{
    if(!eng.has_value()){
        ui->original->setTextColor(QColor(180,0,0));
        ui->original->setText("Ничего не найдено!");
        ui->original->setTextColor(QColor(0,0,0));
        return;
    }

    auto txt = eng.value();

    QString to_translate;

    for(auto& j: txt){
        auto color = maxwolf8852::confidence2Color(j.second);
        auto temp = j.first + " ";

        QTextCursor cursor( ui->original->textCursor() );

        QTextCharFormat format;
        format.setFontWeight( QFont::DemiBold );
        format.setForeground( QBrush( QColor(color[0], color[1], color[2]) ) );
        cursor.setCharFormat( format );

        cursor.insertText( temp );

        to_translate += temp;
    }

    trTh->wait();
    trTh->start();

    emit translateTh(to_translate);

}

void MainWindow::takeScreenshot()
{
    ui->original->clear();
    ui->translated->clear();


    if(recTh->isRunning()){
        recTh->quit();
    }
    if(trTh->isRunning()){
        trTh->quit();
    }

    QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
    this->hide();
    ui->original->clear();
    scrs->show();
}

void MainWindow::translated(std::optional<QString> text)
{
    if(!text.has_value()){
        ui->translated->setTextColor(QColor(180,0,0));
        ui->translated->setText("Ошибка перевода!");
        ui->translated->setTextColor(QColor(0,0,0));
        return;
    }
    ui->translated->setText(text.value());

}


