#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/MakeMesh.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <string>
#include <unordered_set>
#include <ctime>
using namespace DRAWER;

template <typename Scalar, typename Index>
bool readOBJ(
    const std::string obj_file_name,
    std::vector<std::vector<Scalar > >& V,
    std::vector<std::vector<Scalar > >& TC,
    std::vector<std::vector<Scalar > >& N,
    std::vector<std::vector<Index > >& F,
    std::vector<std::vector<Index > >& FTC,
    std::vector<std::vector<Index > >& FN)
{
    // Open file, and check for error
    FILE* obj_file = fopen(obj_file_name.c_str(), "r");
    if (NULL == obj_file)
    {
        fprintf(stderr, "IOError: %s could not be opened...\n",
            obj_file_name.c_str());
        return false;
    }
    std::vector<std::tuple<std::string, Index, Index >> FM;
    return readOBJ(obj_file, V, TC, N, F, FTC, FN, FM);
}

template <typename Scalar, typename Index>
bool readOBJ(
    FILE* obj_file,
    std::vector<std::vector<Scalar > >& V,
    std::vector<std::vector<Scalar > >& TC,
    std::vector<std::vector<Scalar > >& N,
    std::vector<std::vector<Index > >& F,
    std::vector<std::vector<Index > >& FTC,
    std::vector<std::vector<Index > >& FN,
    std::vector<std::tuple<std::string, Index, Index >>& FM)
{
    // File open was successful so clear outputs
    V.clear();
    TC.clear();
    N.clear();
    F.clear();
    FTC.clear();
    FN.clear();

    // variables and constants to assist parsing the .obj file
    // Constant strings to compare against
    std::string v("v");
    std::string vn("vn");
    std::string vt("vt");
    std::string f("f");
    std::string tic_tac_toe("#");

#  define IGL_LINE_MAX 2048
#  define MATERIAL_LINE_MAX 2048

    char line[IGL_LINE_MAX];
    char currentmaterialref[MATERIAL_LINE_MAX] = "";
    bool FMwasinit = false;
    int line_no = 1, previous_face_no = 0, current_face_no = 0;
    while (fgets(line, IGL_LINE_MAX, obj_file) != NULL)
    {
        char type[IGL_LINE_MAX];
        // Read first word containing type
        if (sscanf(line, "%s", type) == 1)
        {
            // Get pointer to rest of line right after type
            char* l = &line[strlen(type)];
            if (type == v)
            {
                std::istringstream ls(&line[1]);
                std::vector<Scalar > vertex{ std::istream_iterator<Scalar >(ls), std::istream_iterator<Scalar >() };

                if (vertex.size() < 3)
                {
                    fprintf(stderr,
                        "Error: readOBJ() vertex on line %d should have at least 3 coordinates",
                        line_no);
                    fclose(obj_file);
                    return false;
                }

                V.push_back(vertex);
            }
            else if (type == vn)
            {
                double x[3];
                int count =
                    sscanf(l, "%lf %lf %lf\n", &x[0], &x[1], &x[2]);
                if (count != 3)
                {
                    fprintf(stderr,
                        "Error: readOBJ() normal on line %d should have 3 coordinates",
                        line_no);
                    fclose(obj_file);
                    return false;
                }
                std::vector<Scalar > normal(count);
                for (int i = 0; i < count; i++)
                {
                    normal[i] = x[i];
                }
                N.push_back(normal);
            }
            else if (type == vt)
            {
                double x[3];
                int count =
                    sscanf(l, "%lf %lf %lf\n", &x[0], &x[1], &x[2]);
                if (count != 2 && count != 3)
                {
                    fprintf(stderr,
                        "Error: readOBJ() texture coords on line %d should have 2 "
                        "or 3 coordinates (%d)",
                        line_no, count);
                    fclose(obj_file);
                    return false;
                }
                std::vector<Scalar > tex(count);
                for (int i = 0; i < count; i++)
                {
                    tex[i] = x[i];
                }
                TC.push_back(tex);
            }
            else if (type == f)
            {
                const auto& shift = [&V](const int i)->int
                {
                    return i < 0 ? i + V.size() : i - 1;
                };
                const auto& shift_t = [&TC](const int i)->int
                {
                    return i < 0 ? i + TC.size() : i - 1;
                };
                const auto& shift_n = [&N](const int i)->int
                {
                    return i < 0 ? i + N.size() : i - 1;
                };
                std::vector<Index > f;
                std::vector<Index > ftc;
                std::vector<Index > fn;
                // Read each "word" after type
                char word[IGL_LINE_MAX];
                int offset;
                while (sscanf(l, "%s%n", word, &offset) == 1)
                {
                    // adjust offset
                    l += offset;
                    // Process word
                    long int i, it, in;
                    if (sscanf(word, "%ld/%ld/%ld", &i, &it, &in) == 3)
                    {
                        f.push_back(shift(i));
                        ftc.push_back(shift_t(it));
                        fn.push_back(shift_n(in));
                    }
                    else if (sscanf(word, "%ld/%ld", &i, &it) == 2)
                    {
                        f.push_back(shift(i));
                        ftc.push_back(shift_t(it));
                    }
                    else if (sscanf(word, "%ld//%ld", &i, &in) == 2)
                    {
                        f.push_back(shift(i));
                        fn.push_back(shift_n(in));
                    }
                    else if (sscanf(word, "%ld", &i) == 1)
                    {
                        f.push_back(shift(i));
                    }
                    else
                    {
                        fprintf(stderr,
                            "Error: readOBJ() face on line %d has invalid element format\n",
                            line_no);
                        fclose(obj_file);
                        return false;
                    }
                }
                if (
                    (f.size() > 0 && fn.size() == 0 && ftc.size() == 0) ||
                    (f.size() > 0 && fn.size() == f.size() && ftc.size() == 0) ||
                    (f.size() > 0 && fn.size() == 0 && ftc.size() == f.size()) ||
                    (f.size() > 0 && fn.size() == f.size() && ftc.size() == f.size()))
                {
                    // No matter what add each type to lists so that lists are the
                    // correct lengths
                    F.push_back(f);
                    FTC.push_back(ftc);
                    FN.push_back(fn);
                    current_face_no++;
                }
                else
                {
                    fprintf(stderr,
                        "Error: readOBJ() face on line %d has invalid format\n", line_no);
                    fclose(obj_file);
                    return false;
                }
            }
            else if (strlen(type) >= 1 && strcmp("usemtl", type) == 0)
            {
                if (FMwasinit) {
                    FM.push_back(std::make_tuple(currentmaterialref, previous_face_no, current_face_no - 1));
                    previous_face_no = current_face_no;
                }
                else {
                    FMwasinit = true;
                }
                sscanf(l, "%s\n", currentmaterialref);
            }
            else if (strlen(type) >= 1 && (type[0] == '#' ||
                type[0] == 'g' ||
                type[0] == 's' ||
                strcmp("mtllib", type) == 0))
            {
                //ignore comments or other shit
            }
            else
            {
                //ignore any other lines
                fprintf(stderr,
                    "Warning: readOBJ() ignored non-comment line %d:\n  %s",
                    line_no,
                    line);
            }
        }
        else
        {
            // ignore empty line
        }
        line_no++;
    }
    if (strcmp(currentmaterialref, "") != 0)
        FM.push_back(std::make_tuple(currentmaterialref, previous_face_no, current_face_no - 1));
    fclose(obj_file);

    assert(F.size() == FN.size());
    assert(F.size() == FTC.size());

    return true;
}
// Just V and F
template <typename Scalar, typename Index>
bool readOBJ(
    const std::string obj_file_name,
    std::vector<std::vector<Scalar > >& V,
    std::vector<std::vector<Index > >& F)
{
    std::vector<std::vector<Scalar > > TC, N;
    std::vector<std::vector<Index > > FTC, FN;
    std::vector<std::tuple<std::string, Index, Index >> FM;

    return readOBJ(obj_file_name, V, TC, N, F, FTC, FN);
}

