#version 410 core

in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_texture_coordinate;

out vec4 frag_color;

uniform vec3 view_position;

uniform vec3 light_positions[4];
uniform vec3 light_colors[4];
uniform int light_count;

uniform sampler2D material_albedo;
uniform sampler2D material_metallic_roughness;
uniform sampler2D material_normal;
uniform sampler2D material_emissive;
uniform sampler2D material_occlusion;

const float PI = 3.14159265359;

float distribution_ggx(vec3 normal, vec3 halfway, float roughness);
float geometry_schlick_ggx(float n_dot_v, float roughness);
float geometry_smith(vec3 normal, vec3 view_direction, vec3 light_direction, float roughness);
vec3 fresnel_schlick(float cos_theta, vec3 base_reflectivity);
vec3 fresnel_schlick_roughness(float cos_theta, vec3 base_reflectivity, float roughness);

void main() {  
    vec3 view_direction = normalize(view_position - frag_position);
    vec3 normal = normalize(frag_normal);
    vec3 reflected = reflect(-view_direction, normal);

    // Albedo
    vec3 albedo = pow(texture(material_albedo, frag_texture_coordinate).rgb, vec3(2.2));

    // Metallic / Roughness
    vec4 metallic_roughness_sample = texture(material_metallic_roughness, frag_texture_coordinate);
    float metallic = metallic_roughness_sample.b;
    float roughness = metallic_roughness_sample.g;

    // Normal
    vec3 tangent_normal = texture(material_normal, frag_texture_coordinate).xyz * 2.0 - 1.0;
    vec3 q1 = dFdx(frag_position);
    vec3 q2 = dFdy(frag_position);
    vec2 st1 = dFdx(frag_texture_coordinate);
    vec2 st2 = dFdy(frag_texture_coordinate);
    vec3 tangent = normalize(q1 * st2.t - q2 * st1.t);
    vec3 bitangent = -normalize(cross(normal, tangent));
    normal = normalize(mat3(tangent, bitangent, normal) * tangent_normal);

    // Emissive
    vec3 emissive = pow(texture(material_emissive, frag_texture_coordinate).xyz, vec3(2.2));

    // Ambient Occlusion
    float ambient_occlusion = texture(material_occlusion, frag_texture_coordinate).r;

    vec3 base_reflectivity = mix(vec3(0.04), albedo, metallic);
    vec3 light_out = vec3(0.0);
    for (int light_index = 0; light_index < light_count; light_index++) {
        // Calculate per-light radiance
        vec3 light_direction = normalize(light_positions[light_index] - frag_position);
        vec3 halfway = normalize(view_direction + light_direction);
        float light_distance = length(light_positions[light_index] - frag_position);
        float attenuation = 1.0 / (light_distance * light_distance);
        vec3 radiance = light_colors[light_index] * attenuation;

        // Cook-Torrance BRDF
        float NDF = distribution_ggx(normal, halfway, roughness);
		float G = geometry_smith(normal, view_direction, light_direction, roughness);
		vec3 light_reflected = fresnel_schlick(clamp(dot(halfway, view_direction), 0.0, 1.0), base_reflectivity);
		// the refracted light is any light that wasn't reflected
		// we also multiply by 1 - metallic in order to reinforce the property that metallic surfaces don't have refractions
		vec3 light_refracted = (vec3(1.0) - light_reflected) * (1.0 - metallic);

		vec3 numerator = NDF * G * light_reflected;
		// adding 0.0001 to this prevents division by 0 in the case that the dot products are 0
		float denominator = 4.0 * max(dot(normal, view_direction), 0.0) * max(dot(normal, light_direction), 0.0) + 0.0001;
		vec3 specular = numerator / denominator;

		float n_dot_l = max(dot(normal, light_direction), 0.0);
		light_out += (light_refracted * albedo / PI + specular) * radiance * n_dot_l;
    }

    vec3 ambient = vec3(0.03) * albedo * ambient_occlusion;
    vec3 color = ambient + light_out + emissive;
    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0 / 2.2));

    frag_color = vec4(color, 1.0);
}

float distribution_ggx(vec3 normal, vec3 halfway, float roughness) {
	float a = roughness * roughness;
	float a_squared = a * a;
	float n_dot_h = max(dot(normal, halfway), 0.0);
	float n_dot_h_squared = n_dot_h * n_dot_h;

	float denominator = (n_dot_h_squared * (a_squared - 1.0) + 1.0);
	denominator = PI * denominator * denominator;

	return a_squared / denominator;
}

float geometry_schlick_ggx(float n_dot_v, float roughness) {
	float r = roughness + 1.0;
	float k = (r * r) / 8.0;

	float denominator = n_dot_v * (1.0 - k) + k;
	return n_dot_v / denominator;
}

float geometry_smith(vec3 normal, vec3 view_direction, vec3 light_direction, float roughness) {
	float n_dot_v = max(dot(normal, view_direction), 0.0);
	float n_dot_l = max(dot(normal, light_direction), 0.0);
	float ggx2 = geometry_schlick_ggx(n_dot_v, roughness);
	float ggx1 = geometry_schlick_ggx(n_dot_l, roughness);

	return ggx1 * ggx2;
}

vec3 fresnel_schlick(float cos_theta, vec3 base_reflectivity) {
	return base_reflectivity + (1.0 - base_reflectivity) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}

vec3 fresnel_schlick_roughness(float cos_theta, vec3 base_reflectivity, float roughness) {
	return base_reflectivity + (max(vec3(1.0 - roughness), base_reflectivity) - base_reflectivity) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}