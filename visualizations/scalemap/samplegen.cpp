#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>

#include <random>

using namespace std;
using namespace glm;

#define PI 3.14159265f

uniform_real_distribution<double> u(0.0, 1.0);

vec3 spheresample(mt19937& g){

  double theta = 2 * M_PI * u(g);
  double phi = acos(1 - 2 * u(g));
  return vec3(
    sin(phi) * cos(theta),
    sin(phi) * sin(theta),
    cos(phi)
  );

}

vec2 circumcenter(vec2 M, vec2 A, vec2 B){
  mat2 Q = inverse(mat2(A.x-M.x, B.x-M.x, A.y-M.y, B.y-M.y));
  return Q*0.5f*vec2(dot(A,A)-dot(M,M), dot(B,B)-dot(M,M));
}

vec2 stereographic(vec3 p, float k){
  return k*vec2(p.x/(1.0f-p.y), p.z/(1.0f-p.y));       //Projected Plane
}

const int N = 3;
vec2 original[N];
vec2 tempsetA[N];
vec2 tempsetB[N];

float getscale(vec2 M){

  for(int i = 0; i < N; i++){
    tempsetA[i] = original[i];      //Copy
    tempsetB[i] = vec2(0);          //Empty Set
  }

  for(int i = 0; i < N; i++){   //Iterate N Times
    for(int k = 0; k < N; k++){ //Over N Intervals
      if(i%2 == 0) tempsetB[k] = circumcenter(M, tempsetA[k], tempsetA[(k+1)%N]);
      else tempsetA[k] = circumcenter(M, tempsetB[k], tempsetB[(k+1)%N]);
    }
  }

  float L0, L1;
  L0 = length(original[1] - original[0]);
  L1 = (N%2 == 0)? length(tempsetA[1] - tempsetA[0]):
              length(tempsetB[1] - tempsetB[0]);
  return L1 / L0;

}

int main( int argc, char* args[] ) {

  //Create Regular N-Gon

  for(int i = 0; i < N; i++){
    original[i] = vec2(
      cos(float(i)/float(N)*2.0f*PI),
      sin(float(i)/float(N)*2.0f*PI)
    );
  }

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::mt19937 generator (seed);

  //Size of the N-Gon

  const int K = (2 << 14);
  const float scale = 1.0f;

  cout<<"Generating Samples"<<endl;

  vector<float> scales;

  // Generate Samples
  while(scales.size() < K){
    float S = getscale(stereographic(spheresample(generator), scale));
    if(S <= 1) scales.push_back(S);
  }

  string scalefile = "scales_N"+to_string(N)+"_S"+to_string(K)+"_K"+to_string(scale)+".txt";

  fstream scaleout(scalefile, ios::out);
  if(!scaleout.is_open()){
    cout<<"Failed to Open Output File..."<<endl;
    exit(0);
  }


  std::sort(scales.begin(), scales.end());

  // Output the Samples
  for(auto& s: scales)
    scaleout<<s<<endl;

  scaleout.close();
  return 0;

}
