#include "../headers/RenderEngine.h"

#include <iostream>

#include "../headers/GraphicConveyor.h"
#include "../../forms/headers/mainwindow.h"
#include "../../math/headers/DepthBuffer.h"
#include "../../math/headers/Point2D.h"
#include "../headers/TypeOfRender.h"

void RenderEngine::render(QPainter &painter,
                          Camera &camera,
                          std::string &filename,
                          const Model &mesh,
                          const int &width,
                          const int &height,
                          const bool &show_triangulation)
{
    DepthBuffer depth_buffer(width, height);
    const Matrix4D model_matrix = Matrix4D::create_identity_matrix();
    const Matrix4D view_matrix = camera.get_view_matrix();
    const Matrix4D projection_matrix = camera.get_projection_matrix();

    Matrix4D model_view_projection_matrix(model_matrix);
    model_view_projection_matrix.mul(view_matrix);
    model_view_projection_matrix.mul(projection_matrix);
    if (show_triangulation) {
        render_triangles(painter, mesh, width, height,
                         model_view_projection_matrix, static_cast<int>(mesh.triangles.size()), depth_buffer, filename, camera);
    } else {
        render_polygons(painter, mesh, width, height, model_view_projection_matrix,
                        static_cast<int>(mesh.polygons.size()));
    }
}

void RenderEngine::add_polygons_vertex(const Model &mesh, const int &width, const int &height,
                                       const Matrix4D &model_view_projection_matrix, const int polygon_ind,
                                       const int n_vertices_in_polygon,
                                       std::vector<Point2D> &result_points)
{
    for (int vertex_in_polygon_ind = 0; vertex_in_polygon_ind < n_vertices_in_polygon; ++vertex_in_polygon_ind) {
        Vector3D vertex = mesh.vertices[mesh.polygons[polygon_ind].get_vertex_indices()[vertex_in_polygon_ind]];
        Vector3D vertex_vecmath(vertex.getX(), vertex.getY(), vertex.getZ());
        Point2D result_point = Point2D::vertex_to_point(
            Matrix4D::multiply_matrix4d_by_vector3d(model_view_projection_matrix, vertex_vecmath), width, height);
        result_points.emplace_back(result_point);
    }
}

void RenderEngine::add_triangles_vertex(const Model &mesh, const int &width, const int &height,
                                        const Matrix4D &model_view_projection_matrix, int triangle_ind,
                                        int n_vertices_in_polygon,
                                        std::vector<Point3D> &result_points, std::vector<Point3D> &world_vertex)
{
    for (int vertex_in_triangle_ind = 0; vertex_in_triangle_ind < n_vertices_in_polygon; ++vertex_in_triangle_ind) {
        Vector3D vertex = mesh.vertices[mesh.triangles[triangle_ind].get_vertex_indices()[vertex_in_triangle_ind]];
        Vector3D vertex_vecmath(vertex.getX(), vertex.getY(), vertex.getZ());
        world_vertex.emplace_back(Point3D(vertex.getX(), vertex.getY(), vertex.getZ()));
        Point3D result_point = Point3D::vertex_to_point(
            Matrix4D::multiply_matrix4d_by_vector3d(model_view_projection_matrix, vertex_vecmath), width, height,
            vertex.getZ());
        result_points.emplace_back(result_point);
    }
}

void RenderEngine::add_normal_vertex(const Model &mesh, int triangle_ind, int n_vertices_in_polygon,
                                     std::vector<Point3D> &normal_points)
{
    for (int vertex_in_triangle_ind = 0; vertex_in_triangle_ind < n_vertices_in_polygon; ++vertex_in_triangle_ind) {
        int texture_vertex_ind = mesh.triangles[triangle_ind].get_normal_indices()[vertex_in_triangle_ind];
        Point3D result_point = {
            mesh.normals[texture_vertex_ind].getX(), mesh.normals[texture_vertex_ind].getY(),
            mesh.normals[texture_vertex_ind].getZ()
        };
        normal_points.emplace_back(result_point);
    }
}

void RenderEngine::add_texture_vertex(const Model &mesh, int triangle_ind, int n_vertices_in_polygon,
                                      std::vector<Point2D> &texture_vectors)
{
    for (int vertex_in_triangle_ind = 0; vertex_in_triangle_ind < n_vertices_in_polygon; ++vertex_in_triangle_ind) {
        int texture_vertex_ind = mesh.triangles[triangle_ind].get_texture_indices()[vertex_in_triangle_ind];
        Point2D result_point = {
            mesh.textureVertices[texture_vertex_ind].getX(), mesh.textureVertices[texture_vertex_ind].getY()
        };
        texture_vectors.emplace_back(result_point);
    }
}


