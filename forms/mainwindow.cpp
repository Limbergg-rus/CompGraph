//
// Created by Ренат Асланов on 24.10.2024.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "../headers/mainwindow.h"
#include "../ui_MainWindow.h"
#include "../headers/RenderEngine.h"
#include <QMessageBox>
#include <QPainter>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QMainWindow::paintEvent(event);

    QPainter painter(this); // Создаем объект QPainter

    // Здесь можно использовать painter для рисования на окне
    for(const Model &model : models) { // Используйте ссылку на модель
        RenderEngine::render(painter, Vector3D(1, 1, 1,), model, 800, 600, 0); // Передаем painter по ссылке
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionLoad_Model_triggered()
{
    QMessageBox::information(this, "Load model", "Coming soon...");
}

void MainWindow::on_actionSave_Model_triggered()
{
    QMessageBox::information(this, "Save", "Coming soon...");
}
