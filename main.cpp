#include <TinyEngine/TinyEngine>

#include "model.h"
#include <iostream>

using namespace glm;
using namespace std;

int main( int argc, char* args[] ) {
	srand(time(NULL));  //SEED

	Tiny::view.vsync = true;
	Tiny::window("Triangle Fractal Explorer", 1000, 1000);	//Open Window

	Tiny::event.handler  = eventHandler;	//Set Event Handler
	Tiny::view.interface = interfaceFunc;	//Set Interface Function

	Square2D flat;												//Flat geometry primitive
	Shader triangle({"shader/triangle.vs", "shader/triangle.fs"}, {"in_Quad", "in_Tex"}, {"pointset"});
	Shader lines({"shader/lines.vs", "shader/lines.fs"}, {"in_Position", "in_Normal", "in_Color"});

	//Set Up the Point-Set
	for(int i = 0; i < N; i++){
		pointset.push_back(glm::vec2(cos(2.0f*PI*(float)i/(float)N), sin(2.0f*PI*(float)i/(float)N)));
	}
	triangle.buffer("pointset", pointset);

	computeTriangles();

	Model model(construct_strip, triangleset);
	model.indexed = false;

	auto start = std::chrono::high_resolution_clock::now();
	auto stop = std::chrono::high_resolution_clock::now();

	Tiny::view.pipeline = [&](){

		stop = std::chrono::high_resolution_clock::now();
		frametime = ((float)std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count())/1000.0f;
		start = std::chrono::high_resolution_clock::now();

		Tiny::view.target(glm::vec3(0));		//Target Screen

		triangle.use();												//Prepare Shader

		triangle.uniform("zoom", zoom);

		triangle.uniform("viewpoints", viewpoints);
		triangle.uniform("viewanchor", viewanchor);
		triangle.uniform("threshold", threshold);

		triangle.uniform("viewtype", viewtype);
		triangle.uniform("viewcolor", viewcolor);

		triangle.uniform("center", center);
		triangle.uniform("anchor", anchor);

		triangle.uniform("N", N);
		flat.render();											//Draw onto Flat Geometry

		if(viewiteration){
			lines.use();
			lines.uniform("zoom", zoom);
			lines.uniform("center", center);
			lines.uniform("anchor", anchor);
			lines.uniform("color", glm::vec3(1,0,0));
			model.render(GL_LINES);
		}

	};

	Tiny::loop([&](){											//Extra Stuff (every tick)
		if(update){
			triangle.buffer("pointset", pointset);
			model.construct(construct_strip, triangleset);
			update = false;
		}
	});

	Tiny::quit();

	return 0;
}
