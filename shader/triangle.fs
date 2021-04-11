#version 430 core

in vec2 ex_Tex;

uniform float zoom;
uniform vec2 center;

uniform int viewtype;
uniform int viewcolor;

uniform float threshold;

uniform bool viewpoints;
uniform bool viewanchor;
uniform vec2 anchor;

uniform int N;

out vec4 fragColor;

layout (std430, binding = 0) buffer pointset {
  vec2 p[];
};

const float PI = 3.14159265f;

const vec3 black = vec3(0);
const vec3 white = vec3(1);

/*
============================
      Helper Functions
============================
*/

vec2 circumcenter(vec2 M, vec2 A, vec2 B) {

  const float EPSILON = 1E-12;

  float fabsy1y2 = abs(M.y - A.y);
  float fabsy2y3 = abs(A.y - B.y);

  float xc, yc, m1, m2, mx1, mx2, my1, my2, dx, dy;

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

  return vec2(xc, yc);

}

vec3 hsv2rgb(vec3 c){
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 colorscheme(float t, float v){
  return hsv2rgb(vec3(t, 1, v));
}

//======================

void main(){

  //Background Color
  fragColor = vec4(1);

  //Position of this fragment
  vec2 M = (ex_Tex*2-vec2(1)+center)/zoom;

  //Perform Iteration (Extra Array - Not Possible in SSBO!)
  const int K = 16;

  vec2 tempsetA[K];
  vec2 tempsetB[K];
  for(int i = 0; i < N; i++){ //First Copy
    tempsetA[i] = p[i];
    tempsetB[i] = vec2(0);
  }

  for(int i = 0; i < N; i++){   //Iterate N Times
    for(int k = 0; k < N; k++){ //Over N Intervals
      if(i%2 == 0) tempsetB[k] = circumcenter(M, tempsetA[k], tempsetA[(k+1)%N]);
      else tempsetA[k] = circumcenter(M, tempsetB[k], tempsetB[(k+1)%N]);
    }
  }

  //Viewing the Scale-Map
  if(viewtype == 0){

    float scale = 1.0;
    if(N%2 == 0) scale = length(tempsetA[1]-tempsetA[0])/length(p[1]-p[0]);
    else scale = length(tempsetB[1]-tempsetB[0])/length(p[1]-p[0]);


    if(viewcolor == 0){
      scale /= threshold;
      if(scale < 1) scale = 0;
      else scale = 1;
      fragColor = mix(vec4(black,1), vec4(white, 1), scale);
    }
    else{
      float value = 1.0;
      if(scale > threshold) value = 0.0;
      fragColor = vec4(colorscheme(scale, value), 1.0);
    }


  }

  //Viewing the Rotation-Map
  if(viewtype == 1){

    float rot = 0.0;
    if(N%2 == 0) rot = acos(abs(dot(tempsetA[1]-tempsetA[0], p[1]-p[0]))/length(tempsetA[1]-tempsetA[0])/length(p[1]-p[0]));
    else rot = acos(abs(dot(tempsetB[1]-tempsetB[0], p[1]-p[0]))/length(tempsetB[1]-tempsetB[0])/length(p[1]-p[0]));

    if(viewcolor == 0){
      if(abs(rot) < 0.01) rot = 0;
      else rot = 2.0f*PI;
      fragColor = mix(vec4(black,1), vec4(white, 1), 2.0f*rot/PI);
    }
    else fragColor = vec4(colorscheme(2.0f*rot/PI, 1), 1.0);

  }

  //Combined Map
  if(viewtype == 2){

    float rot = 0.0;
    if(N%2 == 0) rot = acos(abs(dot(tempsetA[1]-tempsetA[0], p[1]-p[0]))/length(tempsetA[1]-tempsetA[0])/length(p[1]-p[0]));
    else rot = acos(abs(dot(tempsetB[1]-tempsetB[0], p[1]-p[0]))/length(tempsetB[1]-tempsetB[0])/length(p[1]-p[0]));

    float scale = 1.0;
    if(N%2 == 0) scale = length(tempsetA[1]-tempsetA[0])/length(p[1]-p[0]);
    else scale = length(tempsetB[1]-tempsetB[0])/length(p[1]-p[0]);

    if(viewcolor == 0){

      scale /= threshold;

      if(abs(scale-1) < 0.02) fragColor = vec4(1,0,0,1);
      if(abs(rot) < 0.02) fragColor = vec4(0,1,0,1);

    }
    else if(viewcolor == 1){
      if(scale > threshold) rot = 0.0f;

  //    fragColor = vec4(vec3(1.0-rot/2.0f/PI, 0.0, scale), 1.0);
    fragColor = vec4(colorscheme(scale, 2.0f*rot/PI), 1.0);

    }
    //fragColor = mix(vec4(black,1), vec4(white, 1), rot/2.0f/PI);

/*
    if(viewcolor == 0){
      if(abs(rot) < 0.01) rot = 0;
      else rot = 2.0f*PI;
      fragColor = mix(vec4(black,1), vec4(white, 1), rot/2.0f/PI);
    }
    else
*/

  }


  //Point Position Overlay

  if(viewpoints){

    vec3 pointcolor = vec3(1,0,0);
    if(viewcolor == 1) pointcolor = white;

    for(int i = 0; i < N; i++)
      if(length(M-p[i]) < 0.01/zoom) fragColor = vec4(pointcolor,1);

  }

  //Anchor Position Overlay

  if(viewanchor){

    vec3 pointcolor = vec3(1,0,0);
    if(viewcolor == 1) pointcolor = white;

    if(length(M-anchor) < 0.01/zoom) fragColor = vec4(pointcolor,1);

  }

}
