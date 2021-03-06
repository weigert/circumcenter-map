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
uniform float pointsize;

uniform bool applylog;
uniform bool cutoff;
uniform float logscale;

uniform vec2 stretch;
uniform vec2 skew;

uniform int N;

out vec4 fragColor;

layout (std430, binding = 0) buffer pointset {
  vec2 p[];
};

const float PI = 3.14159265f;

const vec3 black = vec3(0);
const vec3 white = vec3(1);

const vec3 diverge = vec3(1.0, 0.85, 0.85);
const vec3 converge = vec3(0.88, 1, 0.88);

/*
============================
      Helper Functions
============================
*/

bool iscoplanar(vec2 M, vec2 A, vec2 B) {
  return abs(determinant(mat2(A.x-M.x, B.x-M.x, A.y-M.y, B.y-M.y))) < 1E-8;
}

vec2 circumcenter(vec2 M, vec2 A, vec2 B) {


  mat2 Q = inverse(mat2(A.x-M.x, B.x-M.x, A.y-M.y, B.y-M.y));
  //return Q*vec2(dot(A,B)-dot(M,B), dot(B,A)-dot(M,A));

  return Q*0.5*vec2(dot(A,A)-dot(M,M), dot(B,B)-dot(M,M));

  //vec2 v = vec2(dot(A,A)-dot(M,M), dot(B,B)-dot(M,M));

  // v += vec2(length(A-B) - length(M-B), length(A-B) - length(M-A));

  //return Q*0.5*v;

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

  //Affine Transformation Matrix
  mat3 affine = mat3(stretch.x, skew.x, 0, skew.y, stretch.y, 0, 0, 0, 1);

  //Background Color
  fragColor = vec4(1);

  vec2 polycenter = vec2(0);
  for(int i = 0; i < N; i++){ //First Copy
    polycenter += (affine*(vec3(p[i], 1.0))).xy;
  }
  polycenter /= float(N);

  //Position of this fragment
  vec2 M = (ex_Tex*2-vec2(1)+center)/zoom;

  //Perform Iteration (Extra Array - Not Possible in SSBO!)
  const int K = 32;

  vec2 tempsetA[K];
  vec2 tempsetB[K];
  for(int i = 0; i < N; i++){ //First Copy
    tempsetA[i] = (affine*(vec3(p[i], 1.0))).xy;
    tempsetB[i] = vec2(0);
  }

  //Check Co-Planarity
  for(int i = 0; i < N; i++){
    if(iscoplanar(M, tempsetA[i], tempsetA[(i+1)%N])){
      if(viewcolor == 0) fragColor = vec4(diverge, 1.0);
      if(viewcolor == 1) fragColor = vec4(black, 1.0);
      return;
    }
  }

  for(int i = 0; i < N; i++){   //Iterate N Times
    for(int k = 0; k < N; k++){ //Over N Intervals
      if(i%2 == 0) tempsetB[k] = circumcenter(M, tempsetA[k], tempsetA[(k+1)%N]);
      else tempsetA[k] = circumcenter(M, tempsetB[k], tempsetB[(k+1)%N]);
    }
  }

  vec2 transformedcenter = vec2(0);
  for(int i = 0; i < N; i++){ //First Copy
    if(N%2 == 0) transformedcenter += tempsetA[i];// = circumcenter(M, tempsetA[k], tempsetA[(k+1)%N]);
    else transformedcenter += tempsetB[i];// = circumcenter(M, tempsetB[k], tempsetB[(k+1)%N]);
  }
  transformedcenter /= float(N);

  //Viewing the Scale-Map
  if(viewtype == 0){

    float scale = 1.0;
    if(N%2 == 0) scale = length(tempsetA[1]-tempsetA[0])/length(p[1]-p[0]);
    else scale = length(tempsetB[1]-tempsetB[0])/length(p[1]-p[0]);


    if(viewcolor == 0){
      scale /= threshold;
      if(scale < 1) fragColor = vec4(converge,1);
      else fragColor = vec4(diverge, 1);


      if(abs(scale-1.0f) < 0.01)
        fragColor = vec4(0,0,0,1);


    }
    else{
      float value = 1.0;

      if(cutoff){
        if(scale > threshold) value = 0.0;
        if(scale < 0.0) scale = 0.0;
      }

      if(applylog){
        scale = log(1+logscale*scale);
      }

    //  scale = log(1.0+scale);
  //    scale = exp(-scale);

      fragColor = vec4(colorscheme(scale, value), 1.0);
    }



  }

  //Viewing the Rotation-Map
  if(viewtype == 1){

    vec2 d0 = p[0]-polycenter;
    vec2 dN;
    if(N%2 == 0) dN = tempsetA[0]-transformedcenter;
    else dN = tempsetB[0]-transformedcenter;

    float rot = acos(abs(dot(dN, d0))/length(dN)/length(d0));

    float thresh = dot(dN, vec2(-d0.y, d0.x));

    if(viewcolor == 0){
      if(thresh < 0) thresh = 0;
      else thresh = 1.0;
  //    if(abs(thresh) < 0.01) thresh = 0;
  //    else thresh = 1.0f;
      //  fragColor = mix(vec4(black,1), vec4(white, 1), 2.0f*rot/PI);
      fragColor = mix(vec4(diverge,1), vec4(converge, 1), thresh);


    }

    else fragColor = vec4(colorscheme(2.0f*rot/PI, 1), 1.0);

  }

/*
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
  */

  //Combined Map
  if(viewtype == 2){

    float rot = 0.0;
    if(N%2 == 0) rot = acos(abs(dot(tempsetA[1]-tempsetA[0], p[1]-p[0]))/length(tempsetA[1]-tempsetA[0])/length(p[1]-p[0]));
    else rot = acos(abs(dot(tempsetB[1]-tempsetB[0], p[1]-p[0]))/length(tempsetB[1]-tempsetB[0])/length(p[1]-p[0]));

    float scale = 1.0;
    if(N%2 == 0) scale = length(tempsetA[1]-tempsetA[0])/length(p[1]-p[0]);
    else scale = length(tempsetB[1]-tempsetB[0])/length(p[1]-p[0]);

    if(viewcolor == 1){
      if(scale > threshold) rot = 0.0f;

      if(applylog){
        scale = log(1+logscale*scale);
      }


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

  vec3 pointcolor = vec3(1,0,0);
  if(viewcolor == 1) pointcolor = white;

  if(viewpoints){

    if(length(M) < pointsize/zoom) fragColor = vec4(vec3(1,0,0), 1);

  }

  //Anchor Position Overlay

  if(viewanchor){

    if(length(M-anchor) < 1.5f*pointsize/zoom) fragColor = vec4(vec3(1,0,0),1);
    if(length(M-anchor) < pointsize/zoom) fragColor = vec4(white,1);

  }



}
