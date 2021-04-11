
/*
===================================================
            Visualization Parameters
===================================================
*/

#define PI 3.14159265f

double zoom = 0.2;

glm::vec2 center = glm::vec2(0);
glm::vec2 anchor = glm::vec2(-2.0, 0.0);

bool viewpoints = true;
bool viewrotation = false;
bool viewanchor = true;
int viewtype = 0;
int viewcolor = 0;
float threshold = 1.0;

int N = 3;
const int minN = 3;
const int maxN = 16;

bool viewiteration = true;
int depth = 1;

bool isselected = false;
bool anchorselected = false;
int selected = 0;

bool update = false;

float frametime = 0.0f;

std::vector<glm::vec2> pointset;
std::vector<glm::vec2> triangleset;

//Functions

glm::vec2 circumcenter(glm::vec2 M, glm::vec2 A, glm::vec2 B) {

    const float EPSILON = 1E-12;

    float fabsy1y2 = abs(M.y - A.y);
    float fabsy2y3 = abs(A.y - B.y);

    float xc, yc, m1, m2, mx1, mx2, my1, my2, dx, dy;

    /* Check for coincident points */
    if(fabsy1y2 < EPSILON && fabsy2y3 < EPSILON) return M;

    if(fabsy1y2 < EPSILON) {
        m2  = -((B.x - A.x) / (B.y - A.y));
        mx2 = (A.x + B.x) / 2.0;
        my2 = (A.y + B.y) / 2.0;
        xc  = (A.x + M.x) / 2.0;
        yc  = m2 * (xc - mx2) + my2;
    }

    else if(fabsy2y3 < EPSILON) {
        m1  = -((A.x - M.x) / (A.y - M.y));
        mx1 = (M.x + A.x) / 2.0;
        my1 = (M.y + A.y) / 2.0;
        xc  = (B.x + A.x) / 2.0;
        yc  = m1 * (xc - mx1) + my1;
    }

    else {
        m1  = -((A.x - M.x) / (A.y - M.y));
        m2  = -((B.x - A.x) / (B.y - A.y));
        mx1 = (M.x + A.x) / 2.0;
        mx2 = (A.x + B.x) / 2.0;
        my1 = (M.y + A.y) / 2.0;
        my2 = (A.y + B.y) / 2.0;
        xc  = (m1 * mx1 - m2 * mx2 + my2 - my1) / (m1 - m2);
        yc  = (fabsy1y2 > fabsy2y3) ?
        m1 * (xc - mx1) + my1 :
        m2 * (xc - mx2) + my2;
    }

    return glm::vec2(xc, yc);

}

void computeTriangles(){
  triangleset.clear();
  for(int i = 0; i < N; i++)
    triangleset.push_back(pointset[i]);
  for(int i = 0; i < depth; i++){
    for(int j = 0; j < N; j++){
      triangleset.push_back(circumcenter(anchor, triangleset[i*N+j], triangleset[i*N+(j+1)%N]));
    }
  }
  update = true;
}



// Event Handler
std::function<void()> eventHandler = [](){

  if(Tiny::event.scroll.posy)
    zoom *= 0.9;
  if(Tiny::event.scroll.negy)
    zoom /= 0.9;

  if(Tiny::event.mousemove){

    if(isselected){

      glm::vec2 clickpos = glm::vec2(Tiny::event.mouse.x, Tiny::event.mouse.y);
      clickpos = 2.0f * clickpos / glm::vec2(1000, 1000) - glm::vec2(1.0) + center;
      pointset[selected] = clickpos/(float)zoom;
      triangleset[selected] = clickpos/(float)zoom;
      update = true;

      computeTriangles();

    }

    if(anchorselected){

      glm::vec2 clickpos = glm::vec2(Tiny::event.mouse.x, Tiny::event.mouse.y);
      clickpos = 2.0f * clickpos / glm::vec2(1000, 1000) - glm::vec2(1.0) + center;
      anchor = clickpos/(float)zoom;
      update = true;

      computeTriangles();

    }

  }

  if(!Tiny::event.clicked.empty()){
    if(Tiny::event.clicked.back() == SDL_BUTTON_LEFT){

      if(isselected || anchorselected){
        isselected = false;
        anchorselected = false;
      }
      else {

        glm::vec2 clickpos = glm::vec2(Tiny::event.mouse.x, Tiny::event.mouse.y);
        clickpos = 2.0f * clickpos / glm::vec2(1000, 1000) - glm::vec2(1.0) + center;


      if(glm::length(anchor - clickpos/(float)zoom) < 0.03/zoom){
        anchorselected = true;
        isselected = false;
      }

      else
        for(size_t i = 0; i < pointset.size(); i++){

          if(glm::length(pointset[i] - clickpos/(float)zoom) < 0.03/zoom){
            isselected = true;
            selected = i;
            break;
          }

        }



      }

    }

  }

  if(Tiny::event.active[SDLK_w]) center += glm::vec2( 0.0,-0.01);
  if(Tiny::event.active[SDLK_a]) center += glm::vec2(-0.01, 0.0);
  if(Tiny::event.active[SDLK_s]) center += glm::vec2( 0.0, 0.01);
  if(Tiny::event.active[SDLK_d]) center += glm::vec2( 0.01, 0.0);

};