void RenderEngine::draw_points(QPainter &painter, const int point_count,
                               const std::vector<Point2D> &result_points)
{
    for (int point_ind = 1; point_ind < point_count; ++
         point_ind) {
        painter.drawLine(
            static_cast<int>(result_points[point_ind - 1].getX()),
            static_cast<int>(result_points[point_ind - 1].getY()),
            static_cast<int>(result_points[point_ind].getX()),
            static_cast<int>(result_points[point_ind].getY()));
    }

    if (point_count > 0)
        painter.drawLine(
            static_cast<int>(result_points[point_count - 1].getX()),
            static_cast<int>(result_points[point_count - 1].getY()),
            static_cast<int>(result_points[0].getX()),
            static_cast<int>(result_points[0].getY()));
}

void RenderEngine::rasterization(QPainter &painter,
                                 const std::vector<Point3D> &result_points, DepthBuffer &depth_buffer)
{
    Point3D A = result_points[0], B = result_points[1], C = result_points[2], P;

    const int x_left = static_cast<int>(std::min({
        A.getX(), B.getX(), C.getX(), static_cast<float>(depth_buffer.getWidth())
    }));
    const int x_right = static_cast<int>(std::max({A.getX(), B.getX(), C.getX(), 0.0f}));
    const int y_down = static_cast<int>(std::min({
        A.getY(), B.getY(), C.getY(), static_cast<float>(depth_buffer.getHeight())
    }));
    const int y_up = static_cast<int>(std::max({A.getY(), B.getY(), C.getY(), 0.0f}));

    for (int y = y_down; y < y_up; y++) {
        for (int x = x_left; x < x_right; x++) {
            if (x < 0 || x > depth_buffer.getWidth() || y > depth_buffer.getHeight() || y < 0) {
                break;
            }
            P.set(x, y, 0);
            const float ABP = edgeFunction(A, B, P);
            const float BCP = edgeFunction(B, C, P);
            const float CAP = edgeFunction(C, A, P);

            if (ABP >= 0 && BCP >= 0 && CAP >= 0) {
                const float ABC = edgeFunction(A, B, C);
                const float weightA = BCP / ABC;
                const float weightB = CAP / ABC;
                const float weightC = ABP / ABC;
                int z = A.getZ() * weightA + B.getZ() * weightB + C.getZ() * weightC;
                painter.setPen(QColor(255, 140, 0));
                if (depth_buffer.get(x, y) > z) {
                    depth_buffer.set(x, y, z);
                    painter.drawPoint(P.getX(), P.getY());
                }
            }
        }
    }
}

void RenderEngine::show_mesh(QPainter &painter, std::vector<Point3D> &result_points, DepthBuffer &depth_buffer)
{
    for (int i = 1; i < result_points.size(); ++i) {
        RenderEngine::draw_line(painter, result_points[i - 1], result_points[i]);
    }
    if (result_points.size() > 0) {
        RenderEngine::draw_line(painter, result_points[result_points.size() - 1], result_points[0]);
    }

    // float x_left = std::min({A.getX(), B.getX(), C.getX()});
    // float x_right = std::max({A.getX(), B.getX(), C.getX()});
    // float y_down = std::min({A.getY(), B.getY(), C.getY()});
    // float y_up = std::max({A.getY(), B.getY(), C.getY()});

    // for (int y = y_down; y < y_up; y++) {
    //     for (int x = x_left; x < x_right; x++) {
    //         if (x < 0 || x * y > depth_buffer.size() || y < 0) {
    //             break;
    //         }
    //         P.set(x, y, 0);
    //         const float ABC = edgeFunction(A, B, C);
    //         const float ABP = edgeFunction(A, B, P);
    //         const float BCP = edgeFunction(B, C, P);
    //         const float CAP = edgeFunction(C, A, P);
    //         const float weightA = BCP / ABC;
    //         const float weightB = CAP / ABC;
    //         const float weightC = ABP / ABC;
    //         if (ABP >= 0 && BCP >= 0 && CAP >= 0) {
    //             float z = A.getZ() * weightA + B.getZ() * weightB + C.getZ() * weightC;
    //             painter.setPen(QColor(-z * 10, -z * 10, -z * 10));
    //             if (depth_buffer.get(x, y) > z) {
    //                 depth_buffer.set(x, y, z);
    //                 painter.drawPoint(P.getX(), P.getY());
    //             }
    //         }
    //     }
    // }
}

