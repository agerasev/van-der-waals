#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "gl.h"

#include "cl/session.hpp"
#include "cl/program.hpp"
#include "cl/gl_buffer_object.hpp"


int main(int argc, char *argv[])
{
	SDL_Window *sdl_window = SDL_CreateWindow(
			"OpenCL",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			600, 600,
			SDL_WINDOW_OPENGL// | SDL_WINDOW_RESIZABLE
		);
	
	if(sdl_window == NULL)
	{
		fprintf(stderr,"Could not create SDL_Window\n");
		return -1;
	}
	
	SDL_GLContext sdl_glcontext = SDL_GL_CreateContext(sdl_window);
	
	if(sdl_glcontext == NULL)
	{
		fprintf(stderr,"Could not create SDL_GL_Context\n");
		return -2;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,6);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,5);
	SDL_GL_SetSwapInterval(1);

	GLenum glew_status = glewInit();
	if(GLEW_OK != glew_status)
	{
		fprintf(stderr,"Could not init glew: %s\n",glewGetErrorString(glew_status));
		return -3;
	}
	if(!GLEW_VERSION_2_0)
	{
		fprintf(stderr,"No support for OpenGL 2.0 found\n");
		return -4;
	}
	
	static const int BUFFER_SIZE = 2*3*3;
	
	initGL();
	
	cl::session *session = new cl::session;
	cl::program *program = new cl::program("cl/kernel.cl",session->get_context().get_cl_context(),session->get_device_id());
	cl::gl_buffer_object *buffer_src = new cl::gl_buffer_object(session->get_context(),BUFFER_SIZE*sizeof(float));
	cl::gl_buffer_object *buffer_dst = new cl::gl_buffer_object(session->get_context(),BUFFER_SIZE*sizeof(float));
	
	program->bind_queue(session->get_queue().get_cl_command_queue());
	buffer_src->bind_queue(session->get_queue().get_cl_command_queue());
	buffer_dst->bind_queue(session->get_queue().get_cl_command_queue());

	float buffer_data[BUFFER_SIZE] = {
		0.0f,0.0f,0.0f, 1.0f,-1.0f,0.0f,
		1.0f,0.0f,0.0f, 0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f, -1.0f,0.0f,0.0f
	};
	buffer_src->store_data(buffer_data);
	session->get_queue().flush();
	
	cl::work_range *range = new cl::work_range({3});
	
	bool done = false;
	while(!done)
	{
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_QUIT:
				done = true;
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_ESCAPE)
					done = true;
				break;
			}
		}
		
		program->get_kernel("interact")->evaluate(*range,buffer_src,buffer_dst,int(3));
		program->get_kernel("interact")->print_time();
		drawGLBuffer(buffer_src->get_gl_buffer(),3,2*3*sizeof(float));
		
		cl::gl_buffer_object *tmp = buffer_src;
		buffer_src = buffer_dst;
		buffer_dst = tmp;
		
		SDL_GL_SwapWindow(sdl_window);
	}
	
	delete buffer_src;
	delete buffer_dst;
	delete program;
	delete session;
	
	disposeGL();
	
	SDL_GL_DeleteContext(sdl_glcontext);
	SDL_DestroyWindow(sdl_window);
	
	return 0;
}
