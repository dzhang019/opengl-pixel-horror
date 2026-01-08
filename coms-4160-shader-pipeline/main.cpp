// clang++ -std=c++11 main.cpp -I include -I $LIBIGL/include -I /usr/local/libigl/external/eigen/ -framework OpenGL -L/usr/local/lib/ -lglfw && ./a.out
//
#define _USE_MATH_DEFINES
#include <cmath>
// make sure the modern opengl headers are included before any others
#include "gl.h"
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "read_json.h"
#include "icosahedron.h"
#include "mesh_to_vao.h"
#include "print_opengl_info.h"
#include "get_seconds.h"
#include "report_gl_error.h"
#include "create_shader_program_from_files.h"
#include "last_modification_time.h"
#include "camera.h"
#ifdef USE_SOLUTION
#  include "find_and_replace_all.h"
#endif

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <thread>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <vector>

#include "object.h"

bool wire_frame = false;
bool mouse_down = false;
bool is_animating = true;
double last_time = get_seconds();
double animation_seconds = 0;
int width =  1280;
int height = 720;
int highdpi = 1;
GLuint prog_id=0;

Eigen::Vector3f init_pos = Eigen::Vector3f(0.0f,0.0f,10.0f); 
Camera camera(init_pos);
float speed = 1.0f;
float fog_r = 0.7f, fog_g = 0.7f, fog_b = 0.9f;

Eigen::Matrix4f proj = Eigen::Matrix4f::Identity();
GLuint VAO;
std::vector<Object*> scene;

Eigen::Matrix< float,Eigen::Dynamic,3,Eigen::RowMajor> V;
Eigen::Matrix<GLuint,Eigen::Dynamic,3,Eigen::RowMajor> F;