void RenderEngine::render_texture(QPainter &painter, std::vector<Point3D> &result_points, DepthBuffer &depth_buffer,
                                  std::vector<Point2D> &textures)
{
    QImage image("/Users/renat/CLionProjects/3DModels/CaracalCube/caracal_texture.png");

    Point3D A = result_points[0], B = result_points[1], C = result_points[2], P;

    const int x_left = static_cast<int>(std::min({
        A.getX(), B.getX(), C.getX(), static_cast<float>(depth_buffer.getWidth())
    }));
    const int x_right = static_cast<int>(std::max({A.getX(), B.getX(), C.getX(), 0.0f}));
    const int y_down = static_cast<int>(std::min({
        A.getY(), B.getY(), C.getY(), static_cast<float>(depth_buffer.getHeight())
    }));
    const int y_up = static_cast<int>(std::max({A.getY(), B.getY(), C.getY(), 0.0f}));

    for (int y = y_down; y < y_up; y++) {
        for (int x = x_left; x < x_right; x++) {
            if (x < 0 || x > depth_buffer.getWidth() || y > depth_buffer.getHeight() || y < 0) {
                break;
            }
            P.set(x, y, 0);
            const float ABP = edgeFunction(A, B, P), BCP = edgeFunction(B, C, P), CAP = edgeFunction(C, A, P);

            if (ABP >= 0 && BCP >= 0 && CAP >= 0) {
                const float ABC = edgeFunction(A, B, C);
                const float weightA = BCP / ABC, weightB = CAP / ABC, weightC = ABP / ABC;

                float u = weightA * textures[0].getX() + weightB * textures[1].getX() + weightC * textures[2].
                          getX();
                float v = weightA * textures[0].getY() + weightB * textures[1].getY() + weightC * textures[2].
                          getY();
                float z = A.getZ() * weightA + B.getZ() * weightB + C.getZ() * weightC;

                if (depth_buffer.get(x, y) > z) {
                    int texX = static_cast<int>((image.width() - 1) - u * (image.width() - 1));
                    int texY = static_cast<int>((image.height() - 1) - v * (image.height() - 1));
                    // std::cout << "texX: " << texX << " texY: " << texY << std::endl;
                    texX = std::clamp(texX, 0, image.width() - 1);
                    texY = std::clamp(texY, 0, image.height() - 1);

                    QColor texColor = image.pixel(texX, texY);
                    painter.setPen(texColor);
                    // painter.setPen(QColor(255* weightA ,255 * weightB, 255 * weightC));
                    painter.drawPoint(x, y);
                    depth_buffer.set(x, y, z);
                }
            }
        }
    }
}

float RenderEngine::edgeFunction(Point3D a, Point3D b, Point3D c)
{
    return (b.getX() - a.getX()) * (c.getY() - a.getY()) - (b.getY() - a.getY()) * (c.getX() - a.getX());
}

void RenderEngine::render_illumination(QPainter &painter, std::vector<Point3D> &result_points,
                                       DepthBuffer &depth_buffer,
                                       std::vector<Point3D> &illumination, Camera &camera)
{
    // P в локальных координатах, необходимо перевести в мировые
    Point3D A = result_points[0], B = result_points[1], C = result_points[2], P;
    // Инициализация вектора нормали
    Vector3D normal_A = Point3D::point_to_vector(illumination[0]).normalize(), normal_B =
            Point3D::point_to_vector(illumination[1]).normalize(), normal_C = Point3D::point_to_vector(illumination[2]).
            normalize();

    const int x_left = static_cast<int>(std::min({
        A.getX(), B.getX(), C.getX(), static_cast<float>(depth_buffer.getWidth())
    }));
    const int x_right = static_cast<int>(std::max({A.getX(), B.getX(), C.getX(), 0.0f}));
    const int y_down = static_cast<int>(std::min({
        A.getY(), B.getY(), C.getY(), static_cast<float>(depth_buffer.getHeight())
    }));
    const int y_up = static_cast<int>(std::max({A.getY(), B.getY(), C.getY(), 0.0f}));

    for (int y = y_down; y < y_up; y++) {
        for (int x = x_left; x < x_right; x++) {
            if (x < 0 || x > depth_buffer.getWidth() || y > depth_buffer.getHeight() || y < 0) {
                break;
            }
            P.set(x, y, 0);
            const float ABP = edgeFunction(A, B, P), BCP = edgeFunction(B, C, P), CAP = edgeFunction(C, A, P);

            if (ABP >= 0 && BCP >= 0 && CAP >= 0) {
                const float ABC = edgeFunction(A, B, C);
                const float weightA = BCP / ABC, weightB = CAP / ABC, weightC = ABP / ABC;

                int z = A.getZ() * weightA + B.getZ() * weightB + C.getZ() * weightC;
                //Вычисление общего веткора по барецентрической формуле

                Vector3D vn = (normal_A * weightA + normal_B * weightB + normal_C * weightC).normalize();

                Vector3D cam{camera.getPosition().getX(), camera.getPosition().getY(), camera.getPosition().getZ()};
                Vector3D ray = (cam - Vector3D{P.getX(), P.getY(), P.getZ()}).normalize();

                //Луч
                // Vector3D ray(1, 1, 0);
                ray = ray.normalize();

                float k = 0.4;
                // Вычисление нормированного коэфицента
                float l = -(ray * vn);
                if (l < 0.0f) {
                    l = 0.0f;
                }
                float r = 255.0f;
                float g = 140.0f;
                float b = 0.0f;
                // float a1 = r*(1 - k + k*l);
                // float a2 = g*(1 - k + k*l);
                // float a3 = b*(1 - k + k*l);
                painter.setPen(QColor(r * (1 - k + k * l), g * (1 - k + k * l), b * (1 - k + k * l)));
                if (depth_buffer.get(x, y) > z) {
                    depth_buffer.set(x, y, z);
                    painter.drawPoint(P.getX(), P.getY());
                }
            }
        }
    }
}

