#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>
#include "icosphere.h"

using namespace std;
/*
    To improve this model:
      - Higher density mesh in pole regions
      - Change between scale representation, colorings
      -
*/


#define PI 3.14159265f

int N = 3;
float K = 1.0f;


int main( int argc, char* args[] ) {

  int viewmode = 0;
  float logscale = 1.0f;
  bool scalesphere = true;



  float SK = 6.0f;
  float SM = 4.0f;
  float GM = 2.0f;

  Tiny::window("Circumcenter Iteration Map Stereographic Projection", 800, 800);	//Open Window

  cam::near = -100.0f;
  cam::far = 100.0f;
  cam::zoomrate *= 10.0f;
  cam::init(200.0f, cam::ORTHO);

  Shader sphere({"shader/sphere.vs", "shader/sphere.fs"}, {"in_Position"});

  Icosphere icosahedron;

  /*

  isobuild(positions, indices);
  for(int i = 0; i < 5; i++)
    isosplit(positions, indices);

  if(viewmode == 2)
  for(auto& p: positions){
     p = normalize(p);
     p /= getscale(stereographic(p, K));
  }

  */

  glDisable(GL_CULL_FACE);

  glm::vec4 converge = glm::vec4(0,1,0,1);
  glm::vec4 diverge = glm::vec4(1,0,0,1);
  glm::mat4 invvp = glm::inverse(cam::vp);


  Tiny::event.handler  = [&](){

    if(cam::handler()){
      invvp = glm::inverse(cam::vp);
    }

  };

  Tiny::view.interface = [&](){
    ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once);
    ImGui::Begin("Circumcenter Iteration Map Stereographic Projection", NULL, ImGuiWindowFlags_NoResize);
    if(ImGui::DragFloat("Scale", &K, 0.01f, 0.0f, 20.0f)){

        /*
      if(viewmode == 2){
        for(auto& p: positions){
          p = normalize(p);
          p /= getscale(stereographic(p, K));
        }
        posbuf.fill(positions);
      }
      else for(auto& p: positions)
        p = normalize(p);
        */

    }
    if(ImGui::SliderInt("N", &N, 3, 8)){

      /*
      if(viewmode == 2){
        for(auto& p: positions){
          p = normalize(p);
          p /= getscale(stereographic(p, K));
        }
        posbuf.fill(positions);
      }
      else for(auto& p: positions)
        p = normalize(p);

        */
    }
    ImGui::ColorEdit4("Converge Color", &converge[0]);
    ImGui::ColorEdit4("Diverge Color", &diverge[0]);
    ImGui::RadioButton("Color Scale", &viewmode, 0); ImGui::SameLine();
    ImGui::RadioButton("Color Rotation", &viewmode, 1); ImGui::SameLine();
    ImGui::RadioButton("Color LogScale", &viewmode, 2);// ImGui::SameLine();
    ImGui::Checkbox("Scale Sphere", &scalesphere);
    ImGui::DragFloat("Log-Scale", &logscale, 0.01f, 0.0f, 20.0f);
    ImGui::DragFloat("Scale K", &SK, 0.01f, 0.0f, 20.0f);
    ImGui::DragFloat("Scale GMax", &GM, 0.01f, 0.0f, 20.0f);
    ImGui::DragFloat("Scale Max", &SM, 0.01f, 0.0f, 20.0f);
    ImGui::End();
  };	//Set Interface Function


  Tiny::view.pipeline = [&](){

    Tiny::view.target(glm::vec3(1));

    sphere.use();
    sphere.uniform("vp", cam::vp);	//View Projection Matrix
    sphere.uniform("invvp", invvp);	//View Projection Matrix
    sphere.uniform("scale", K);
    sphere.uniform("converge", converge);
    sphere.uniform("diverge", diverge);
    sphere.uniform("viewmode", viewmode);
    sphere.uniform("scalesphere", scalesphere);
    sphere.uniform("SK", SK);
    sphere.uniform("SM", SM);
    sphere.uniform("GM", GM);
    sphere.uniform("N", N);
    icosahedron.render(GL_TRIANGLES);							  //Render Model with Lines

  };

  Tiny::loop([&](){											//Extra Stuff (every tick)

  });

  Tiny::quit();
}
