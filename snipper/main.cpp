#include "mainwindow.h"
#include <QApplication>
#include <opencv2/dnn.hpp>


template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    os << "[";
    for (int i = 0; i < v.size(); ++i)
    {
        os << v[i];
        if (i != v.size() - 1)
        {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"Russian");
    QApplication a(argc, argv);
    MainWindow w;
    return a.exec();
}
