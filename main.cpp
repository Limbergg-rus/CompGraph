#include "src/math/headers/Vector2D.h"
#include "src/forms/mainwindow.h"
#include "src/obj_utils/objreader/ObjReader.h"

#include <iostream>
#include <QApplication>
#include <QPushButton>
#include <omp.h>

int main(int argc, char *argv[])
{
    //std::string filename("C:/Users/Пользователь/CLionProjects/3DModels/Faceform/WrapBody.obj");
    // std::string filename("C:/Users/Пользователь/CLionProjects/3DModels/Faceform/WrapBody.obj");
    // std::string filename("C:/Users/Пользователь/CLionProjects/3DModels/Faceform/WrapBody.obj");
    //std::string filename("C:/Users/Пользователь/CLionProjects/3DModels/Faceform/WrapBody.obj");

    // std::string filename("/Users/renat/CLionProjects/3DModels/Faceform/WrapHand.obj");
    //std::string filename("/Users/renat/CLionProjects/3DModels/Faceform/WrapHand.obj");

    // std::string filename("/Users/renat/CLionProjects/3DModels/CaracalCube/caracal_cube.obj");
    // std::string filename("/Users/renat/CLionProjects/3DModels/Faceform/WrapBody.obj");
    // std::string filename("/Users/renat/CLionProjects/3DModels/Faceform/WrapHead.obj");
    // std::string filename("/Users/renat/CLionProjects/3DModels/Faceform/WrapLowerTeeth.obj");
    // std::string filename("C:/Users/Пользователь/CLionProjects/3DModels/Faceform/WrapSkull.obj");
    // std::string filename("/Users/renat/CLionProjects/3DModels/Faceform/WrapSkull.obj");
    //std::string filename("/Users/renat/CLionProjects/3DModels/SimpleModelsForReaderTests/NonManifold2.obj");
    // std::string filename("/Users/renat/CLionProjects/3DModels/Faceform/WrapUpperTeeth.obj");
    // std::string filename("/Users/renat/CLionProjects/3DModels/Faceform/WrapFemaleBody.obj");
//    std::string filename( "/Users/Пользователь/CLionProjects/3DModels/CaracalCube/caracal_cube.obj"); // МОЙ КУБИК :3


/* ЭТОТ КУБИК ТОЖЕ БУДЕТ МОИМ БУГАГА*/
	std::string filename( "/Users/Пользователь/CLionProjects/3DModels/CaracalCube/caracal_cube.obj");

    // Model ans(ObjReader::read(filename));
	omp_set_dynamic(1); // Enable dynamic adjustment
    QApplication a(argc, argv);
    MainWindow w;
	w.setWindowTitle("Toster");
	w.setWindowIcon(QIcon(":/toster.png"));
//    w.models.emplace_back(ObjReader::read(filename));
    // w.models.emplace_back(ans);
	w.setWindowFlags(Qt::FramelessWindowHint);
    w.showFullScreen();

    return a.exec();
}
