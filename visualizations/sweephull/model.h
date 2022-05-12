//Data
const int SIZE = 500;
int data[SIZE*SIZE] = {0};
bool paused = true;
float PI = 3.14159265;

//Resolution
int res = 25;
float radius = 2.0;
float bias[2] = {0.0};
double zoom = 10;
int order = 2;

float shift = -10.0f;
bool invert = false;

float cc[4] = {0.070, 0.086, 0.211, 1.0};
float dc[4] = {0.882, 0.157, 0.204, 1.0};

bool active = false;
float dshift = 0.1;
bool appear = false;
float appearAmp = 1.0;
bool movex = false;
bool movey = false;
float phase[2] = {0.0, 0.0};
float amplitude[2] = {1.0, 1.0};
float center[2] = {0.0, 0.0};
float rate = 0.01;
float t = 0.0;
glm::vec3 up = glm::vec3(0,1,0);

float angley = 0.0f;
float anglex = 0.0f;

glm::vec3 camerapos = glm::vec3(5.0);
glm::vec3 lookpos = glm::vec3(0.0);

// Event Handler
std::function<void()> eventHandler = [](){

  if(Tiny::event.scroll.posy)
    zoom /= 0.9;
  if(Tiny::event.scroll.negy)
    zoom *= 0.9;

  if(Tiny::event.active[SDLK_DOWN])
    angley -= 0.05;
  if(Tiny::event.active[SDLK_UP])
    angley += 0.05;

  if(Tiny::event.active[SDLK_RIGHT])
    anglex -= 0.05;
  if(Tiny::event.active[SDLK_LEFT])
    anglex += 0.05;

    if(angley <= -PI/2) angley = -PI/2+1E-2;
    if(angley >=  PI/2) angley = PI/2-1E-2;

  camerapos = 10.0f*glm::vec3(
    cos(angley)*cos(anglex),
    sin(angley),
    cos(angley)*sin(anglex)
  );


};

//Interface Function
Handle interfaceFunc = [](){
  //Window Size
  ImGui::SetNextWindowSize(ImVec2(480, 260), ImGuiCond_Once);
  ImGui::SetNextWindowPos(ImVec2(50, 470), ImGuiCond_Once);

  //Open Window
  ImGui::Begin("Raymarching Controller", NULL, ImGuiWindowFlags_NoResize);

    ImGui::ColorEdit3("Intersect Color", cc);
    ImGui::ColorEdit3("Miss Color", dc);
    ImGui::DragFloat("Shift", &shift, 0.01, -10, 20);
    ImGui::Checkbox("Invert", &invert);
    ImGui::Checkbox("Animate", &active);

  ImGui::End();
};
