
/*
===================================================
            Visualization Parameters
===================================================
*/

#define PI 3.14159265f

int SIZEX = 800;
int SIZEY = 800;

double zoom = 0.2;

glm::vec2 center = glm::vec2(0);
glm::vec2 anchor = glm::vec2(-2.0, 0.0);

glm::vec2 stretch = glm::vec2(1);
glm::vec2 skew = glm::vec2(0);

glm::vec2 mousepos = glm::vec2(0);

bool viewpoints = true;
bool viewrotation = false;
bool viewanchor = false;
int viewtype = 0;
int viewcolor = 0;
float threshold = 1.0;

float pointsize = 0.01;

int N = 3;
const int minN = 3;
const int maxN = 32;

char filename[256] = "output.png";
//char* filename = new char[256];
//filename = "output.png";

bool viewiteration = true;
int depth = 1;

bool isselected = false;
bool anchorselected = false;
int selected = 0;

bool applylog = false;
float logscale = 1.0f;
bool cutoff = true;

bool update = false;

bool isolines = true;

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

glm::vec2 scalerot(){
  std::vector<glm::vec2> newset;
  glm::vec2 _scalerot = glm::vec2(0);
  for(int i = 0; i < N; i++)
    newset.push_back(pointset[i]);
  for(int i = 0; i < N; i++){
    for(int j = 0; j < N; j++){
      newset.push_back(circumcenter(mousepos, newset[i*N+j], newset[i*N+(j+1)%N]));
    }
  }

  glm::vec2 originalcenter = glm::vec2(0);
  glm::vec2 transformcenter = glm::vec2(0);
  for(int i = 0; i < N; i++){
    originalcenter += pointset[i]/(float)N;
    transformcenter += newset[N*N+i]/(float)N;
  }

  _scalerot.x = glm::length(newset[N*N]-transformcenter)/glm::length(newset[0]-originalcenter);
  _scalerot.y = acos((dot(newset[N*N]-transformcenter, newset[0]-originalcenter))/glm::length(newset[N*N]-transformcenter)/glm::length(newset[0]-originalcenter));

  return _scalerot;
}

glm::vec2 atpos = glm::vec2(0);




std::function<void(Buffer*, Buffer*, std::vector<glm::vec2>&)> construct_strip = [](Buffer* posbuf, Buffer* colbuf, std::vector<glm::vec2>& points){

  std::vector<glm::vec2> positions;
  std::vector<glm::vec4> colors;

  for(int i = 0; i < depth; i++){
    for(size_t j = 0; j < N; j++){

      positions.push_back(points[i*N+j]);
      positions.push_back(points[i*N+(j+1)%N]);

      if(viewcolor == 0){

        if(i%N == 0){
          colors.push_back(glm::vec4(1,0,0,1));
          colors.push_back(glm::vec4(1,0,0,1));
        }
        if(i%N == 1){
          colors.push_back(glm::vec4(0,1,0,1));
          colors.push_back(glm::vec4(0,1,0,1));
        }
        if(i%N == 2){
          colors.push_back(glm::vec4(0,0,1,1));
          colors.push_back(glm::vec4(0,0,1,1));
        }
        if(i%N == 3){
          colors.push_back(glm::vec4(1,1,0,1));
          colors.push_back(glm::vec4(1,1,0,1));
        }
        if(i%N == 4){
          colors.push_back(glm::vec4(0,1,1,1));
          colors.push_back(glm::vec4(0,1,1,1));
        }
        if(i%N == 5){
          colors.push_back(glm::vec4(1,0,1,1));
          colors.push_back(glm::vec4(1,0,1,1));
        }
        if(i%N == 6){
          colors.push_back(glm::vec4(1,0.5,0,1));
          colors.push_back(glm::vec4(1,0.5,0,1));
        }
        if(i%N == 7){
          colors.push_back(glm::vec4(0.5,1,0,1));
          colors.push_back(glm::vec4(0.5,1,0,1));
        }
        if(i%N == 8){
          colors.push_back(glm::vec4(0,1,0.5,1));
          colors.push_back(glm::vec4(0,1,0.5,1));
        }
        if(i%N == 9){
          colors.push_back(glm::vec4(0,0.5,1,1));
          colors.push_back(glm::vec4(0,0.5,1,1));
        }
        if(i%N == 10){
          colors.push_back(glm::vec4(0.5,0,1,1));
          colors.push_back(glm::vec4(0.5,0,1,1));
        }
        if(i%N == 11){
          colors.push_back(glm::vec4(1,0,0.5,1));
          colors.push_back(glm::vec4(1,0,0.5,1));
        }
    //    else{
    //      colors.push_back(glm::vec4(1));
    //      colors.push_back(glm::vec4(1));
    //    }
      }
      else{
        colors.push_back(glm::vec4(1));
        colors.push_back(glm::vec4(1));
      }
    }
  }

  posbuf->fill(positions);
  colbuf->fill(colors);

};


