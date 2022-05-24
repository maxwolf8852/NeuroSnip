#include "screenshot.h"
#include <QApplication>
#include <QMouseEvent>
#include <QWindow>
#include <QScreen>

ScreenShot::ScreenShot(QWidget *parent) : QWidget(parent, Qt::Dialog)
{
    drawer = new QRubberBand(QRubberBand::Rectangle, this);

    QPalette pal;
    pal.setBrush(QPalette::Highlight, QBrush(Qt::red));
    drawer->setPalette(pal);


    showFullScreen();
    setWindowOpacity(0.4);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
}

std::optional<QPixmap> ScreenShot::getScreenshot(QRect r)
{
    if(r.isEmpty())
        return std::nullopt;

    QScreen *screen = QGuiApplication::primaryScreen();

    return screen->grabWindow(0, r.x(), r.y(), r.width(), r.height());
}

void ScreenShot::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton){
        begin = event->pos();
        drawer->setGeometry(QRect(begin, QSize()));
        drawer->show();
    }
}

void ScreenShot::mouseMoveEvent(QMouseEvent *event)
{
    drawer->setGeometry(QRect(begin, event->pos()).normalized());
}

void ScreenShot::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton){
        end = event->pos();
        drawer->hide();
        this->hide();

        emit takeRegion(QRect(begin, end));


        QApplication::restoreOverrideCursor();

    }
}

void ScreenShot::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape){
        QApplication::restoreOverrideCursor();
        QApplication::quit();
    }
}


