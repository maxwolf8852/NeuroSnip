#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QRubberBand>
#include <QWidget>

class ScreenShot : public QWidget
{
    Q_OBJECT
public:
    explicit ScreenShot(QWidget *parent = nullptr);

    static std::optional<QPixmap> getScreenshot(QRect);

signals:
    void takeRegion(QRect);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;
private:
    QPoint begin = QPoint(0,0), end = QPoint(0,0);
    QRubberBand *drawer = nullptr;

};

#endif // SCREENSHOT_H
