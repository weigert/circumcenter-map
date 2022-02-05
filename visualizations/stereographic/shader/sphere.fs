/*
Iterative Circumcenter Map
Fragment Shader
Author: Nicholas McDonald, 2021-2022
*/

#version 430 core

flat in vec3 lightpos;
in vec3 ex_Normal;

out vec4 fragColor;

uniform int N;
const int maxN = 8;
const float PI = 3.14159265f;

uniform float scale;

uniform vec4 diverge;
uniform vec4 converge;
uniform int viewmode;

/*
Compute Convergence vs. Divergence
*/

vec2 circumcenter(vec2 M, vec2 A, vec2 B) {
  mat2 Q = inverse(mat2(A.x-M.x, B.x-M.x, A.y-M.y, B.y-M.y));
  return Q*0.5*vec2(dot(A,A)-dot(M,M), dot(B,B)-dot(M,M));
}

vec3 hsv2rgb(vec3 c){
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 colorscheme(float t, float v){
  return hsv2rgb(vec3(t, 1, v));
}

vec4 planesample(vec2 M){

  vec2 p[maxN];
  vec2 tempsetA[maxN];
  vec2 tempsetB[maxN];

  // Set of Points

  for(int i = 0; i < N; i++){
    p[i] = vec2(
      cos(float(i)/float(N)*2.0f*PI),
      sin(float(i)/float(N)*2.0f*PI)
    );
    tempsetA[i] = p[i];     //Copy
    tempsetB[i] = vec2(0);  //Empty Set
  }

  // N Circumcenter Iterations, N Segments

  for(int i = 0; i < N; i++){   //Iterate N Times
    for(int k = 0; k < N; k++){ //Over N Intervals
      if(i%2 == 0) tempsetB[k] = circumcenter(M, tempsetA[k], tempsetA[(k+1)%N]);
      else tempsetA[k] = circumcenter(M, tempsetB[k], tempsetB[(k+1)%N]);
    }
  }

  if(viewmode == 0){

    // Binary Scale Discrimination

    float L0, L1;
    L0 = length(p[1] - p[0]);
    L1 = (N%2 == 0)? length(tempsetA[1] - tempsetA[0]):
                length(tempsetB[1] - tempsetB[0]);

    if(L1 / L0 > 1) return diverge;
    else return converge;

  }

  if(viewmode == 1){

    // Rotation HSV Map

    vec2 L0, L1;
    L0 = p[1]-p[0];
    L1 = (N%2 == 0)? (tempsetA[1] - tempsetA[0]):
                (tempsetB[1] - tempsetB[0]);

    float d = abs(dot(L1, L0)/length(L1)/length(L0));
    d = clamp(d, -1, 1);
    return vec4(colorscheme(2.0f*acos(d)/PI, 1), 1);

  }

  if(viewmode == 2){

    // Binary Scale Discrimination

    float L0, L1;
    L0 = length(p[1] - p[0]);
    L1 = (N%2 == 0)? length(tempsetA[1] - tempsetA[0]):
                length(tempsetB[1] - tempsetB[0]);

    //Scale is in [0, inf]

    return mix(converge, diverge, (atan(L0/L1)+PI/2.0));
//    return mix(converge, diverge, abs(log(log(L0 / L1))));
  //  return mix(converge, diverge, exp(-(L0 / L1)));

  }

}

void main() {

  //Ambient Lighting
  const float ambient = 0.5f;
  float shade = ambient + (1.0f - ambient)*dot(lightpos, ex_Normal);

  vec2 ppos = vec2(scale*ex_Normal.x/(1.0f-ex_Normal.y), scale*ex_Normal.z/(1.0f-ex_Normal.y));       //Projected Plane
  vec4 color = planesample(ppos);

  fragColor = vec4(shade*color.xyz, color.w);

}
