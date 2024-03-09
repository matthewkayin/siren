#include "math_test.h"

#include "core/logger.h"

#include "math/matrix.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void log_matrices(siren::mat4 siren_mat4, glm::mat4 glm_mat4) {
    char buffer[1024];
    char* bpointer = buffer;
    bpointer += sprintf(bpointer, "Expected: \n");
    for (uint32_t row = 0; row < 4; row++) {
        bpointer += sprintf(bpointer, "[%f, %f, %f, %f]\n", glm_mat4[0][row], glm_mat4[1][row], glm_mat4[2][row], glm_mat4[3][row]);
    }
    bpointer += sprintf(bpointer, "Received: \n");
    for (uint32_t row = 0; row < 4; row++) {
        bpointer += sprintf(bpointer, "[%f, %f, %f, %f]\n", siren_mat4[0][row], siren_mat4[1][row], siren_mat4[2][row], siren_mat4[3][row]);
    }
    SIREN_LOG_ERROR("\n%s", buffer);
}

void math_test_evaluate(siren::mat4 siren_mat4, glm::mat4 glm_mat4, const char* test_name) {
    for (uint32_t col = 0; col < 4; col++) {
        for (uint32_t row = 0; row < 4; row++) {
            if (siren_mat4[col][row] != glm_mat4[col][row]) {
                SIREN_LOG_INFO("MATH TEST %s... Failed!", test_name);
                log_matrices(siren_mat4, glm_mat4);
                return;
            }
        }
    }

    SIREN_LOG_INFO("MATH TEST %s... Passed!", test_name);
}

void math_test_evaluate(bool condition, const char* test_name) {
    if (condition) {
        SIREN_LOG_INFO("MATH TEST %s... Passed!", test_name);
    } else {
        SIREN_LOG_ERROR("MATH TEST %s... Failed!", test_name);
    }
}

void math_test_identity() {
    siren::mat4 a = siren::mat4(1.0f);
    glm::mat4 b = glm::mat4(1.0f);
    math_test_evaluate(a, b, "Matrix identity");
}

void math_test_multiply() {
    siren::mat4 sa;
    siren::mat4 sb;
    siren::mat4 sc;
    glm::mat4 ga;
    glm::mat4 gb;
    glm::mat4 gc;
    for (uint32_t col = 0; col < 4; col++) {
        for (uint32_t row = 0; row < 4; row++) {
            uint32_t index = row + (col * 4);
            sa[col][row] = index + 1;
            sb[col][row] = index + 16;
            ga[col][row] = index + 1;
            gb[col][row] = index + 16;
        }
    }

    sc = sa * sb;
    gc = ga * gb;

    math_test_evaluate(sc, gc, "Matrix multiply");
}

void math_test_ortho() {
    siren::mat4 sortho = siren::mat4::orthographic(0.0f, 1280.0f, 0.0f, 720.0f, -1.0f, 1.0f);
    glm::mat4 gortho = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f, -1.0f, 1.0f);

    math_test_evaluate(sortho, gortho, "Matrix orthographic");
}

void math_test_perspective() {
    float fov = siren::deg_to_rad(45.0f);
    siren::mat4 s = siren::mat4::perspective(fov, 1280.0f / 720.0f, 0.0f, 100.0f);
    glm::mat4 g = glm::perspective(fov, 1280.0f / 720.0f, 0.0f, 100.0f);
    math_test_evaluate(fov == glm::radians(45.0f), "Convert to radians");
    math_test_evaluate(s, g, "Matrix perspective");
}

void math_test_lookat() {
    siren::mat4 s = siren::mat4::look_at(siren::vec3(1.0f, 2.0f, 3.0f), siren::vec3(1.0, 2.0f, 3.0f) + siren::vec3(0.0f, 0.0f, -1.0f), siren::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 g = glm::lookAt(glm::vec3(1.0f, 2.0f, 3.0f), glm::vec3(1.0f, 2.0f, 3.0f) + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    math_test_evaluate(s, g, "Matrix look at");
}

void run_math_tests() {
    math_test_identity();
    math_test_multiply();
    math_test_ortho();
    math_test_perspective();
    math_test_lookat();
}