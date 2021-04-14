#include <TinyEngine/TinyEngine>
#include <TinyEngine/image>

#include "model.h"
#include <iostream>

using namespace glm;
using namespace std;

int main( int argc, char* args[] ) {

	if(argc == 3){
		SIZEX = std::stoi(args[1]);
		SIZEY = std::stoi(args[2]);
	}

	Tiny::view.lineWidth = 2.0;
	Tiny::view.pointSize = 8.0;
	Tiny::view.vsync = true;
	Tiny::window("Iterative Circumcenter Map (c) 2021 N. McDonald, D. Reznik, R. Garcia", SIZEX, SIZEY);	//Open Window

	Tiny::event.handler  = eventHandler;	//Set Event Handler
	Tiny::view.interface = interfaceFunc;	//Set Interface Function

	std::vector<glm::vec2> intersections;

	Square2D flat;												//Flat geometry primitive
	Shader billboard({"shader/billboard.vs", "shader/billboard.fs"}, {"in_Quad", "in_Tex"});
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

	Model intersectionmodel(construct_points, intersections);
	intersectionmodel.indexed = false;

	auto start = std::chrono::high_resolution_clock::now();
	auto stop = std::chrono::high_resolution_clock::now();

	Billboard scaleimage(Tiny::view.WIDTH, Tiny::view.HEIGHT);
	Billboard rotationimage(Tiny::view.WIDTH, Tiny::view.HEIGHT);
	Billboard fullimage(Tiny::view.WIDTH, Tiny::view.HEIGHT);

	Tiny::view.pipeline = [&](){

		//Get the Frame-Time

		stop = std::chrono::high_resolution_clock::now();
		frametime = ((float)std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count())/1000.0f;
		start = std::chrono::high_resolution_clock::now();

		//Get the Scale Image (for Edge Detection)

		triangle.use();												//Prepare Shader
		triangle.uniform("zoom", zoom);

		if(isolines && (viewtype == 0 || viewtype == 2)){

			scaleimage.target(vec3(0));

			triangle.uniform("viewpoints", false);
			triangle.uniform("viewanchor", false);
			triangle.uniform("threshold", threshold);

			triangle.uniform("viewtype", 0);
			triangle.uniform("viewcolor", 0);

			triangle.uniform("center", center);
			triangle.uniform("anchor", anchor);
			triangle.uniform("pointsize", pointsize);

			triangle.uniform("applylog", false);
			triangle.uniform("logscale", logscale);

			triangle.uniform("stretch", stretch);
			triangle.uniform("skew", skew);

			triangle.uniform("N", N);
			flat.render();											//Draw onto Flat Geometry

		}

		if(isolines && (viewtype == 1 || viewtype == 2)){

			rotationimage.target(vec3(0));

			triangle.uniform("viewpoints", false);
			triangle.uniform("viewanchor", false);
			triangle.uniform("threshold", threshold);

			triangle.uniform("viewtype", 1);
			triangle.uniform("viewcolor", 0);

			triangle.uniform("center", center);
			triangle.uniform("anchor", anchor);
			triangle.uniform("pointsize", pointsize);

			triangle.uniform("applylog", false);
			triangle.uniform("logscale", logscale);

			triangle.uniform("stretch", stretch);
			triangle.uniform("skew", skew);

			triangle.uniform("N", N);
			flat.render();											//Draw onto Flat Geometry

		}

		fullimage.target(vec3(0));

		triangle.uniform("viewpoints", viewpoints);
		triangle.uniform("viewanchor", viewanchor);
		triangle.uniform("threshold", threshold);

		triangle.uniform("viewtype", viewtype);
		triangle.uniform("viewcolor", viewcolor);

		triangle.uniform("applylog", applylog);
		triangle.uniform("cutoff", cutoff);
		triangle.uniform("logscale", logscale);

		triangle.uniform("center", center);
		triangle.uniform("anchor", anchor);
		triangle.uniform("pointsize", pointsize);

		triangle.uniform("stretch", stretch);
		triangle.uniform("skew", skew);

		triangle.uniform("N", N);
		flat.render();											//Draw onto Flat Geometry




		Tiny::view.target(glm::vec3(0));		//Target Screen

		billboard.use();

		billboard.uniform("viewtype", viewtype);
		billboard.uniform("viewcolor", viewcolor);
		billboard.uniform("isolines", isolines);
		billboard.texture("scaletexture", scaleimage.texture);
		billboard.texture("rotationtexture", rotationimage.texture);
		billboard.texture("fulltexture", fullimage.texture);
		flat.render();

		if(viewiteration){

			lines.use();
			lines.uniform("zoom", zoom);
			lines.uniform("center", center);
			lines.uniform("anchor", anchor);
			lines.uniform("color", glm::vec3(1,0,0));

			lines.uniform("stretch", stretch);
			lines.uniform("skew", skew);

			model.render(GL_LINES);

			if(viewpoints){
				model.render(GL_POINTS);
			}
		}

		if(viewtype == 2 && viewcolor == 0 && isolines){

			//Find all the Intersection Points!
			int* s = new int[Tiny::view.WIDTH*Tiny::view.HEIGHT];
			intersections.clear();
			glBindFramebuffer(GL_FRAMEBUFFER, 0); glReadBuffer(GL_COLOR_BUFFER_BIT);
			glReadPixels(0, 0, Tiny::view.WIDTH, Tiny::view.HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, s);
			for(int i = 0; i < Tiny::view.WIDTH*Tiny::view.HEIGHT; i++){
				if( ((s[i] >>  0) & 0xff) == 0 &&
						((s[i] >>  8) & 0xff) == 0 &&
						((s[i] >> 16) & 0xff) == 0){
							glm::vec2 ip = glm::vec2(i%Tiny::view.HEIGHT, i/Tiny::view.HEIGHT);
							ip = ((2.0f * ip / glm::vec2(SIZEX, SIZEY) - glm::vec2(1.0)) + center)/float(zoom);
							intersections.push_back(ip);
						}
			}
			delete s;

			intersectionmodel.construct(construct_points, intersections);

			lines.use();
			lines.uniform("zoom", zoom);
			lines.uniform("center", center);
			lines.uniform("anchor", anchor);
			lines.uniform("color", glm::vec3(1,0,0));

			lines.uniform("stretch", stretch);
			lines.uniform("skew", skew);

			intersectionmodel.render(GL_POINTS);

		}

		// Draw the Image with Effects and Annotations

/*
		Tiny::view.target(glm::vec3(0));		//Target Screen

*/


/*

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
		triangle.uniform("pointsize", pointsize);

		triangle.uniform("stretch", stretch);
		triangle.uniform("skew", skew);

		triangle.uniform("N", N);
		flat.render();											//Draw onto Flat Geometry



*/

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