// Event Handler
std::function<void()> eventHandler = [](){

  if(Tiny::event.scroll.posy)
    zoom += 0.01;
  if(Tiny::event.scroll.negy)
    zoom -= 0.01;

  if(zoom < 0.01) zoom = 0.01;

  mousepos = glm::vec2(Tiny::event.mouse.x, Tiny::event.mouse.y);
  mousepos = (glm::vec2(1.0, -1.0)*(2.0f * mousepos / glm::vec2(SIZEX, SIZEY) - glm::vec2(1.0)) + center)/float(zoom);

  if(Tiny::event.mousemove){
    atpos = scalerot();

    if(isselected){

      pointset[selected] = mousepos;
      triangleset[selected] = mousepos;
      update = true;

      computeTriangles();

    }

    if(anchorselected){

      anchor = mousepos;
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

        if(glm::length(anchor - mousepos) < 0.03/zoom){
          anchorselected = true;
          isselected = false;
        }

        else {
          for(size_t i = 0; i < pointset.size(); i++){

              glm::mat3 affine = glm::mat3(stretch.x, skew.x, 0.0, skew.y, stretch.y, 0.0, 0.0, 0.0, 1.0);
              glm::vec2 transformed = glm::vec2(affine*glm::vec3(pointset[i], 1.0));

              if(glm::length(transformed - mousepos) < 0.03/zoom){
                isselected = true;
                selected = i;

                //Reset Affine Transform Matrix, Transform the Points Indefinitely
                for(size_t i = 0; i < pointset.size(); i++){
                  pointset[i] =  glm::vec2(affine*glm::vec3(pointset[i], 1.0));
                }
                computeTriangles();

                skew = glm::vec2(0);
                stretch = glm::vec2(1);

                break;
              }

          }
        }

      }
    }
  }

  if(Tiny::event.active[SDLK_w]) center += glm::vec2( 0.0, 0.01);
  if(Tiny::event.active[SDLK_a]) center += glm::vec2(-0.01, 0.0);
  if(Tiny::event.active[SDLK_s]) center += glm::vec2( 0.0, -0.01);
  if(Tiny::event.active[SDLK_d]) center += glm::vec2( 0.01, 0.0);

};







