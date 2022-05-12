/*
Iterative Circumcenter Map
Fragment Shader
Author: Nicholas McDonald, 2021-2022
*/

#version 430 core

flat in vec3 lightpos;
in vec3 ex_Normal;

out vec4 fragColor;

uniform float scale;
uniform float logscale;

uniform vec4 diverge;
uniform vec4 converge;
uniform int viewmode;

uniform float SK;
uniform float SM;
uniform float GM;

#include transform.cs

/*
Compute Convergence vs. Divergence
*/

vec3 hsv2rgb(vec3 c){
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 colorscheme(float t, float v){
  return hsv2rgb(vec3(t, 1, v));
}

void main() {

  vec2 ppos = stereographic(ex_Normal, scale);

  if(viewmode == 0){
    float S = getScale( ppos );

    fragColor =  (S > 1) ? diverge : converge;
  }

  if(viewmode == 1){
    fragColor = vec4(colorscheme(getRot( ppos ), 1), 1);
  }

  if(viewmode == 2){
    float S = getScale( ppos );
    if(abs(S-1) < 5E-2){
      fragColor = vec4(1);
    }
    else if(S < 1){
      fragColor = vec4(0, (1.0/S-1.0f)/GM,0,1);
    }
    else {
      S = log(1+SK*log(S));
      if(S > SM) S = SM;
      fragColor = vec4(S/SM,0,0,1);
    }


  }

  if(fragColor.w < 1.0)
    discard;

  const float ambient = 0.8f;
  float shade = ambient + (1.0f - ambient)*dot(lightpos, ex_Normal);
  fragColor.xyz *= shade;

}
