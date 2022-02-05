#version 430 core

in vec2 ex_Tex;
out vec4 fragColor;

uniform float zoom;
uniform float shift = 5;
uniform vec2 offset;

uniform vec4 converge;
uniform vec4 diverge;

uniform bool invert = false;

uniform vec3 camerapos;
vec3 lookpos = vec3(0);

vec3 lightpos;
vec3 lightcol = vec3(1);

//Signed Distance Field Functions
float box(vec3 p, vec3 b){
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

/*
================================================================================
                          Circumcenter Mapping
================================================================================
*/

bool iscoplanar(vec2 M, vec2 A, vec2 B) {
  float m1  = -((A.x - M.x) / (A.y - M.y));
  float m2  = -((B.x - A.x) / (B.y - A.y));
  if(abs(m2-m1) < 0.01) return true;
  return false;
}

vec2 circumcenter(vec2 M, vec2 A, vec2 B) {
  mat2 Q = inverse(mat2(A.x-M.x, B.x-M.x, A.y-M.y, B.y-M.y));
  return Q*0.5*vec2(dot(A,A)-dot(M,M), dot(B,B)-dot(M,M));
}

const int maxN = 3;
const int N = 3;
const float PI = 3.14159265f;

vec2 p[maxN];             //Original Points
vec2 tempsetA[maxN];      //Ping-Pong Array A
vec2 tempsetB[maxN];      //Ping-Pong Arrary B

float getscale(vec3 M){

  float scalex = log(M.y+shift);

  //Generate Point-Set

  for(int i = 0; i < N; i++)
  p[i] = vec2(
    cos(float(i)/float(N)*2.0f*PI),
    sin(float(i)/float(N)*2.0f*PI)*scalex
  );

  //Initialize Ping-Pong Arrays

  for(int i = 0; i < N; i++){
    tempsetA[i] = p[i];
    tempsetB[i] = vec2(0);
  }

  //Perform N-Iterations

  for(int i = 0; i < N; i++){
    for(int k = 0; k < N; k++){
      if(i%2 == 0) tempsetB[k] = circumcenter(M.xz, tempsetA[k], tempsetA[(k+1)%N]);
      else tempsetA[k] = circumcenter(M.xz, tempsetB[k], tempsetB[(k+1)%N]);
    }
  }

  float L0 = length(p[1]-p[0]);
  float L1 = (N%2 == 0)?length(tempsetA[1]-tempsetA[0]):
    length(tempsetB[1]-tempsetB[0]);

  float scale = L1 / L0;

  if(invert) return -0.1*log(scale);
  else return 0.1*log(scale);

}

/*
================================================================================
                    Signed Distance Field Computation
================================================================================
*/

float sdf(vec3 p){

  float a = box(p+vec3(0), vec3(5));
  float b = getscale(p);
  return max(a, b);

}

/*
================================================================================
                                Ray-Casting
================================================================================
*/

vec3 ray(vec3 start, vec3 dir, float mind, float maxd){
  vec3 p = start;
  float d = sdf(p);
  int maxiter = 1000;
  while(length(p - start) < maxd && d > mind && maxiter-- >= 0)
    d = sdf((p += d*dir));
  //if(maxiter == 0) return start+dir*maxd;
  return p;
}

vec3 sdfnormal(vec3 p, float d){
  float gx = (sdf(p + d*vec3(1,0,0)) - sdf(p - d*vec3(1,0,0)))/(2*d);
  float gy = (sdf(p + d*vec3(0,1,0)) - sdf(p - d*vec3(0,1,0)))/(2*d);
  float gz = (sdf(p + d*vec3(0,0,1)) - sdf(p - d*vec3(0,0,1)))/(2*d);
  return normalize(vec3(gx, gy, gz));
}

vec4 light(vec3 p){
  vec3 n = normalize((sdfnormal(p, 0.0001) + sdfnormal(p, 0.001) + sdfnormal(p, 0.01))/3.0f);
  float diffuse = 0.5*dot(n, lightpos);
  float specular = 0.1*pow(max(0.0, dot(reflect(normalize(lightpos), n), normalize(p - camerapos))), 64);
  float ambient = 1.0;
  return vec4(vec3(diffuse+specular+ambient)*lightcol, 1.0);
}

float shade(vec3 p){
  vec3 dir = normalize(p - lightpos);
  vec3 e = ray(lightpos, dir, 0.01, length(p-lightpos));
  if(length(e - p) < 0.1) return 1.0;
  else return 0.9;
}

void main(){

  lightpos = vec3(-camerapos.z, camerapos.y, camerapos.x);

  vec3 dir = normalize(lookpos - camerapos);          //Look Direction
  vec3 nx = normalize(cross(dir, vec3(0,1,0)));       //Normal Vector Sideways
  vec3 ny = normalize(cross(dir, nx));                //Normal Vector Up

  vec2 t = (ex_Tex*2-vec2(1.0))*zoom;                 //Fragment Position (2D Space on Plane)
  vec3 s = camerapos + t.x*nx + t.y*ny;               //Starting Position (3D Position on Plane)

  float maxd = 15.0;                                  //Stopping Position (Max Distance)
  float mind = 0.0001f;                                //Stopping Condition (Minimum Distance)

  vec3 e = ray(s, dir, mind, maxd);                   //Stopping Position
  float d = length(e - s)/maxd;                       //Distance to Camera

//  if(d > 1.0) fragColor = diverge;
  if(d < 1.0) fragColor = light(e)*converge;          //Hit
  else fragColor = diverge;                           //No-Hit

}
