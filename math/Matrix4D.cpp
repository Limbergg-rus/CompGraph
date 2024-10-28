#include "../headers/Matrix4D.h"
#include "../headers/Vector4D.h"

#include <vector>

Matrix4D Matrix4D::operator * (const Matrix4D& m)
{
    int rowsA = this->matrix.size();
    int colsA = this->matrix[0].size();
    int rowsB = m.matrix.size();
    int colsB = m.matrix.size();

    std::vector<std::vector<float>> ans(rowsA, std::vector<float>(colsB, 0.0f));

    for (int i = 0; i < rowsA; i++) 
    {
        for (int j = 0; j < colsB; j++) 
        {
            for (int k = 0; k < colsA; k++) 
            {
                ans[i][j] += this->matrix[i][k] * m.matrix[k][j];
            }
        }
    }

    return Matrix4D(ans);
}

Vector4D Matrix4D::operator * (const Vector4D& v)
{
    return Vector4D(this->matrix[0][0] * v.getX() + this->matrix[0][1] * v.getY() + this->matrix[0][2] * v.getZ() + this->matrix[0][3] * v.getW(),
        this->matrix[1][0] * v.getX() + this->matrix[1][1] * v.getY() + this->matrix[1][2] * v.getZ() + this->matrix[1][3] * v.getW(),
        this->matrix[2][0] * v.getX() + this->matrix[2][1] * v.getY() + this->matrix[2][2] * v.getZ() + this->matrix[2][3] * v.getW(),
        this->matrix[3][0] * v.getX() + this->matrix[3][1] * v.getY() + this->matrix[3][2] * v.getZ() + this->matrix[3][3] * v.getW()
    );
}
