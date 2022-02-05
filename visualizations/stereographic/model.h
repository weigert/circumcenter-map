/*

Icosphere Construction Functions

*/

using namespace glm;
#define PI 3.14159265f

int N = 3;
float K = 1.0f;
float EPSILON = 1E-16;

bool colinear(dvec2 M, dvec2 A, dvec2 B){
  dmat2 G = dmat2(A.x-M.x, B.x-M.x, A.y-M.y, B.y-M.y);
  return (abs(determinant(G)) < EPSILON);
}

dvec2 circumcenter(dvec2 M, dvec2 A, dvec2 B){
  dmat2 Q = inverse(dmat2(A.x-M.x, B.x-M.x, A.y-M.y, B.y-M.y));
  return Q*0.5*dvec2(dot(A,A)-dot(M,M), dot(B,B)-dot(M,M));
}

dvec2 stereographic(dvec3 p, double k){
  return k*dvec2(p.x/(1.0f-p.y), p.z/(1.0f-p.y));       //Projected Plane
}

float getscale(dvec2 M){

  dvec2 original[N];
  dvec2 tempsetA[N];
  dvec2 tempsetB[N];

  for(int i = 0; i < N; i++){
    original[i] = dvec2(
      cos(double(i)/double(N)*2.0f*PI),
      sin(double(i)/double(N)*2.0f*PI)
    );
  }

  for(int i = 0; i < N; i++){
    tempsetA[i] = original[i];      //Copy
    tempsetB[i] = dvec2(0);          //Empty Set
  }

  for(int i = 0; i < N; i++){   //Iterate N Times

    for(int k = 0; k < N; k++){   //Iterate N Times
      if(i%2 == 0 && colinear(M, tempsetA[k], tempsetA[(k+1)%N])) return 1.0f;
      if(i%2 == 1 && colinear(M, tempsetB[k], tempsetB[(k+1)%N])) return 1.0f;
    }

    for(int k = 0; k < N; k++){ //Over N Intervals
      if(i%2 == 0) tempsetB[k] = circumcenter(M, tempsetA[k], tempsetA[(k+1)%N]);
      else tempsetA[k] = circumcenter(M, tempsetB[k], tempsetB[(k+1)%N]);
    }

  }

  double L0, L1;
  L0 = length(original[1] - original[0]);
  L1 = (N%2 == 0)? length(tempsetA[1] - tempsetA[0]):
              length(tempsetB[1] - tempsetB[0]);
  return L1 / L0;

}

void isobuild(vector<vec3>& positions, vector<uvec3>& indices){

  const float r = 10.0f;
  const float a = 72.0f/360.0f*2.0f*3.14159265f;

  //Helper Methods
  auto addpos = [&](float a, float b, float c){
    positions.emplace_back(a, b, c);
  };

  //For a Triangles
  auto triangle = [&](int a, int b, int c){
    indices.emplace_back(a, b, c);
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

void isosplit(vector<vec3>& positions, vector<uvec3>& indices){

  std::vector<uvec3> newind;

  auto addpos = [&](vec3 p){
    positions.push_back(p);
  };

  auto triangle = [&](int a, int b, int c){
    newind.emplace_back(a, b, c);
  };

  float r = 10.0f;

  for(int i = 0; i < indices.size(); i++){   //Loop over Old Triangles

    //Indicies of the old triangle
    GLuint k1 = indices[i][0];
    GLuint k2 = indices[i][1];
    GLuint k3 = indices[i][2];

    //Positions of the old triangle
    vec3 a = positions[k1];//vec3(positions[k1][0], positions[k1][1], positions[k1][2]);
    vec3 b = positions[k2];//vec3(positions[k2][0], positions[k2][1], positions[k2][2]);
    vec3 c = positions[k3];//vec3(positions[k3][0], positions[k3][1], positions[k3][2]);

    //Compute Split Points
    vec3 d = 0.5f*(a+b);
    vec3 e = 0.5f*(b+c);
    vec3 f = 0.5f*(c+a);

    //Normalize vectors to project onto sphere
    d = r*normalize(d);
    e = r*normalize(e);
    f = r*normalize(f);

    //Starting Indices
    int NI = positions.size();

    //Add New Positions to Model
    addpos(d); addpos(e); addpos(f);

    triangle(k1, NI+0, NI+2);
    triangle(k2, NI+1, NI+0);
    triangle(k3, NI+2, NI+1);
    triangle(NI+0, NI+1, NI+2);
  }

  indices = newind;

}
