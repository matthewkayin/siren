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

uniform vec3 material_ambient;
uniform vec3 material_diffuse;
uniform vec3 material_specular;
uniform float material_shininess;
uniform float material_shininess_strength;
uniform sampler2D material_texture;
uniform sampler2D material_emissive;

void main() {  
    vec3 view_direction = normalize(view_position - frag_position);
    frag_color = vec4(calculate_point_light(point_light, frag_normal, frag_position, view_direction), 1.0);
}

vec3 calculate_point_light(PointLight light, vec3 normal, vec3 frag_position, vec3 view_direction) {
    // Ambient
    vec3 ambient = material_ambient;

    // Diffuse
    vec3 light_direction = normalize(light.position - frag_position);
    float diffuse_strength = max(dot(normal, light_direction), 0.0); 
    vec3 diffuse_color = material_diffuse;
    vec3 diffuse = diffuse_strength * diffuse_color;

    // Specular
    vec3 reflect_direction = reflect(-light_direction, normal);
    vec3 specular = vec3(0.0);
    if (diffuse_strength > 0.0) {
        // specular = pow(max(dot(view_direction, reflect_direction), 0.0), 32.0) * material.ks;
        specular = material_specular * pow(max(dot(view_direction, reflect_direction), 0.0), material_shininess) * material_shininess_strength;
    }

    // Attenuation
    float vertex_distance = length(light.position - frag_position);
    float attenuation = 1.0 / (light.constant + (light.linear * vertex_distance) + (light.quadratic * vertex_distance * vertex_distance));

    // Final color
    vec3 sampled = texture(material_texture, frag_texture_coordinate).rgb;
    vec3 emissive = texture(material_emissive, frag_texture_coordinate).rgb;
    return (ambient + diffuse + specular + emissive) * sampled * attenuation;
}