//Interface Function
Handle interfaceFunc = [](){

  ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_Once);

  //Open Window
  ImGui::Begin("Triangle Fractal Controller", NULL, ImGuiWindowFlags_NoResize);

  if(ImGui::BeginTabBar("Tab Bar", ImGuiTabBarFlags_None)){
    if(ImGui::BeginTabItem("Visualization")){

      ImGui::Text("Frametime (ms): %f", frametime);

      ImGui::DragFloat2("Center", &center[0], 0.001f, -10.0f, 10.0f);


        if(ImGui::DragInt("N", &N, 1, minN, maxN)){

          pointset.clear();
          for(int i = 0; i < N; i++){
            pointset.push_back(glm::vec2(cos(2.0f*PI*(float)i/(float)N), sin(2.0f*PI*(float)i/(float)N)));
          }

          computeTriangles();
          update = true;

        }

      ImGui::EndTabItem();
    }
    if(ImGui::BeginTabItem("Iteration")){

      ImGui::Checkbox("View Points", &viewpoints); ImGui::SameLine();
      ImGui::Checkbox("View Anchor", &viewanchor); ImGui::SameLine();
      ImGui::Checkbox("View Iteration", &viewiteration);

      if(ImGui::DragFloat2("Anchor", &anchor[0], 0.001f, -10.0f, 10.0f)){
        computeTriangles();
      }

      if(ImGui::DragInt("Iteration Depth", &depth, 1, 1, 64)){
        computeTriangles();
      }

      ImGui::EndTabItem();
    }
    if(ImGui::BeginTabItem("Coloring")){

      ImGui::RadioButton("Scaling Field", &viewtype, 0); ImGui::SameLine();
      ImGui::RadioButton("Rotation Field", &viewtype, 1); ImGui::SameLine();
      ImGui::RadioButton("Combined", &viewtype, 2);

      if(ImGui::RadioButton("Black/White", &viewcolor, 0)) computeTriangles(); ImGui::SameLine();
      if(ImGui::RadioButton("Color Gradient", &viewcolor, 1)) computeTriangles();

      ImGui::DragFloat("Threshold", &threshold, 0.01f, 0.1f, 20.0f);

      ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
  }

  ImGui::End();
};




std::function<void(Model* m, std::vector<glm::vec2>)> construct_strip = [](Model* h, std::vector<glm::vec2> points){

  for(int i = 0; i < depth; i++){
    for(size_t j = 0; j < N; j++){
      h->positions.push_back(triangleset[i*N+j].x);//, points[i].y, 0.0f);
      h->positions.push_back(triangleset[i*N+j].y);//, points[i].y, 0.0f);
      h->positions.push_back(1.0f);//, points[i].y, 0.0f);
      h->positions.push_back(triangleset[i*N+(j+1)%N].x);//, points[i].y, 0.0f);
      h->positions.push_back(triangleset[i*N+(j+1)%N].y);//, points[i].y, 0.0f);
      h->positions.push_back(1.0f);//, points[i].y, 0.0f);

      if(viewcolor == 0){
        if(i%N == 0){
          h->add(h->colors, glm::vec4(1,0,0,1));
          h->add(h->colors, glm::vec4(1,0,0,1));
        }
        if(i%N == 1){
          h->add(h->colors, glm::vec4(0,1,0,1));
          h->add(h->colors, glm::vec4(0,1,0,1));
        }
        if(i%N == 2){
          h->add(h->colors, glm::vec4(0,0,1,1));
          h->add(h->colors, glm::vec4(0,0,1,1));
        }
        if(i%N == 3){
          h->add(h->colors, glm::vec4(1,1,0,1));
          h->add(h->colors, glm::vec4(1,1,0,1));
        }
        if(i%N == 4){
          h->add(h->colors, glm::vec4(0,1,1,1));
          h->add(h->colors, glm::vec4(0,1,1,1));
        }
        if(i%N == 5){
          h->add(h->colors, glm::vec4(1,0,1,1));
          h->add(h->colors, glm::vec4(1,0,1,1));
        }
        if(i%N == 6){
          h->add(h->colors, glm::vec4(1,0.5,0,1));
          h->add(h->colors, glm::vec4(1,0.5,0,1));
        }
        if(i%N == 7){
          h->add(h->colors, glm::vec4(0.5,1,0,1));
          h->add(h->colors, glm::vec4(0.5,1,0,1));
        }
        if(i%N == 8){
          h->add(h->colors, glm::vec4(0,1,0.5,1));
          h->add(h->colors, glm::vec4(0,1,0.5,1));
        }
        if(i%N == 9){
          h->add(h->colors, glm::vec4(0,0.5,1,1));
          h->add(h->colors, glm::vec4(0,0.5,1,1));
        }
        if(i%N == 10){
          h->add(h->colors, glm::vec4(0.5,0,1,1));
          h->add(h->colors, glm::vec4(0.5,0,1,1));
        }
        if(i%N == 11){
          h->add(h->colors, glm::vec4(1,0,0.5,1));
          h->add(h->colors, glm::vec4(1,0,0.5,1));
        }

      }
      else{
        h->add(h->colors, glm::vec4(1));
        h->add(h->colors, glm::vec4(1));
      }

    }
  }

};
