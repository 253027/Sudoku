#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QRect>
#include <QGuiApplication>
#include <QScreen>

class ScreenShot final: public QWidget
{
    Q_OBJECT
public:
    explicit ScreenShot(QWidget *parent = nullptr);

signals:
    void emitScreenShot(QImage shot);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    bool iscapturing;
    QPoint start_pos, end_pos;

};

#endif // SCREENSHOT_H
