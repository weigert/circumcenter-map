#version 430 core

in vec2 ex_Tex;

uniform sampler2D scaletexture;
uniform sampler2D rotationtexture;
uniform sampler2D fulltexture;

uniform int viewtype;
uniform int viewcolor;
uniform bool isolines;

out vec4 fragColor;

float edgedetect(sampler2D tex){

  int foundedge = 0;
  vec4 a0 = textureOffset(tex, ex_Tex, ivec2( 0, 0));

  vec4 a1 = textureOffset(tex, ex_Tex, ivec2(-1, 0));
  vec4 a2 = textureOffset(tex, ex_Tex, ivec2( 1, 0));
  vec4 a3 = textureOffset(tex, ex_Tex, ivec2( 0,-1));
  vec4 a4 = textureOffset(tex, ex_Tex, ivec2( 0, 1));

  if(a1 != a0) foundedge++;
  if(a2 != a0) foundedge++;
  if(a3 != a0) foundedge++;
  if(a4 != a0) foundedge++;

/*
  vec4 a5 = textureOffset(tex, ex_Tex, ivec2(-1,-1));
  vec4 a6 = textureOffset(tex, ex_Tex, ivec2( 1,-1));
  vec4 a7 = textureOffset(tex, ex_Tex, ivec2(-1, 1));
  vec4 a8 = textureOffset(tex, ex_Tex, ivec2( 1, 1));

  if(a5 != a0) foundedge++;
  if(a6 != a0) foundedge++;
  if(a7 != a0) foundedge++;
  if(a8 != a0) foundedge++;
*/

//  vec4 a0 = texture(scaletexture, ex_Tex + ivec2(0, 0));
  return float(foundedge);
}

void main(){

  fragColor =  texture(fulltexture, ex_Tex);

  vec3 linecolorA = vec3(0);
  vec3 linecolorB = vec3(0);
  if(viewcolor == 1) linecolorA = vec3(1);
  if(viewcolor == 1) linecolorB = vec3(1);

  if(viewtype == 2){
    linecolorA = vec3(0.8,0,0);
    linecolorB = vec3(0,0.8,0);
  }


  if(isolines && viewtype == 2){
    float edgestrengthA = edgedetect(scaletexture);
    float edgestrengthB = edgedetect(rotationtexture);
    fragColor = mix(fragColor, vec4(linecolorA, 1.0), edgestrengthA);
    fragColor = mix(fragColor, vec4(linecolorB, 1.0), edgestrengthB);
    if(edgestrengthA > 0 && edgestrengthB > 0){
      fragColor = vec4(0,0,0,1);
      //Found an Intersection!
  //    atomicAdd(intset[0].x, 1);
  //    atomicAdd(, 100);
  //    atomicAdd(intset[intset[0].x].y, 100);
    }

  //  if()
  }

  if(isolines && viewtype == 0){
    float edgestrength = edgedetect(scaletexture);
    fragColor = mix(fragColor, vec4(linecolorA, 1.0), edgestrength);
  }
  if(isolines && viewtype == 1){
    float edgestrength = edgedetect(rotationtexture);
    fragColor = mix(fragColor, vec4(linecolorB, 1.0), edgestrength);
  }

}
