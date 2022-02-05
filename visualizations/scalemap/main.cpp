#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>

#include <iomanip>      // std::setprecision
#include <random>

using namespace std;
using namespace glm;

#define PI 3.14159265f

const int N = 8;
const double K = 10.0f;
const double EPSILON = 1E-16;

uniform_real_distribution<double> u(0.0, 1.0);

dvec3 spheresample(mt19937& g){

  double theta = 2 * M_PI * u(g);
  double phi = acos(1 - 2 * u(g));
  return dvec3(
    sin(phi) * cos(theta),
    sin(phi) * sin(theta),
    cos(phi)
  );

}

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

dvec2 original[N];
dvec2 tempsetA[N];
dvec2 tempsetB[N];

double getscale(dvec2 M){

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

int main( int argc, char* args[] ) {

  //Create Regular N-Gon

  for(int i = 0; i < N; i++){
    original[i] = dvec2(
      cos(double(i)/double(N)*2.0f*PI),
      sin(double(i)/double(N)*2.0f*PI)
    );
  }

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::mt19937 generator (seed);

  cout<<"Finding Smallest Scaling Value ("<<N<<")..."<<endl;

  double minscale = 1.0f;

//  glm::dvec3 s = spheresample(generator);
  glm::dvec2 near = glm::dvec2(0);//stereographic(s, K);

  // Generate Samples
  while(true){

    glm::dvec2 shift = 5.0*glm::dvec2(u(generator), u(generator)) - 2.5;
    double newscale = getscale(near + shift);

    if(newscale >= minscale) continue;

    near = near + shift;
    minscale = newscale;
    cout<<near.x<<" "<<near.y<<" "<<setprecision(12)<<minscale<<endl;

  }

  return 0;

}
