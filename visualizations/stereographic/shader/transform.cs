/*
iterative circumcenter map shader functions
*/

const float EPSILON = 1E-10;
const float PI = 3.14159265f;
const int maxN = 8;
uniform int N;

// Triangle Colinearity

bool colinear( vec2 M, vec2 A, vec2 B ){

  mat2 Q = mat2(A.x-M.x, B.x-M.x,
                A.y-M.y, B.y-M.y);

  return abs( determinant( Q ) ) < EPSILON;

}

// Triangle Circumcenter

vec2 circumcenter( vec2 M, vec2 A, vec2 B ){

  mat2 Q = mat2(A.x-M.x, B.x-M.x,
                A.y-M.y, B.y-M.y);

  return 0.5 * inverse(Q) * vec2( dot(A,A) - dot(M,M), dot(B,B) - dot(M,M) );

}

// Stereographic Projection

vec2 stereographic( vec3 p, float k ){

  return min(k * p.xz / (1.0f - p.y), 1/EPSILON);

}

// Retrieve Scale

float getScale( vec2 M ){

  vec2 original[maxN];
  vec2 tempsetA[maxN];
  vec2 tempsetB[maxN];

  for(int i = 0; i < N; i++){
    original[i] = vec2(
      cos(float(i)/float(N)*2.0f*PI),
      sin(float(i)/float(N)*2.0f*PI)
    );
    tempsetA[i] = original[i];      //Copy
    tempsetB[i] = vec2(0);          //Empty Set
  }

  for(int i = 0; i < N; i++){       //Iterate N Times

    for(int k = 0; k < N; k++){     //Iterate N Times
      if(i%2 == 0 && colinear(M, tempsetA[k], tempsetA[(k+1)%N])) return 1.0f;
      if(i%2 == 1 && colinear(M, tempsetB[k], tempsetB[(k+1)%N])) return 1.0f;
    }

    for(int k = 0; k < N; k++){
      if(i%2 == 0) tempsetB[k]  = circumcenter(M, tempsetA[k], tempsetA[(k+1)%N]);
      else tempsetA[k]          = circumcenter(M, tempsetB[k], tempsetB[(k+1)%N]);
    }

  }

  float L0, L1;
  L0 = length(original[1] - original[0]);
  L1 = (N%2 == 0)? length(tempsetA[1] - tempsetA[0]):
              length(tempsetB[1] - tempsetB[0]);
  return float(min(L1 / L0, 1/EPSILON));

}


float getRot( vec2 M ){

  vec2 original[maxN];
  vec2 tempsetA[maxN];
  vec2 tempsetB[maxN];

  for(int i = 0; i < N; i++){
    original[i] = vec2(
      cos(float(i)/float(N)*2.0f*PI),
      sin(float(i)/float(N)*2.0f*PI)
    );
    tempsetA[i] = original[i];      //Copy
    tempsetB[i] = vec2(0);          //Empty Set
  }

  for(int i = 0; i < N; i++){       //Iterate N Times

    for(int k = 0; k < N; k++){     //Iterate N Times
      if(i%2 == 0 && colinear(M, tempsetA[k], tempsetA[(k+1)%N])) return 1.0f;
      if(i%2 == 1 && colinear(M, tempsetB[k], tempsetB[(k+1)%N])) return 1.0f;
    }

    for(int k = 0; k < N; k++){
      if(i%2 == 0) tempsetB[k]  = circumcenter(M, tempsetA[k], tempsetA[(k+1)%N]);
      else tempsetA[k]          = circumcenter(M, tempsetB[k], tempsetB[(k+1)%N]);
    }

  }

  vec2 L0, L1;
  L0 = normalize(original[1]-original[0]);
  L1 = (N%2 == 0)? normalize(tempsetA[1] - tempsetA[0]):
              normalize(tempsetB[1] - tempsetB[0]);

  float d = float(clamp(abs(dot(L1, L0)), -1, 1));
  return 2.0f*acos(d)/PI;

}
