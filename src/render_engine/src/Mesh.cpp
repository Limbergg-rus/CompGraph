//
// Created by Ренат Асланов on 15.11.2024.
//

#include "../headers/Mesh.h"

#include <cmath>
#include <QPainter>
#include <iostream>

#include "../../math/headers/Point3D.h"

bool Mesh::show_mesh(float weight_a, float weight_b, float weight_c, int &r, int &g, int &b)
{
    if (weight_a <= 0.02f || weight_b <= 0.02f || weight_c <= 0.02f) {
        r = 255, g = 255, b = 255;
        return true;
    }
    return false;
}

bool Mesh::show_selection(float weight_a, float weight_b, float weight_c, int cursorX, int cursorY, int pixelX, int pixelY)
{
	int epsi = 4;
//	std::cout << cursorX - pixelX << ", " << cursorY- pixelY << std::endl;

	if ((abs(cursorX - pixelX)*abs(cursorX - pixelX) + abs(cursorY- pixelY)*abs(cursorY- pixelY) < epsi*epsi)) {
		return true;
	}

	return false;
}

void Mesh::show_mesh_by_points(QPainter &painter,Point3D A,Point3D B)
{
    int deltax = abs( B.getX() - A.getX() ) ;
    int deltay = abs( B.getY() - A.getY() ) ;
    int y = A.getY() ;
    int diry = B.getY() - A.getY() ;
    int error = 0;
    int deltaerr = ( deltay + 1) ;
    if ( diry > 0) diry = 1;
    if ( diry < 0) diry = -1;
    if(A.getX() < B.getX()) {
        for (int x = A.getX(); x < B.getX(); x++) {
            painter.drawPoint(x,y);
            error = error + deltaerr ;
            if ( error >= ( deltax + 1) )
                y = y + diry ;
            error = error - ( deltax + 1) ;
        }
    }
    else {
        for (int x = B.getX(); x < A.getX(); x++) {
            painter.drawPoint(x,y);
            error = error + deltaerr ;
            if ( error >= ( deltax + 1) )
                y = y + diry ;
            error = error - ( deltax + 1) ;
        }
    }

}