void RenderEngine::draw_line(QPainter &painter, Point3D &A, Point3D &B)
{
    // painter.setPen(QColor(0, 0, 0));
    float x1 = B.getX();
    float y1 = B.getY();
    float x0 = A.getX();
    float y0 = A.getY();

    float dx = abs(x1 - x0);
    float sx = x0 < x1 ? 1 : -1;
    float dy = -abs(y1 - y0);
    float sy = y0 < y1 ? 1 : -1;
    float error = dx + dy;

    while (true) {
        painter.drawPoint(x0, y0);
        if (x0 == x1 && y0 == y1) break;
        float e2 = 2 * error;
        if (e2 >= dy) {
            error += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            error += dx;
            y0 += sy;
        }
    }
}

void RenderEngine::render_polygons(QPainter &painter, const Model &mesh, const int &width, const int &height,
                                   const Matrix4D &model_view_projection_matrix, const int n_polygons)
{
    for (int polygon_ind = 0; polygon_ind < n_polygons; ++polygon_ind) {
        const int n_vertices_in_polygon = static_cast<int>(mesh.polygons[polygon_ind].get_vertex_indices().size());
        std::vector<Point2D> result_points;
        add_polygons_vertex(mesh, width, height, model_view_projection_matrix, polygon_ind, n_vertices_in_polygon,
                            result_points);
        draw_points(painter, n_vertices_in_polygon, result_points);
    }
}

void RenderEngine::initialize_loop_varibles(DepthBuffer &depth_buffer, Point3D &A, Point3D &B, Point3D &C,
                                            int &x_left, int &x_right, int &y_down, int &y_up)
{
    x_left = static_cast<int>(std::min({
        A.getX(), B.getX(), C.getX(), static_cast<float>(depth_buffer.getWidth())
    }));
    x_right = static_cast<int>(std::max({A.getX(), B.getX(), C.getX(), 0.0f}));
    y_down = static_cast<int>(std::min({
        A.getY(), B.getY(), C.getY(), static_cast<float>(depth_buffer.getHeight())
    }));
    y_up = static_cast<int>(std::max({A.getY(), B.getY(), C.getY(), 0.0f}));
}

float RenderEngine::calculate_parametr_of_illumination(std::vector<Point3D> &normal_vectors, Camera &camera, Point3D P,
                                                       const float weightA, const float weightB, const float weightC)
{
    Vector3D normal_A = Point3D::point_to_vector(normal_vectors[0]).normalize(), normal_B =
            Point3D::point_to_vector(normal_vectors[1]).normalize(), normal_C = Point3D::point_to_vector(
                normal_vectors[2]).
            normalize();
    Vector3D vn = (normal_A * weightA + normal_B * weightB + normal_C * weightC).normalize();
    Vector3D cam{camera.getPosition().getX(), camera.getPosition().getY(), camera.getPosition().getZ()};
    Vector3D ray = (cam - Vector3D{P.getX(), P.getY(), P.getZ()}).normalize();
    ray = ray.normalize();
    float l = -(ray * vn);
    if (l < 0.0f) {
        l = 0.0f;
    }
    return l;
}

