#include <TinyEngine/TinyEngine>
#include <TinyEngine/color>

#include "model.h"

int main( int argc, char* args[] ) {
	srand(time(NULL));  //SEED

	Tiny::view.vsync = true;

	Tiny::window("Circumcenter Iteration Map Sweephull", 800, 800);

	Tiny::event.handler  = eventHandler;
	Tiny::view.interface = interfaceFunc;

	Shader sdf({"shader/sdf.vs", "shader/sdf.fs"}, {"in_Quad", "in_Tex"});
	Square2D flat;												//Flat geometry primitive

	Tiny::view.pipeline = [&](){

		Tiny::view.target(color::black);	//Target Screen

		sdf.use();
		sdf.uniform("camerapos", camerapos);
		sdf.uniform("lookpos", lookpos);
		sdf.uniform("zoom", zoom);
		sdf.uniform("shift", shift);
		sdf.uniform("invert", invert);
		sdf.uniform("converge", glm::vec4(cc[0], cc[1], cc[2], cc[3]));
		sdf.uniform("diverge", glm::vec4(dc[0], dc[1], dc[2], dc[3]));
		sdf.uniform("model", flat.model);
		flat.render();

	};

	Tiny::loop([&](){

		if(active){
			shift += dshift;
			if(shift > 20) dshift *= -1.0;
			if(shift < -10) dshift *= -1.0;
		}

	});

	Tiny::quit();

	return 0;
}
