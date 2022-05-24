#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include "screenshot.h"
#include "recognizer.h"
#include "translator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void recognizeTh(const QPixmap&);
    void translateTh(QString);

private slots:
    void getRegion(QRect);
    void recognized(std::optional<std::vector<std::pair<QString, float>>>);

    void takeScreenshot();
    void translated(std::optional<QString>);

private:
    Ui::MainWindow *ui;

    QThread *recTh = nullptr, *trTh = nullptr;

    ScreenShot *scrs = nullptr;
    Recognizer *rec = nullptr;
    Translator *trans = nullptr;

    std::optional<QPixmap> last = std::nullopt;



};
#endif // MAINWINDOW_H