//Interface Function
Handle interfaceFunc = [](){

  ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_Once);
  ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_Once);

  //Open Window
  ImGui::Begin("Iterative Circumcenter Map (c) N. McDonald, D. Reznik", NULL, ImGuiWindowFlags_NoResize);

  if(ImGui::BeginTabBar("Tab Bar", ImGuiTabBarFlags_None)){
    if(ImGui::BeginTabItem("Visualization")){

      ImGui::Text("Frametime (ms): %f", frametime);

      ImGui::DragFloat2("Center", &center[0], 0.001f, -10.0f, 10.0f);

      if(ImGui::DragFloat("Point Size", &Tiny::view.pointSize, 1.0f, 1.0f, 20.0f)){
         glPointSize(Tiny::view.pointSize);
      }
      if(ImGui::DragFloat("Line Width", &Tiny::view.lineWidth, 1.0f, 1.0f, 20.0f)){
         glLineWidth(Tiny::view.lineWidth);
      }

      ImGui::Text("Mouse Over: [%f, %f]", mousepos.x, mousepos.y);
      ImGui::Text("At Mouse - Scale: %f, Rotation: %f", atpos.x, atpos.y);
      ImGui::Text("Viewport: x[%f, %f], y[%f, %f]", (center.x-1)/zoom, (center.x+1)/zoom, (center.y-1)/zoom, (center.y+1)/zoom);

      ImGui::InputText("Filename", filename, 256);
      if(ImGui::Button("Export Image")){

        SDL_Surface *s = SDL_CreateRGBSurface(0, Tiny::view.WIDTH, Tiny::view.HEIGHT, 32, 0x0000ff, 0x00ff00, 0xff0000, 0);
        SDL_LockSurface(s);
        glBindFramebuffer(GL_FRAMEBUFFER, 0); glReadBuffer(GL_COLOR_BUFFER_BIT);
        glReadPixels(0, 0, Tiny::view.WIDTH, Tiny::view.HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
        SDL_UnlockSurface(s);
        image::save(s, filename);
        delete s;
        std::cout<<"Exported"<<std::endl;

      }

      ImGui::EndTabItem();

    }

    if(ImGui::BeginTabItem("Geometry")){

      if(ImGui::DragInt("N", &N, 1, minN, maxN)){

        pointset.clear();
        for(int i = 0; i < N; i++){
          pointset.push_back(glm::vec2(cos(2.0f*PI*(float)i/(float)N), sin(2.0f*PI*(float)i/(float)N)));
        }

        computeTriangles();
        update = true;

      } ImGui::SameLine();

      if(ImGui::Button("Reset N-gon")){
        pointset.clear();
        for(int i = 0; i < N; i++){
          pointset.push_back(glm::vec2(cos(2.0f*PI*(float)i/(float)N), sin(2.0f*PI*(float)i/(float)N)));
        }
        computeTriangles();
        stretch = glm::vec2(1);
        skew = glm::vec2(0);
      }

      ImGui::Text("Affine Transform");
      ImGui::DragFloat2("Stretch", &stretch[0], 0.01f, 0.0f, 100.0f);
      ImGui::DragFloat2("Skew", &skew[0], 0.01f, -100.0f, 100.0f);


      ImGui::Text("Point List");
      char a;
      for(int i = 0; i < pointset.size(); i++){
        ImGui::Text("[%d]: ", i); ImGui::SameLine();
        a = i+'0';
        if(ImGui::DragFloat2(&a, &pointset[i][0], 0.01f, -10.0f, 10.0f)){
          computeTriangles();

        }
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

      ImGui::Text("Field: "); ImGui::SameLine();
      ImGui::RadioButton("Scaling", &viewtype, 0); ImGui::SameLine();
      ImGui::RadioButton("Rotation", &viewtype, 1); ImGui::SameLine();
      ImGui::RadioButton("Combined", &viewtype, 2);

      ImGui::Text("Scheme"); ImGui::SameLine();
      if(ImGui::RadioButton("Binary", &viewcolor, 0)) computeTriangles(); ImGui::SameLine();
      if(ImGui::RadioButton("Gradient", &viewcolor, 1)) computeTriangles();

      ImGui::Checkbox("View Isolines", &isolines); ImGui::SameLine();
      ImGui::Checkbox("Log-Scale (1 + k*scale)", &applylog);
      ImGui::DragFloat("k", &logscale, 0.01f, 0.01f, 50.0f);

      ImGui::DragFloat("Isoline", &threshold, 0.01f, 0.1f, 20.0f);
      ImGui::Checkbox("Cutoff at Isoline", &cutoff);

      ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
  }

  ImGui::End();
};
