/*
iterative circumcenter map shader functions
*/

const float EPSILON = 1E-12;
const float PI = 3.14159265f;
const int maxN = 8;
uniform int N;

// Triangle Colinearity

bool colinear( dvec2 M, dvec2 A, dvec2 B ){

  dmat2 Q = mat2(A.x-M.x, B.x-M.x,
                A.y-M.y, B.y-M.y);

  return abs( determinant( Q ) ) < EPSILON;

}

// Triangle Circumcenter

dvec2 circumcenter( dvec2 M, dvec2 A, dvec2 B ){

  dmat2 Q = dmat2(A.x-M.x, B.x-M.x,
                A.y-M.y, B.y-M.y);

  return 0.5 * inverse(Q) * dvec2( dot(A,A) - dot(M,M), dot(B,B) - dot(M,M) );

}

// Stereographic Projection

vec2 stereographic( vec3 p, float k ){

  return k * p.xz / (1.0f - p.y);

}

// Retrieve Scale

float getScale( vec2 M ){

  dvec2 original[maxN];
  dvec2 tempsetA[maxN];
  dvec2 tempsetB[maxN];

  for(int i = 0; i < N; i++){
    original[i] = dvec2(
      cos(float(i)/float(N)*2.0f*PI),
      sin(float(i)/float(N)*2.0f*PI)
    );
    tempsetA[i] = original[i];      //Copy
    tempsetB[i] = dvec2(0);          //Empty Set
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

  double L0, L1;
  L0 = length(original[1] - original[0]);
  L1 = (N%2 == 0)? length(tempsetA[1] - tempsetA[0]):
              length(tempsetB[1] - tempsetB[0]);
  return float(min(L1 / L0, 1/EPSILON));

}


float getRot( vec2 M ){

  dvec2 original[maxN];
  dvec2 tempsetA[maxN];
  dvec2 tempsetB[maxN];

  for(int i = 0; i < N; i++){
    original[i] = dvec2(
      cos(float(i)/float(N)*2.0f*PI),
      sin(float(i)/float(N)*2.0f*PI)
    );
    tempsetA[i] = original[i];      //Copy
    tempsetB[i] = dvec2(0);          //Empty Set
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

  dvec2 L0, L1;
  L0 = normalize(original[1]-original[0]);
  L1 = (N%2 == 0)? normalize(tempsetA[1] - tempsetA[0]):
              normalize(tempsetB[1] - tempsetB[0]);

  float d = float(clamp(abs(dot(L1, L0)), -1, 1));
  return 2.0f*acos(d)/PI;

}
