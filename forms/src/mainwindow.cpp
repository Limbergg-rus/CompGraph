#include "../headers/mainwindow.h"
#include "../ui_MainWindow.h"
#include "../../render_engine/headers/RenderEngine.h"
#include "../../objreader/headers/ObjReader.h"
#include "../../objwriter/headers/ObjWriter.h"
#include "../../render_engine/headers/GraphicConveyor.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QColorDialog>
#include <QPainter>
#include <QPixmap>
#include <QGraphicsPixmapItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      camera(Vector3D(0, 0, 100), Vector3D(0, 0, 0),
             1.0f, 1, 0.01f, 100),
      ui(new Ui::MainWindow), scene(std::make_unique<QGraphicsScene>(this))
{
    ui->setupUi(this);

	ui->graphicsView->setScene(scene.get());
}

// БЕСПОНТОВЫЙ МЕТОД, ОСТАВЛЮ ЕГО ТРУП НА ВСЯКИЙ СЛУЧАЙ

//void MainWindow::paintEvent(QPaintEvent *event)
//{
//    Q_UNUSED(event);
//    QMainWindow::paintEvent(event);
//
//    QPainter painter(this); // Создаем объект QPainter
//
//    camera.setAspectRatio(static_cast<float>(this->width()) / static_cast<float>(this->height()));
//    int width = this->width();
//    int height = this->height();
//
//    for (Model &model: models) {
//        QColor basic_color = QColor(255, 255, 255);
//        //При низких значениях rgb, перестает рабоать (цвете 0,0,0 все ломается)
//        RenderEngine renderEngine(painter, camera, model_texture_path, basic_color, model, width,
//                                  height, show_mesh, show_texture, show_illumination);
//        renderEngine.render();
//    }
//}

void MainWindow::update_scene()
{
	scene->clear();
	int width = ui->graphicsView->viewport()->width();
	int height = ui->graphicsView->viewport()->height();

	QPixmap pixmap(width, height);
	pixmap.fill(Qt::black);
	QPainter painter(&pixmap);

	camera.setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
	for (Model& model : models)
	{
		QColor basic_color = QColor(255, 255, 255);
		RenderEngine renderEngine(painter, camera, model_texture_path, basic_color, model, width,
				height, show_mesh, show_texture, show_illumination);
		renderEngine.render();
	}

	auto item = std::make_unique<QGraphicsPixmapItem>(pixmap);
	scene->addItem(item.release());
	ui->graphicsView->update();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionLoad_Model_triggered()
{
    std::string file_name = QFileDialog::getOpenFileName(this,
                                                         tr("Open Object"), ":/",
                                                         tr("Object Files (*.obj)")).toUtf8().constData();

    //TODO Переделать когда нужно будет делать сценку
    if (models.size() == 1)
	{
        models[0] = (ObjReader::read(file_name));
    }
	else
	{
        models.emplace_back(ObjReader::read(file_name));
    }
	update_scene();
}

void MainWindow::on_actionSave_Model_triggered()
{
    if (models.empty()) {
        QMessageBox::information(this, "Save model", "You haven't selected a model");
        return;
    }
    std::string file_name = QFileDialog::getSaveFileName(this, tr("Save Object"),
                                                         ":/",
                                                         tr("Objects (*.obj)")).toUtf8().constData();
    ObjWriter::write(models[0], file_name);
}

void MainWindow::on_actionUp_triggered()
{
    camera.movePosition(Vector3D(0, static_cast<float>(TRANSLATION), 0));
    update_scene();
}

void MainWindow::on_actionDown_triggered()
{
    camera.movePosition(Vector3D(0, static_cast<float>(-TRANSLATION), 0));
	update_scene();
}

void MainWindow::on_actionLeft_triggered()
{
    camera.movePosition(Vector3D(static_cast<float>(TRANSLATION), 0, 0));
	update_scene();
}

void MainWindow::on_actionRight_triggered()
{
    camera.movePosition(Vector3D(static_cast<float>(-TRANSLATION), 0, 0));
	update_scene();
}

void MainWindow::on_actionForward_triggered()
{
    camera.movePosition(Vector3D(0, 0, static_cast<float>(-TRANSLATION)));
	update_scene();
}

void MainWindow::on_actionBack_triggered()
{
    camera.movePosition(Vector3D(0, 0, static_cast<float>(TRANSLATION)));
	update_scene();
}

void MainWindow::on_actionLoad_Texture_triggered()
{
    std::string file_name = QFileDialog::getOpenFileName(this,
                                                         tr("Open Texture"), ":/",
                                                         tr("Object Image (*.png *.jpg *.bmp)")).toUtf8().constData();
    //TODO Переделать когда нужно будет делать сценку
    model_texture_path = file_name;
}

void MainWindow::on_actionChose_Color_triggered()
{
    QColor color = QColorDialog::getColor(QColor(255, 100, 200, 255));
    fill_model_color = color;
	update_scene();
    // if (!color.isValid()) {
    // Cancel
    // }
    // QMessageBox::information(this, "Choose Color", "Cooming soon...");
}

void MainWindow::on_actionTriangulation_changed()
{
    triangulation = !triangulation;
	update_scene();
}

void MainWindow::on_actionRotate_Scale_Translate_triggered()
{
    GraphicConveyor::rotate_scale_translate(models[0], 1, 1, 1, 1, 1, 1, 1, 1, 1);
	update_scene();
}

void MainWindow::on_checkBox_show_mesh_toggled(bool checked)
{
    show_mesh = !show_mesh;
	update_scene();
    // QMessageBox::information(this, "Save model", "Today is monday");
}

void MainWindow::on_checkBox_show_texture_toggled(bool checked)
{
    if(model_texture_path.empty() ) {
        QMessageBox::information(this, "OSHIBKA", "Elki palki, shachalo model' zagruzi, yo, 52");
        // ui->checkBox_show_texture->toggled(false);
    }
    show_texture = !show_texture;
	update_scene();
}

void MainWindow::on_checkBox_show_illumination_toggled(bool checked)
{
    show_illumination = !show_illumination;
	update_scene();
    
    // QMessageBox::information(this, "Save model", "Today is tuesday");
}
