#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>

using namespace std;
#include "model.h"

int main( int argc, char* args[] ) {

  Tiny::window("Circumcenter Iteration Map Stereographic Projection", 800, 800);	//Open Window

  cam::near = -100.0f;
  cam::far = 100.0f;
  cam::init(61.0f, cam::ORTHO);

  Shader sphere({"shader/sphere.vs", "shader/sphere.fs"}, {"in_Position"});

  Model icosahedron({"in_Position"});

  std::vector<glm::vec3> positions;
  std::vector<glm::uvec3> indices;

  isobuild(positions, indices);
  for(int i = 0; i < 5; i++)
    isosplit(positions, indices);

  std::sort(indices.begin(), indices.end(), [&](const glm::uvec3& a, const glm::uvec3& b){
    glm::vec3 ap = cam::vp*glm::vec4((positions[a.x] + positions[a.y] + positions[a.z])/3.0f, 1.0);
    glm::vec3 bp = cam::vp*glm::vec4((positions[b.x] + positions[b.y] + positions[b.z])/3.0f, 1.0);
    return ap.z > bp.z;
  });

  icosahedron.bind<glm::vec3>("in_Position", new Buffer(positions), true);
  icosahedron.index(new Buffer(indices), true);
  icosahedron.SIZE = indices.size()*3;

  glDisable(GL_CULL_FACE);

  glm::vec4 converge = glm::vec4(0,1,0,0.75);
  glm::vec4 diverge = glm::vec4(1,0,0,0);

  int N = 3;
  float scale = 1.0f;
  int viewmode = 0;

  Tiny::event.handler  = [&](){

    if(cam::handler())
    std::sort(indices.begin(), indices.end(), [&](const glm::uvec3& a, const glm::uvec3& b){
      glm::vec3 ap = cam::vp*glm::vec4((positions[a.x] + positions[a.y] + positions[a.z])/3.0f, 1.0);
      glm::vec3 bp = cam::vp*glm::vec4((positions[b.x] + positions[b.y] + positions[b.z])/3.0f, 1.0);
      return ap.z > bp.z;
    });
    icosahedron.buffers["index"]->fill(indices);

  };

  Tiny::view.interface = [&](){
    ImGui::SetNextWindowSize(ImVec2(500, 260), ImGuiCond_Once);
    ImGui::Begin("Circumcenter Iteration Map Stereographic Projection", NULL, ImGuiWindowFlags_NoResize);
    ImGui::DragFloat("Scale", &scale, 0.01f, 0.0f, 20.0f);
    ImGui::SliderInt("N", &N, 3, 8);
    ImGui::ColorEdit4("Converge Color", &converge[0]);
    ImGui::ColorEdit4("Diverge Color", &diverge[0]);
    ImGui::RadioButton("Color Scale", &viewmode, 0); ImGui::SameLine();
    ImGui::RadioButton("Color Rotation", &viewmode, 1); ImGui::SameLine();
    ImGui::RadioButton("Color LogScale", &viewmode, 2);
    ImGui::End();
  };	//Set Interface Function

  Tiny::view.pipeline = [&](){

    Tiny::view.target(glm::vec3(0));

    sphere.use();
    sphere.uniform("vp", cam::vp);	//View Projection Matrix
    sphere.uniform("scale", scale);
    sphere.uniform("converge", converge);
    sphere.uniform("diverge", diverge);
    sphere.uniform("viewmode", viewmode);
    sphere.uniform("N", N);
    icosahedron.render(GL_TRIANGLES);							  //Render Model with Lines

  };

  Tiny::loop([&](){											//Extra Stuff (every tick)

  });

  Tiny::quit();
}
