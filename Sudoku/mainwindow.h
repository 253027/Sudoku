#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <vector>
#include <QImage>
#include <QTableWidgetItem>
#include <QEvent>
#include <QFile>

#include "screenshot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void GetFileInfo();

public slots:
    void setScreenshot(QImage shot);

protected:
    void focusOutEvent(QFocusEvent *event) override;

private:
    Ui::MainWindow *ui;
    QString filepath;
    ScreenShot sc_shot;
    std::vector<std::vector<char>> grid;
    bool isvalidgrid();

private slots:
    void GetFilePath();
    void CalcGridReslution();
    void Resetall();
    void GetfileInfo_Slot();
};
#endif // MAINWINDOW_H
