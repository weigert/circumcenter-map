#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>

#define PI 3.14159265f

#include "model.h"

int main( int argc, char* args[] ) {

  Tiny::view.pointSize = 8.0f;
  Tiny::window("Pedal Curve Operator", 800, 800);	//Open Window

  //Upload a Model which contains the Lines!
  Shader lines({"shader/lines.vs", "shader/lines.fs"}, {"in_Position"});

  const int N = 3;
  std::vector<glm::vec2> points;
  for(int i = 0; i <= N; i++){
    points.push_back(glm::vec2(cos(2.0f*PI*(float)i/(float)N), sin(2.0f*PI*(float)i/(float)N)));
  }

  Model linemodel({"in_Position"});
  Buffer pointsbuf(points);
  linemodel.bind<glm::vec2>("in_Position", &pointsbuf);
  linemodel.SIZE = pointsbuf.SIZE;

  float zoom = 0.05f;
  glm::vec2 center = glm::vec2(0);
  float WIDTH = 800;
  float HEIGHT = 800;

  float scale = 1.0f;
  bool paused = true;

  glm::vec2 fixpoint = glm::vec2(10, 0);
  if(N == 3) fixpoint = glm::vec2(1-pow(3.0f, 1.0f/2.0f), 0);
  if(N == 5) fixpoint = glm::vec2(-1.0f*pow(4.0f, 1.0f/3.0f), 0);

  Tiny::event.handler  = [&](){

    if(Tiny::event.scroll.posy)
      zoom += 0.01;
    if(Tiny::event.scroll.negy)
      zoom -= 0.01;

    if(!Tiny::event.press.empty() && Tiny::event.press.back() == SDLK_p)
      paused = !paused;

  };

  Tiny::view.interface = [&](){};

  Tiny::view.pipeline = [&](){

    Tiny::view.target(glm::vec3(1));

    lines.use();

    lines.uniform("zoom", zoom);
    lines.uniform("center", center);
    lines.uniform("color", glm::vec3(1,0,0));

    linemodel.render(GL_POINTS);							  //Render Model with Lines
    linemodel.render(GL_LINE_STRIP);							  //Render Model with Lines

  };

  Tiny::loop([&](){											//Extra Stuff (every tick)

    if(paused)
      return;

    std::vector<glm::vec2> temppoints;
    for(int i = 0; i <= N; i++){
      temppoints.push_back(circumcenter(fixpoint, points[i], points[(i+1)%N]));
    }
    points = temppoints;
    pointsbuf.fill(points);

  });

  Tiny::quit();
}
