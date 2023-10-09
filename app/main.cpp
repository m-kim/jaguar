#include <iostream>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp> // all types inspired from GLSL
#include <glm/ext.hpp>

#include <webgpu.h>
#include <vector>
#include <windows.h>
#include <filesystem>
#include <fstream>
#include "ApplicationWin.h"


std::vector<float> vertex_data ={
-0.5, -0.5, -0.3,
+0.5, -0.5, -0.3,
+0.5, +0.5, -0.3,
-0.5, +0.5, -0.3,

-0.5, -0.5, -0.3,
+0.5, -0.5, -0.3,
+0.0, +0.0, +0.5,

+0.5, -0.5, -0.3,
+0.5, +0.5, -0.3,
+0.0, +0.0, +0.5,

+0.5, +0.5, -0.3,
-0.5, +0.5, -0.3,
+0.0, +0.0, +0.5,

-0.5, +0.5, -0.3,
-0.5, -0.5, -0.3,
+0.0, +0.0, +0.5,
};
std::vector<float> normal_data = {
0.0, -1.0, 0.0,
0.0, -1.0, 0.0,
0.0, -1.0, 0.0,
0.0, -1.0, 0.0,
0.0, -0.848, 0.53,
0.0, -0.848, 0.53,
0.0, -0.848, 0.53,
0.848, 0.0, 0.53,
0.848, 0.0, 0.53,
0.848, 0.0, 0.53,
0.0, 0.848, 0.53,
0.0, 0.848, 0.53,
0.0, 0.848, 0.53,
-0.848, 0.0, 0.53,
-0.848, 0.0, 0.53,
-0.848, 0.0, 0.53,
};
std::vector<int> index_data = {
 0,  1,  2,
 0,  2,  3,
 4,  5,  6,
 7,  8,  9,
10, 11, 12,
13, 14, 15,
};
std::vector<float> color_data = {
1.0, 1.0, 1.0,
1.0, 1.0, 1.0,
1.0, 1.0, 1.0,
1.0, 1.0, 1.0,
1.0, 1.0, 1.0,
1.0, 1.0, 1.0,
1.0, 1.0, 1.0,
1.0, 1.0, 1.0,
1.0, 1.0, 1.0,
1.0, 1.0, 1.0,
1.0, 1.0, 1.0,
1.0, 1.0, 1.0,
1.0, 1.0, 1.0,
};


int main (int, char**) 
{
    ApplicationWin app;
    if (!app.onInit())
        return -1;
    app.loadMesh(vertex_data, normal_data, color_data, index_data);

    while (app.render()) {
    }

    app.onFinish();
    std::cout << "Finished" << std::endl;
    return 0;
}