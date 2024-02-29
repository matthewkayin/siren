#version 410 core

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;
};

vec3 calculate_point_light(PointLight light, vec3 normal, vec3 frag_position, vec3 view_direction);

in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_texture_coordinate;

out vec4 frag_color;

uniform vec3 view_position;
uniform PointLight point_light;
uniform sampler2D u_texture;

void main() {  
    vec3 view_direction = normalize(view_position - frag_position);
    frag_color = vec4(calculate_point_light(point_light, frag_normal, frag_position, view_direction), 1.0);
}

vec3 calculate_point_light(PointLight light, vec3 normal, vec3 frag_position, vec3 view_direction) {
    vec3 sampled = vec3(texture(u_texture, frag_texture_coordinate));
    // vec3 ambient = material.ka * vec3(texture(material.map_ka, texture_coordinate));
    // vec3 ambient = vec3(0.2, 0.0, 0.0);
    vec3 ambient = 0.2 * sampled;

    vec3 light_direction = normalize(light.position - frag_position);
    float diffuse_strength = max(dot(normal, light_direction), 0.0); 
    // vec3 diffuse_color = material.kd * vec3(texture(material.map_kd, texture_coordinate));
    vec3 diffuse_color = sampled;
    vec3 diffuse = diffuse_strength * diffuse_color;

    vec3 reflect_direction = reflect(-light_direction, normal);
    vec3 specular = vec3(0.0);
    if (diffuse_strength > 0.0) {
        // specular = pow(max(dot(view_direction, reflect_direction), 0.0), 32.0) * material.ks;
        specular = pow(max(dot(view_direction, reflect_direction), 0.0), 32.0) * sampled;
    }

    float vertex_distance = length(light.position - frag_position);
    float attenuation = 1.0 / (light.constant + (light.linear * vertex_distance) + (light.quadratic * vertex_distance * vertex_distance));

    return (ambient + diffuse + specular) * attenuation;
}