int main(int argc, char * argv[])
{

  std::vector<std::string> vertex_shader_paths;
  std::vector<std::string> tess_control_shader_paths;
  std::vector<std::string> tess_evaluation_shader_paths;
  std::vector<std::string> fragment_shader_paths;

  // Initialize glfw window
  if(!glfwInit())
  {
    std::cerr<<"Could not initialize glfw"<<std::endl;
     return EXIT_FAILURE;
  }
  const auto & error = [] (int error, const char* description)
  {
    std::cerr<<description<<std::endl;
  };
  glfwSetErrorCallback(error);
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  GLFWwindow* window = glfwCreateWindow(width, height, "shader-pipeline", NULL, NULL);
  if(!window)
  {
    glfwTerminate();
    std::cerr<<"Could not create glfw window"<<std::endl;
    return EXIT_FAILURE;
  }
  std::cout<<R"(
Usage:
  [Click and drag]  to orbit view
  [Scroll]  to translate view in and out
  A,a  toggle animation
  L,l  toggle wireframe rending
  Z,z  reset view to look along z-axis
)";
  glfwSetWindowPos(window,0,0);
  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
  {
    std::cerr<<"Failed to load OpenGL and its extensions"<<std::endl;
    return EXIT_FAILURE;
  }
  print_opengl_info(window);
  igl::opengl::report_gl_error("init");


  Object* bob = new Object();
  bob->load_from_file("../data/MilfordLaneStatue1_01.obj");
  bob->model_matrix.setIdentity();
  bob->ka = Eigen::Vector3f(0.1, 0.1, 0.0);
  bob->kd = Eigen::Vector3f(0.9, 0.6, 0.6); 
  bob->ks = Eigen::Vector3f(1.0, 1.0, 1.0);
  bob->p  = 128.0;
  bob->id = 1;
  bob->rotate(-180.0f, 1.0f, 0.0f, 0.0f);
  bob->rotate(50.0f, 0.0f, 1.0f, 0.0f);
  bob->scale(10.0f);

  bob->translate(180.0f, -10.0f, -120.0f);
  scene.push_back(bob);
  

  Object* ground = new Object();
  ground->create_procedural_plane();
  ground->model_matrix.setIdentity();
  ground->ka = Eigen::Vector3f(0.0, 0.1, 0.0);
  ground->kd = Eigen::Vector3f(0.2, 0.4, 0.2); 
  ground->ks = Eigen::Vector3f(0.0, 0.0, 0.0); 
  ground->p  = 10.0;
  ground->id = 0;
  ground->translate(40.0f, -4.0f, 50.0f);
  scene.push_back(ground);

  // int num_trees = 5; 
  // float range = 5.0f;
  // for(int i = 0; i < num_trees; ++i) 
  // {
  //     Object* tree = new Object();

  //     tree->load_from_file("../data/MapleTree.obj");
      
  //     tree->model_matrix.setIdentity();
      
  //     tree->ka = Eigen::Vector3f(0.0, 0.1, 0.0);
  //     tree->kd = Eigen::Vector3f(0.15, 0.6, 0.3);
  //     tree->ks = Eigen::Vector3f(0.0, 0.0, 0.0);
  //     tree->p  = 10.0;
      
  //     tree->id = 2; 

  //     float rx = ((float)rand() / RAND_MAX) * range - (range / 2.0f);
  //     float rz = ((float)rand() / RAND_MAX) * range - (range / 2.0f);

  //     tree->scale(0.01f); // Make it small
      
  //     // float random_rot = ((float)rand() / RAND_MAX) * 360.0f;
  //     // tree->rotate(random_rot, 0, 1, 0);

  //     tree->translate(rx, 0.0f, rz);

  //     scene.push_back(tree);
  // }
  // Object* bark = new Object();
  // bark->load_from_file("../data/MapleTree.obj");
  // bark->model_matrix.setIdentity();
  // bark->ka = Eigen::Vector3f(0.0, 0.1, 0.0);
  // bark->kd = Eigen::Vector3f(0.15, 0.4, 0.0); 
  // bark->ks = Eigen::Vector3f(0.0, 0.0, 0.0);
  // bark->p  = 10.0;
  // bark->id = 1;
  // bark->scale(0.01f);
  // // ground->translate(40.0f, -4.0f, 50.0f);
  // scene.push_back(bark);

  

  // icosahedron(V,F);
  // mesh_to_vao(V,F,VAO);

  igl::opengl::report_gl_error("mesh_to_vao");
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  const auto & reshape = [](
    GLFWwindow* window,
    int _width,
    int _height)
  {
    ::width=_width,::height=_height;

    // augh, windows can't handle variables named near and far.
    float nearVal = 0.01;
    float farVal = 500;
    float top = tan(35./360.*M_PI)*nearVal;
    float right = top * (double)::width/(double)::height;
    float left = -right;
    float bottom = -top;
    proj.setConstant(4,4,0.);
    proj(0,0) = (2.0 * nearVal) / (right - left);
    proj(1,1) = (2.0 * nearVal) / (top - bottom);
    proj(0,2) = (right + left) / (right - left);
    proj(1,2) = (top + bottom) / (top - bottom);
    proj(2,2) = -(farVal + nearVal) / (farVal - nearVal);
    proj(3,2) = -1.0;
    proj(2,3) = -(2.0 * farVal * nearVal) / (farVal - nearVal);
  };

  glfwSetWindowSizeCallback(window,reshape);
  {
    int width_window, height_window;
    glfwGetWindowSize(window, &width_window, &height_window);
    reshape(window,width_window,height_window);
  }

  glfwSetKeyCallback(
    window,
    [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
      if(key == 256 || (key == 67 && (mods & GLFW_MOD_CONTROL)))
      {
        glfwSetWindowShouldClose(window,true);
      }
    });
  glfwSetCharModsCallback(
    window,
    [](GLFWwindow* window, unsigned int codepoint, int modifier)
    {
      switch (codepoint)
      {
        case 'A':
        case 'a':
          is_animating = !is_animating;
          if (is_animating)
          {
            last_time = get_seconds();
          }
          break;

        case 'L':
        case 'l':
          wire_frame = !wire_frame;
          if (wire_frame) {
            glDisable(GL_CULL_FACE);
          } else {
            //glEnable(GL_CULL_FACE);
            glDisable(GL_CULL_FACE);
          }
          break;

        default:
          //std::cout << "Unrecognized key: " << (unsigned char)codepoint << std::endl;
          break;
      }
  });
  glfwSetMouseButtonCallback(
    window,
    [](GLFWwindow * window, int button, int action, int mods)
    {
      mouse_down = action == GLFW_PRESS;
    });
  glfwSetCursorPosCallback(
    window,
    [](GLFWwindow * window, double x, double y)
    {
      static double mouse_last_x = x;
      static double mouse_last_y = y;
      double dx = x - mouse_last_x;
      double dy = y - mouse_last_y;
      mouse_last_x = x;
      mouse_last_y = y;

      // if (!mouse_down) return;

      const float sensitivity = 0.002f; // tweak
      camera.rotateYaw(  -float(dx) * sensitivity);
      camera.rotatePitch(-float(dy) * sensitivity);
  });
  // glfwSetScrollCallback(window,
  //   [](GLFWwindow * window, double xoffset, double yoffset)
  //   {
  //     view.matrix()(2,3) =
  //       std::min(std::max(view.matrix()(2,3)+(float)yoffset,-100.0f),-2.0f);
  //   });

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  // Force compilation on first iteration through loop
  double time_of_last_shader_compilation = 0;
  double time_of_last_json_load = 0;
  const auto any_changed = 
    [](
        const std::vector<std::string> &paths,
        const double time_of_last_shader_compilation
        )->bool
  {
    for(const auto & path : paths)
    {
      if(last_modification_time(path) > time_of_last_shader_compilation)
      {
        std::cout<<path<<" has changed since last compilation attempt."<<std::endl;
        return true;
      }
    }
    return false;
  };

  float start_time = get_seconds();
  // Main display routine
  double last_frame_time = get_seconds();
  while (!glfwWindowShouldClose(window))
  {
    double now = get_seconds();
    float dt = float(now - last_frame_time);
    last_frame_time = now;

    double tic = now;
    //wasd camera
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.moveForward(speed * dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.moveForward(-speed * dt);
      if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.moveRight(speed * dt);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.moveRight(-speed * dt);
    


    if(any_changed({argv[1]},time_of_last_json_load))
    {
      std::cout<<"-----------------------------------------------"<<std::endl;
      time_of_last_json_load = get_seconds();
      if(!read_json(argv[1],
            vertex_shader_paths,
            tess_control_shader_paths,
            tess_evaluation_shader_paths,
            fragment_shader_paths))
      {
        std::cerr<<"Failed to read "<<argv[1]<<std::endl;
      }
#ifdef USE_SOLUTION
      {
        const auto replace_all = [](std::vector<std::string> & paths)
        {
          for(auto & path : paths)
          {
            find_and_replace_all("/src/","/solution/",path);
          }
        };
        replace_all(vertex_shader_paths);
        replace_all(tess_control_shader_paths);
        replace_all(tess_evaluation_shader_paths);
        replace_all(fragment_shader_paths);
      }
#endif
      // force reload of shaders
      time_of_last_shader_compilation = 0;
    }
    if(
      any_changed(vertex_shader_paths         ,time_of_last_shader_compilation) ||
      any_changed(tess_control_shader_paths   ,time_of_last_shader_compilation) ||
      any_changed(tess_evaluation_shader_paths,time_of_last_shader_compilation) ||
      any_changed(fragment_shader_paths       ,time_of_last_shader_compilation))
    {
      std::cout<<"-----------------------------------------------"<<std::endl;
      // remember the time we tried to compile
      time_of_last_shader_compilation = get_seconds();
      if(
          !create_shader_program_from_files(
            vertex_shader_paths,
            tess_control_shader_paths,
            tess_evaluation_shader_paths,
            fragment_shader_paths,
            prog_id))
      {
        // Force null shader to visually indicate failure
        glDeleteProgram(prog_id);
        prog_id = 0;
        std::cout<<"-----------------------------------------------"<<std::endl;
      }
    }

    // clear screen and set viewport
    glClearColor(fog_r, fog_g, fog_b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwGetFramebufferSize(window, &::width, &::height);
    glViewport(0,0,::width,::height);
    // select program 
    glUseProgram(prog_id);

    //add lighting
    GLint lp_loc = glGetUniformLocation(prog_id, "ambient_light");
    glUniform3f(glGetUniformLocation(prog_id, "light_position"), 0.0f, 100.0f, 0.0f);

    // light
    glUniform3f(glGetUniformLocation(prog_id, "light_color"), 0.5f, 0.5f, 1.0f);

    // ambient light
    glUniform3f(glGetUniformLocation(prog_id, "ambient_light"), 0.05f, 0.05f, 0.05f);
    glUniform1i(glGetUniformLocation(prog_id, "use_texture"), 0);

    GLfloat debug_values[3];
    glGetUniformfv(prog_id,lp_loc, debug_values);
    std::cout << "GPU received LP: " << debug_values[0] << debug_values[1] << debug_values[2] << std::endl;
    glPatchParameteri(GL_PATCH_VERTICES, 3);
    // Attach uniforms
    {
      if(is_animating)
      {
        double now = get_seconds();
        animation_seconds += now - last_time;
        last_time = now;
      }
      glUniform1f(glGetUniformLocation(prog_id,"animation_seconds"),animation_seconds);
    }
    glUniformMatrix4fv(
      glGetUniformLocation(prog_id,"proj"),1,false,proj.data());
    //update camera view
    Eigen::Matrix4f view = camera.viewMatrix();
    glUniformMatrix4fv(
      glGetUniformLocation(prog_id,"view"), 1, GL_FALSE, view.data());
    // Draw mesh as wireframe
    if(wire_frame)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }else
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    //render objects
    // for(int i = 0;i<2;i++)
    // {
    //   glUniform1i(glGetUniformLocation(prog_id, "is_moon"), i==1);
    //   glBindVertexArray(VAO);
    //   glDrawElements(GL_PATCHES, F.size(), GL_UNSIGNED_INT, 0);
    //   glBindVertexArray(0);
    // }
    for (Object* obj : scene)
    {
        obj->draw(prog_id);
    }


    glfwSwapBuffers(window);

    // 60 fps
    {
      glfwPollEvents();
      // In microseconds
      double duration = 1000000.*(get_seconds()-tic);
      const double min_duration = 1000000./60.;
      if(duration<min_duration)
      {
        std::this_thread::sleep_for(std::chrono::microseconds((int)(min_duration-duration)));
      }
    }
  }

  // Graceful exit
  glfwDestroyWindow(window);
  glfwTerminate();
  for (Object* obj : scene) {
      delete obj;
  }
  scene.clear();
  return EXIT_SUCCESS;
}
