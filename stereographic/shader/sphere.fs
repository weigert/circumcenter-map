#version 430 core

in vec4 ex_Color;
in vec3 ex_Normal;
in vec4 ex_FragPos;
in vec3 lightpos;

out vec4 fragColor;

uniform int N;
const int maxN = 16;
const float PI = 3.14159265f;

uniform float scale;

const vec3 diverge = vec3(1, 0, 0);
const vec3 converge = vec3(0, 1, 0);
const vec3 black = vec3(0);

bool iscoplanar(vec2 M, vec2 A, vec2 B) {
  float m1  = -((A.x - M.x) / (A.y - M.y));
  float m2  = -((B.x - A.x) / (B.y - A.y));
  if(abs(m2-m1) < 0.01) return true;
  return false;
}



vec2 circumcenter(vec2 M, vec2 A, vec2 B) {

  const float EPSILON = 1E-6;

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



vec3 planesample(vec2 pos){

  vec2 M = pos;

  vec2 p[maxN];
  vec2 tempsetA[maxN];
  vec2 tempsetB[maxN];

  for(int i = 0; i < N; i++){ //First Copy
    p[i] = vec2(cos(float(i)/float(N)*2.0f*PI), sin(float(i)/float(N)*2.0f*PI));
  //  p[i].x *= scale;
  }

  for(int i = 0; i < N; i++){ //First Copy
    tempsetA[i] = p[i];
    tempsetB[i] = vec2(0);
  }

  //Check Co-Planarity
  for(int i = 0; i < N; i++){
    if(iscoplanar(M, tempsetA[i], tempsetA[(i+1)%N])){
      return vec3(0);
    }
  }

  for(int i = 0; i < N; i++){   //Iterate N Times
    for(int k = 0; k < N; k++){ //Over N Intervals
      if(i%2 == 0) tempsetB[k] = circumcenter(M, tempsetA[k], tempsetA[(k+1)%N]);
      else tempsetA[k] = circumcenter(M, tempsetB[k], tempsetB[(k+1)%N]);
    }
  }


  vec2 polycenter = vec2(0);
  for(int i = 0; i < N; i++){ //First Copy
    polycenter += p[i];
  }
  polycenter /= float(N);


  vec2 transformedcenter = vec2(0);
  for(int i = 0; i < N; i++){ //First Copy
    if(N%2 == 0) transformedcenter += tempsetA[i];// = circumcenter(M, tempsetA[k], tempsetA[(k+1)%N]);
    else transformedcenter += tempsetB[i];// = circumcenter(M, tempsetB[k], tempsetB[(k+1)%N]);
  }
  transformedcenter /= float(N);



  float scale = 1.0;
  if(N%2 == 0) scale = length(tempsetA[1]-tempsetA[0])/length(p[1]-p[0]);
  else scale = length(tempsetB[1]-tempsetB[0])/length(p[1]-p[0]);
  //  scale = log(scale);
    if(scale < 1) scale = 0;
    else scale = 1;
  //  if(scale > 1) return vec3(0);
    return mix(converge, diverge, scale);
  //return colorscheme(scale, 1.0);


/*
  vec2 d0 = p[0]-polycenter;
  vec2 dN;
  if(N%2 == 0) dN = tempsetA[0]-transformedcenter;
  else dN = tempsetB[0]-transformedcenter;

  float rot = acos(abs(dot(dN, d0))/length(dN)/length(d0));

  return colorscheme(2.0f*rot/PI, 1);
*/

}

void main() {

  vec3 cpos = normalize(ex_FragPos.xyz);                              //Normalized Sphere
  vec2 ppos = vec2(scale*cpos.x/(1.0f-cpos.y), scale*cpos.z/(1.0f-cpos.y));       //Projected Plane

  float shade = 0.5f+0.5f*dot(normalize(lightpos), normalize(ex_Normal));

  vec3 color = planesample(ppos);
  fragColor = vec4(shade*color, 1.0);

}