void buildE(const Eigen::Matrix<GLfloat, -1, -1>& V, const Eigen::Matrix<int, -1, -1>& F, Eigen::Matrix<int, -1, -1>& E)
{
    std::vector<std::tuple<int, int, int, int>> temp;
    temp.reserve(F.rows() * 3);
    for (int i = 0; i < F.rows(); ++i) {
        int vn = F.row(i).size();
        for (int j = 0; j < vn; ++j) {
            int v0 = F(i,j), v1 = F(i, (j + 1) % vn);
            if (v0 > v1)
                std::swap(v0, v1);
            temp.push_back(std::make_tuple(v0, v1, i, j));
        }
    }
    std::sort(temp.begin(), temp.end());
    std::vector<std::vector<int>> _E;
    int E_num = 0;
    std::vector<int> e(2);
    for (int i = 0; i < temp.size(); ++i) {
        if (i == 0 || (i != 0 && (std::get<0>(temp[i]) != std::get<0>(temp[i - 1]) ||
            std::get<1>(temp[i]) != std::get<1>(temp[i - 1])))) {
            E_num++;
            e[0] = std::get<0>(temp[i]);
            e[1] = std::get<1>(temp[i]);
            _E.push_back(e);
        }
    }
    E.resize(_E.size(), 2);
    for (int i = 0; i < _E.size(); i++)
        for (int j = 0; j < _E[i].size(); j++)
            E(i, j) = _E[i][j];
}

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);
        std::string path = "D:/tencent/lod/dataset/GroupMeshes1/original1.obj";
        std::vector<std::vector<double>> V;
        std::vector<std::vector<int> > F;
  readOBJ(path, V, F);
  Eigen::Matrix<GLfloat, -1, -1> DV(V.size(), V[0].size());
  Eigen::Matrix<int, -1, -1> DF(F.size(), F[0].size());
  for (int i = 0; i < V.size(); i++)
      for (int j = 0; j < V[i].size(); j++)
          DV(i, j) = V[i][j];
  for (int i = 0; i < F.size(); i++)
      for (int j = 0; j < F[i].size(); j++)
          DF(i, j) = F[i][j];

  time_t time_start = clock();
  auto shape = DRAWER::makeTriMesh(true, DV, DF);
  drawer.addShape(shape);
  shape->setEnabled(true);

  Eigen::Matrix<int, -1, -1> DE;
  buildE(DV, DF, DE);
  auto shape1 = DRAWER::makeWires(DV, DE);
  shape1->setColor(GL_LINES, 1, 0, 0);
  shape1->setEnabled(true);
  drawer.addShape(shape1);
  shape1->setUseLight(false);
  shape1->setLineWidth(2);

  double time_elapsed = double(clock() - time_start) / CLOCKS_PER_SEC;
    std::cout << "\nFinished in " << time_elapsed << " secs";
  drawer.mainLoop();
  return 0;
}