QColor RenderEngine::do_work(std::vector<Point2D> &texture_vectors, QImage &image, const float &weightA,
                             const float &weightB, const float &weightC)
{
    // std::cout << image << std::endl;

    float u = weightA * texture_vectors[0].getX() + weightB * texture_vectors[1].getX() + weightC * texture_vectors[2].
              getX();
    float v = weightA * texture_vectors[0].getY() + weightB * texture_vectors[1].getY() + weightC * texture_vectors[2].
              getY();
    int texX = static_cast<int>((image.width() - 1) - u * (image.width() - 1));
    int texY = static_cast<int>((image.height() - 1) - v * (image.height() - 1));
    // std::cout << "texX: " << texX << " texY: " << texY << std::endl;
    texX = std::clamp(texX, 0, image.width() - 1);
    texY = std::clamp(texY, 0, image.height() - 1);

    return image.pixel(texX, texY);
}

void RenderEngine::universal_render(QPainter &painter, std::vector<Point3D> &result_points,
                                    std::vector<Point3D> &normal_vectors,
                                    std::vector<Point2D> &texture_vectors, DepthBuffer &depth_buffer,std::string &filename ,QColor &fill_color,
                                    Camera &camera)
{
    QImage image((filename.data()));
    Point3D A = result_points[0], B = result_points[1], C = result_points[2], P;

    int x_left, x_right, y_down, y_up;
    initialize_loop_varibles(depth_buffer, A, B, C, x_left, x_right, y_down, y_up);

    for (int y = y_down; y < y_up; y++) {
        for (int x = x_left; x < x_right; x++) {
            if (x < 0 || x > depth_buffer.getWidth() || y > depth_buffer.getHeight() || y < 0) {break;}
            P.set(x, y, 0);
            const float ABP = edgeFunction(A, B, P), BCP = edgeFunction(B, C, P), CAP = edgeFunction(C, A, P);
            if (ABP >= 0 && BCP >= 0 && CAP >= 0) {
                const float ABC = edgeFunction(A, B, C);
                const float weightA = BCP / ABC, weightB = CAP / ABC, weightC = ABP / ABC;

                int z = A.getZ() * weightA + B.getZ() * weightB + C.getZ() * weightC;

                if (depth_buffer.get(x, y) > z) {
                    int r = fill_color.red(), g = fill_color.green(), b = fill_color.blue();
                    if (!texture_vectors.empty()) {
                        QColor texColor = do_work(texture_vectors, image, weightA, weightB, weightC);
                        r = texColor.red(), g = texColor.green(), b = texColor.blue();
                    }
                    //TODOПересчет нормалей выключен

                    if (!normal_vectors.empty()) {
                        float k = 0.4;
                        float l = calculate_parametr_of_illumination(normal_vectors, camera, P, weightA, weightB, weightC);
                        r *= (1 - k + k * l), g *= (1 - k + k * l), b *= (1 - k + k * l);
                    }
                    painter.setPen(QColor(r, g, b));
                    depth_buffer.set(x, y, z);
                    painter.drawPoint(P.getX(), P.getY());
                }
            }
        }
    }
}

void RenderEngine::render_triangles(QPainter &painter, const Model &mesh,
                                    const int &width, const int &height,
                                    const Matrix4D &model_view_projection_matrix, int n_triangles,
                                    DepthBuffer &depth_buffer, std::string &filename,Camera &camera)
{
    for (int triangle_ind = 0; triangle_ind < n_triangles; ++triangle_ind) {
        const int n_vertices_in_triangle = static_cast<int>(mesh.triangles[triangle_ind].get_vertex_indices().size());
        std::vector<Point3D> result_points;
        std::vector<Point3D> normal_vectors;
        std::vector<Point2D> texture_vectors;
        std::vector<Point3D> world_vector;
        add_triangles_vertex(mesh, width, height, model_view_projection_matrix, triangle_ind, n_vertices_in_triangle,
                             result_points, world_vector);
        add_texture_vertex(mesh, triangle_ind, n_vertices_in_triangle, texture_vectors);
        add_normal_vertex(mesh, triangle_ind, n_vertices_in_triangle, normal_vectors);
        // Варианты рендеринга
        // render_texture(painter, result_points, depth_buffer, texture_vectors);
        // rasterization(painter, result_points, depth_buffer);
        // show_mesh(painter, result_points, depth_buffer);
        // render_illumination(painter, result_points, depth_buffer, normal_vectors, camera);
        QColor clr(50,10,0);
        universal_render(painter, result_points, normal_vectors, texture_vectors, depth_buffer, filename,clr, camera);
    }
    painter.end();
}
