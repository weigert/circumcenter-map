#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>


std::function<void(Model* m)> _build = [](Model* h){

  //Radius r
  float r = 10.0f;
  float a = 72.0f/360.0f*2.0f*3.14159265f;

  //Helper Methods
  auto addpos = [&](float a, float b, float c){
    h->positions.push_back(a);
    h->positions.push_back(b);
    h->positions.push_back(c);
  };

  //For a Line-Strip
  /*
  auto triangle = [&](int a, int b, int c){
    h->indices.push_back(a);
    h->indices.push_back(b);
    h->indices.push_back(b);
    h->indices.push_back(c);
    h->indices.push_back(c);
    h->indices.push_back(a);
  };
  */

  //For a Triangles
  auto triangle = [&](int a, int b, int c){
    h->indices.push_back(a);
    h->indices.push_back(b);
    h->indices.push_back(c);
  };

  //Top Cap
  addpos(0, r, 0);

  for(int i = 0; i < 5; i++){ //Top
    float x1 = r*cos(atan(0.5))*cos(a*i);
    float y1 = r*cos(atan(0.5))*sin(a*i);
    float z1 = r*sin(atan(0.5));
    addpos(x1,z1,y1);
  }

  for(int i = 0; i < 5; i++){ //Bottom
    float x2 = r*cos(atan(0.5))*cos(a*i+a/2.0f);
    float y2 = r*cos(atan(0.5))*sin(a*i+a/2.0f);
    float z2 = -r*sin(atan(0.5));
    addpos(x2,z2,y2);
  }

  //Bottom Cap
  addpos(0,-r,0);

  for(int i = 0; i < 12; i++){
    glm::vec3 n = glm::vec3(h->positions[3*i+0],h->positions[3*i+1],h->positions[3*i+2]);
    n = glm::normalize(n);
    h->normals.push_back(n.x);
    h->normals.push_back(n.y);
    h->normals.push_back(n.z);
  }

  //Top Triangles
  triangle(1,0,2);
  triangle(2,0,3);
  triangle(3,0,4);
  triangle(4,0,5);
  triangle(5,0,1);

  //Bottom Triangles
  triangle(6,7,11);
  triangle(7,8,11);
  triangle(8,9,11);
  triangle(9,10,11);
  triangle(10,6,11);

  //Connecting Triangles (Bottom)
  triangle(6,2,7);
  triangle(7,3,8);
  triangle(8,4,9);
  triangle(9,5,10);
  triangle(10,1,6);

  //Connecting Triangles (Top)
  triangle(2,3,7);
  triangle(1,2,6);
  triangle(3,4,8);
  triangle(4,5,9);
  triangle(5,1,10);

};


  std::function<void(Model* m)> split = [](Model* h){

    std::vector<GLuint> newind;

    auto addpos = [&](glm::vec3 p){
      h->positions.push_back(p.x);
      h->positions.push_back(p.y);
      h->positions.push_back(p.z);
    };

    auto addnorm = [&](glm::vec3 p){
      glm::vec3 n = glm::normalize(p);
      h->normals.push_back(n.x);
      h->normals.push_back(n.y);
      h->normals.push_back(n.z);
    };

    auto addcolor = [&](){
      h->colors.push_back(1);
      h->colors.push_back(1);
      h->colors.push_back(1);
      h->colors.push_back(1);
    };

    auto triangle = [&](int a, int b, int c){
      newind.push_back(a);
      newind.push_back(b);
      newind.push_back(c);
    };

    float r = 10.0f;

    for(int i = 0; i < h->indices.size()/3; i++){   //Loop over Old Triangles

      //Indicies of the old triangle
      GLuint k1 = h->indices[3*i+0];
      GLuint k2 = h->indices[3*i+1];
      GLuint k3 = h->indices[3*i+2];

      //Positions of the old triangle
      glm::vec3 a = glm::vec3(h->positions[3*k1+0], h->positions[3*k1+1], h->positions[3*k1+2]);
      glm::vec3 b = glm::vec3(h->positions[3*k2+0], h->positions[3*k2+1], h->positions[3*k2+2]);
      glm::vec3 c = glm::vec3(h->positions[3*k3+0], h->positions[3*k3+1], h->positions[3*k3+2]);

      //Compute Split Points
      glm::vec3 d = 0.5f*(a+b);
      glm::vec3 e = 0.5f*(b+c);
      glm::vec3 f = 0.5f*(c+a);

      //Normalize vectors to project onto sphere
      d = r*glm::normalize(d);
      e = r*glm::normalize(e);
      f = r*glm::normalize(f);

      //Starting Indices
      int NI = h->positions.size()/3;

      //Add New Positions to Model
      addpos(d); addpos(e); addpos(f);
      addnorm(d); addnorm(e); addnorm(f);
      addcolor();
      addcolor();
      addcolor();

      triangle(k1, NI+0, NI+2);
      triangle(k2, NI+1, NI+0);
      triangle(k3, NI+2, NI+1);
      triangle(NI+0, NI+1, NI+2);
    }

    //Copy Indices
    h->indices = newind;
  };

int main( int argc, char* args[] ) {

  Tiny::window("Circumcenter Iteration Map Sphere Projection", 800, 800);	//Open Window
  cam::near = -100.0f;
  cam::far = 100.0f;
  cam::init(5.0f, cam::ORTHO);

  Shader sphere({"shader/sphere.vs", "shader/sphere.fs"}, {"in_Position", "in_Normal"});

  Model icosahedron(_build);
  for(int i = 0; i < 5; i++)
    split(&icosahedron);
  icosahedron.update();

  float zoom = 0.05f;
  float WIDTH = 1000;
  float HEIGHT = 1000;

  float skew = 1.0f;

  Tiny::event.handler  = cam::handler;	//Set Event Handler
  Tiny::view.interface = [&](){
    ImGui::SetNextWindowSize(ImVec2(500, 100), ImGuiCond_Once);
    ImGui::Begin("Circumcenter Iteration Map Sphere Projection", NULL, ImGuiWindowFlags_NoResize);
    ImGui::DragFloat("Stretch X", &skew, 0.01f, 0.0f, 20.0f);

    ImGui::End();
  };	//Set Interface Function

  Tiny::view.pipeline = [&](){

    Tiny::view.target(glm::vec3(0));

    sphere.use();

    sphere.uniform("model", glm::mat4(1));			//Set Model Matrix
    sphere.uniform("vp", cam::vp);	//View Projection Matrix
    sphere.uniform("skew", skew);

    icosahedron.render();							  //Render Model with Lines

  };

  Tiny::loop([&](){											//Extra Stuff (every tick)

  });

  Tiny::quit();
}
