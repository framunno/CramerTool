#include "mainwindow.h"
#include "./ui_mainwindow.h"

#define SWVERSION "1.0"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    std::for_each(matrix.begin(), matrix.end(), [](auto& row)
    {
        std::for_each(row.begin(), row.end(), [](auto& element)
        {
            element = 0.0;
        });
    });

    detX = 0;
    detY = 0;
    detZ = 0;
    det  = 0;
    Anum = 0;
    Bnum = 0;
    Cnum = 0;
    Xnum = 0;
    Ynum = 0;
    Znum = 0;

    ui->setupUi(this);
    setWindowTitle(tr("CramerTool v%1").arg(SWVERSION));
    setFixedSize(580, 509);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_CLEARbutton_clicked()
{
    ui->x1->setText("");
    ui->y1->setText("");
    ui->z1->setText("");

    ui->x2->setText("");
    ui->y2->setText("");
    ui->z2->setText("");

    ui->x3->setText("");
    ui->y3->setText("");
    ui->z3->setText("");

    ui->X->setText("");
    ui->Y->setText("");
    ui->Z->setText("");

    ui->A->setText("");
    ui->B->setText("");
    ui->C->setText("");
}


void MainWindow::on_SOLVEbutton_clicked()
{
    bool ret = false;
    matrix[0][0] = ui->x1->toPlainText().toDouble(&ret);

    if (ret)
    {
        matrix[0][1] = ui->y1->toPlainText().toDouble(&ret);
    }

    if (ret)
    {
        matrix[0][2] = ui->z1->toPlainText().toDouble(&ret);
    }

    if (ret)
    {
        matrix[1][0] = ui->x2->toPlainText().toDouble(&ret);
    }

    if (ret)
    {
        matrix[1][1] = ui->y2->toPlainText().toDouble(&ret);
    }

    if (ret)
    {
        matrix[1][2] = ui->z2->toPlainText().toDouble(&ret);
    }

    if (ret)
    {
        matrix[2][0] = ui->x3->toPlainText().toDouble(&ret);
    }

    if (ret)
    {
        matrix[2][1] = ui->y3->toPlainText().toDouble(&ret);
    }

    if (ret)
    {
        matrix[2][2] = ui->z3->toPlainText().toDouble(&ret);
    }

    if (ret)
    {
        Anum = ui->A->toPlainText().toDouble(&ret);
    }

    if (ret)
    {
        Bnum = ui->B->toPlainText().toDouble(&ret);
    }

    if (ret)
    {
        Cnum = ui->C->toPlainText().toDouble(&ret);
    }

    if (ret)
    {
        det  =  matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) -
              matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0]) +
              matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);

        detX =  Anum * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) -
               Bnum * (matrix[0][1] * matrix[2][2] - matrix[0][2] * matrix[2][1]) +
               Cnum * (matrix[0][1] * matrix[1][2] - matrix[0][2] * matrix[1][1]);

        detY = -Anum * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0]) +
               Bnum * (matrix[0][0] * matrix[2][2] - matrix[0][2] * matrix[2][0]) -
               Cnum * (matrix[0][0] * matrix[1][2] - matrix[0][2] * matrix[1][0]);

        detZ =  Anum * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]) -
               Bnum * (matrix[0][0] * matrix[2][1] - matrix[0][1] * matrix[2][0]) +
               Cnum * (matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0]);

        if (det != 0)
        {
            Xnum = detX/det;
            Ynum = detY/det;
            Znum = detZ/det;

            ui->X->setText(QString::number(Xnum));
            ui->Y->setText(QString::number(Ynum));
            ui->Z->setText(QString::number(Znum));
        }
        else
        {
            ui->X->setText("IMP");
            ui->Y->setText("IMP");
            ui->Z->setText("IMP");
        }
    }
    else
    {
        ui->X->setText("IMP");
        ui->Y->setText("IMP");
        ui->Z->setText("IMP");
    }
}

