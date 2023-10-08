#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    grid.resize(9, std::vector<char>(9, '.'));
    setWindowTitle("九宫格");
    for(int i = 0; i < 9; i++)
    {
        for(int j = 0; j < 9; j++)
        {
            ui->IputTable->setItem(i, j, new QTableWidgetItem);
            ui->IputTable->item(i, j)->setText("");
            ui->IputTable->item(i, j)->setTextAlignment(Qt::AlignCenter);
        }
    }
    connect(ui->btn_Input_Mode, &QPushButton::clicked, this, [=]()
    {
        this->setMaximumSize(356, 394);
        this->resize(356, 394);
        ui->stackedWidget->setCurrentIndex(0);
    });
    connect(ui->btn_Identify_Mode, &QPushButton::clicked, this, [=]()
    {
        this->setMaximumSize(356, 600);
        ui->stackedWidget->setCurrentIndex(1);
    });
    connect(&this->sc_shot, &ScreenShot::emitScreenShot, this, &MainWindow::setScreenshot);
    connect(ui->btn_Screnn_Shot, &QPushButton::clicked, this, [=](){ sc_shot.show();});
    connect(ui->btn_File_Open, &QPushButton::clicked, this, &MainWindow::GetFilePath);
    connect(ui->btn_File_Open_2, &QPushButton::clicked, this, &MainWindow::GetFilePath);
    connect(ui->btn_Calc_resultion, &QPushButton::clicked, this, &MainWindow::CalcGridReslution);
    connect(ui->btn_Reset_all, &QPushButton::clicked, this, &MainWindow::Resetall);
    connect(this, &MainWindow::GetFileInfo, this, &MainWindow::GetfileInfo_Slot);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::isvalidgrid()
{
    auto AreaToIndex = [](int x, int y) -> int
    {
        return (x / 3) * 3 + y / 3;
    };
    std::vector<int> row(9, 0), col(9, 0), area(9, 0);
    std::vector<std::vector<char>> temp(9, std::vector<char>(9, '.'));
    for(int i = 0; i < 9; i++)
    {
        for(int j = 0; j < 9; j++)
        {
            auto str = ui->IputTable->item(i, j)->text();
            if(str.isNull() || str == "")
                temp[i][j] = '.';
            else
            {
                if(str.toInt() < 1 || str.toInt() > 9)
                {
                    QMessageBox::critical(this, "错误", "输入非法");
                    return false;
                }
                else
                {

                    int tem = row[i] | col[j] | area[AreaToIndex(i, j)];
                    if((tem >> str.toInt()) & 1)
                    {
                        QMessageBox::critical(this, "错误", "输入非法");
                        return false;
                    }
                    temp[i][j] = str.toInt() + '0';
                    row[i] |= 1 << str.toInt();
                    col[j] |= 1 << str.toInt();
                    area[AreaToIndex(i, j)] |= 1 << str.toInt();
                }
            }
        }
    }
    grid = temp;
    return true;
}

void MainWindow::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
}

void MainWindow::GetFilePath()
{
    QString deskpath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    if(ui->stackedWidget->currentIndex() == 0)
        filepath = QFileDialog::getOpenFileName(this, "打开文件", deskpath, "文本文件(*txt)");
    else
        filepath = QFileDialog::getOpenFileName(this, "打开文件", deskpath, "图片文件(*png *bmp *jpg)");
    if(filepath.isEmpty())
        QMessageBox::warning(this, "警告", "未选择文件");
    else
        emit GetFileInfo();
}

void MainWindow::CalcGridReslution()
{
    if(!isvalidgrid())
        return;
    std::vector<int> memo, col(9, 0), row(9, 0), area(9, 0);
    auto AreaToIndex = [](int x, int y) -> int
    {
        return (x / 3) * 3 + y / 3;
    };
    auto query = [&](int x, int y, int target) -> bool
    {
        int temp = row[x] | col[y] | area[AreaToIndex(x, y)];
        if ((temp >> target) & 1)
            return false;
        row[x] |= 1 << target, col[y] |= 1 << target;
        area[AreaToIndex(x, y)] |= 1 << target;
        return true;
    };
    auto recover = [&](int x, int y, int target) -> void
    {
        row[x] &= ~(1 << target);
        col[y] &= ~(1 << target);
        area[AreaToIndex(x, y)] &= ~(1 << target);
    };
    for(int i = 0; i < 9; i++)
    {
        for(int j = 0; j < 9; j++)
        {
            if(grid[i][j] == '.')
                memo.push_back(i * 10 + j);
            else
            {
                row[i] |= 1 << (grid[i][j] - '0');
                col[j] |= 1 << (grid[i][j] - '0');
                area[AreaToIndex(i, j)] |= 1 << (grid[i][j] - '0');
            }
        }
    }
    int len = memo.size();
    std::function<bool(int)> dfs = [&](int index) -> bool
    {
        if(index == len)
            return true;
        auto x = memo[index] / 10;
        auto y = memo[index] % 10;
        for(int i = 1; i < 10; i++)
        {
            if(!query(x, y, i))
                continue;
            grid[x][y] = i + '0';
            if(dfs(index + 1))
                return true;
            recover(x, y, i);
        }
        return false;
    };
    dfs(0);
    /*--------------上面为计算部分------------------*/
    for(int i = 0; i < 9; i++)
    {
        for(int j = 0; j < 9; j++)
        {
            auto str = ui->IputTable->item(i, j)->text();
            if(!str.isEmpty())
                continue;
            ui->IputTable->item(i, j)->setText(QString(grid[i][j]));
            ui->IputTable->item(i, j)->setForeground(QBrush(QColor(57, 91, 91)));
        }
    }
}

void MainWindow::Resetall()
{
    for(int i = 0; i < 9; i++)
    {
        for(int j = 0; j < 9; j++)
        {
            ui->IputTable->item(i, j)->setText("");
            grid[i][j] = '.';
        }
    }
}

void MainWindow::GetfileInfo_Slot()
{
    QStringList temp = filepath.split('.');
    if(temp.back() != "txt")
        return;
    Resetall();
    QFile file(filepath);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::warning(this, "警告", "文件打开失败");
    QByteArray array = file.readAll();
    array.replace("\r\n", "\n");
    QList<QByteArray> list = array.split('\n');
    for(int i = 0; i < 9; i++)
    {
        for(int j = 0; j < 9; j++)
        {
            if(list[i][j] == '.')
                continue;
            ui->IputTable->item(i, j)->setText(QString((char)list[i][j]));
        }
    }
}

void MainWindow::setScreenshot(QImage shot)
{
    QPixmap temp = QPixmap::fromImage(shot);
    if(temp.isNull())
    {
        QMessageBox::warning(this, "警告", "标题为空");
        return;
    }
    QSize size(ui->Screen_View->size().width() * 2, ui->Screen_View->size().height() * 2);
    ui->Screen_View->setPixmap(temp.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

