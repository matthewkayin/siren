#include "renderer.h"

#include "core/logger.h"
#include "math/math.h"
#include "renderer/shader.h"

#include <SDL2/SDL.h>
#include <glad/glad.h>

struct RendererState {
    SDL_Window* window;
    SDL_GLContext context;

    siren::ivec2 screen_size;
    siren::ivec2 window_size;

    GLuint screen_framebuffer;
    GLuint screen_texture;
    GLuint screen_intermediate_framebuffer;
    GLuint screen_intermediate_texture;

    GLuint quad_vao;

    Shader screen_shader;
};
static RendererState state;
static bool initialized = false;

bool siren::renderer_init(RendererConfig config) {
    if (initialized) {
        return false;
    }

    // Set GL version
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_LoadLibrary(NULL);

    // Create window
    state.window = SDL_CreateWindow(config.window_name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, config.window_size.x, config.window_size.y, SDL_WINDOW_OPENGL);
    if (state.window == NULL) {
        SIREN_FATAL("Error creating window: %s", SDL_GetError());
        return false;
    }
    state.screen_size = config.screen_size;
    state.window_size = config.window_size;

    // Create GL context
    state.context = SDL_GL_CreateContext(state.window);
    if (state.context == NULL) {
        SIREN_FATAL("Error creating GL context: %s", SDL_GetError());
        return false;
    }

    // Setup GLAD
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    if (glGenVertexArrays == NULL) {
        SIREN_FATAL("Error loading OpenGL.");
        return false;
    }

    // Setup quad VAO
    float quad_vertices[] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
	GLuint quad_vbo;

	glGenVertexArrays(1, &state.quad_vao);
	glGenBuffers(1, &quad_vbo);
	glBindVertexArray(state.quad_vao);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);

    // Setup framebuffer
    glGenFramebuffers(1, &state.screen_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, state.screen_framebuffer);

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &state.screen_texture);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, state.screen_texture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, state.screen_size.x, state.screen_size.y, GL_TRUE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, state.screen_texture, 0);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, state.screen_size.x, state.screen_size.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        SIREN_FATAL("Screen framebuffer not complete");
		return false;
	}
	
	glGenFramebuffers(1, &state.screen_intermediate_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, state.screen_intermediate_framebuffer);
	glGenTextures(1, &state.screen_intermediate_texture);
	glBindTexture(GL_TEXTURE_2D, state.screen_intermediate_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, state.screen_size.x, state.screen_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, state.screen_intermediate_texture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        SIREN_FATAL("Screen intermediate framebuffer not complete");
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Load shaders
    if (!shader_load(&state.screen_shader, "../res/shader/screen.vert.glsl", "../res/shader/screen.frag.glsl")) {
        return false;
    }
    shader_use(state.screen_shader);
    shader_set_uniform(state.screen_shader, "screen_size", state.screen_size);
    shader_set_uniform(state.screen_shader, "screen_texture", 0);

    SIREN_INFO("Renderer subsystem initialized: %s", glGetString(GL_VERSION));

    initialized = true;

    return true;
}

void siren::renderer_quit() {
    if (!initialized) {
        return;
    }

    SDL_GL_DeleteContext(state.context);
    SDL_DestroyWindow(state.window);

    initialized = false;
}

void siren::renderer_prepare_frame() {
    glBindFramebuffer(GL_FRAMEBUFFER, state.screen_framebuffer);
    glViewport(0, 0, state.screen_size.x, state.screen_size.y);
    glBlendFunc(GL_ONE, GL_ZERO);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void siren::renderer_present_frame() {
    // Blit multisample buffer to intermediate buffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, state.screen_framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, state.screen_intermediate_framebuffer);
    glBlitFramebuffer(0, 0, state.screen_size.x, state.screen_size.y, 0, 0, state.screen_size.x, state.screen_size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // Render framebuffer to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, state.window_size.x, state.window_size.y);
    glBlendFunc(GL_ONE, GL_ZERO);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader_use(state.screen_shader);
    glBindVertexArray(state.quad_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, state.screen_intermediate_texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    SDL_GL_SwapWindow(state.window);
}