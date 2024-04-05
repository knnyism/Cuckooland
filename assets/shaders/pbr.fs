#version 330 core

in vec2 tex_coords;
in vec3 vert_pos;
in vec3 vert_norm;

out vec4 FragColor;

// material parameters
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

#define LIGHT_POINT 1
#define LIGHT_SPOT 2
#define LIGHT_SUN 3

struct Light {
    vec3 pos;
    vec3 color;
    vec3 target;
    float intensity;
    int type;
    int on;
};

// lights
// Todo: make this array dynamic?
uniform Light lights[100];

uniform vec3 camPos;
uniform float fogDensity;

const float PI = 3.14159265359;

vec3 GetNormalFromMap() {
    vec3 tangentNormal = texture(normalMap, tex_coords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(vert_pos);
    vec3 Q2  = dFdy(vert_pos);
    vec2 st1 = dFdx(tex_coords);
    vec2 st2 = dFdy(tex_coords);

    vec3 N   = normalize(vert_norm);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * max(1.0 - cosTheta, 0.0);
}
// ----------------------------------------------------------------------------
void main() {
    vec3 albedo     = pow(texture(albedoMap, tex_coords).rgb, vec3(2.2));
    float metallic  = texture(metallicMap, tex_coords).r;
    float roughness = texture(roughnessMap, tex_coords).r;
    float ao        = texture(aoMap, tex_coords).r;

    vec3 N = GetNormalFromMap();
    vec3 V = normalize(camPos - vert_pos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)

    // High F0 makes objects look trippy
    vec3 F0 = vec3(0.9);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0);
    for(int i = 0; i < 100; ++i) {
        if (lights[i].on == 0) continue;
        // calculate per-light radiance
        vec3 L;
        vec3 radiance = lights[i].color.rgb * lights[i].intensity;

        if (lights[i].type == LIGHT_POINT) {
            L = normalize(lights[i].pos - vert_pos);
            float distance = length(lights[i].pos - vert_pos);
            float attenuation = 1.0 / (distance * distance);
            radiance *= attenuation;
        } else if (lights[i].type == LIGHT_SPOT) {
            L = -normalize(lights[i].target - lights[i].pos);
            float distance = length(lights[i].pos - vert_pos);
            float attenuation = 1.0 / (distance * distance);
            radiance *= attenuation;
        } else if (lights[i].type == LIGHT_SUN) {
            L = normalize(lights[i].target);
        }

        // Cook-Torrance BRDF
        vec3 H = normalize(V + L);
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 nominator = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
        vec3 specular = nominator / denominator;

        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - F;
        // multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;

        // scale light by NdotL
        // Setting this to 1 disables shadows
        float NdotL = max(dot(N, L), 0.0);

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }

    // ambient lighting
    vec3 surrounding_light = vec3(0.03);
    vec3 ambient = surrounding_light * albedo * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0 / 2.2));

    // fog
    float dist = length(camPos - vert_pos);
    const vec3 fogColor = vec3(0.078, 0.097, 0.057);

    float fogFactor = 1.0/exp((dist*fogDensity)*(dist*fogDensity));

    fogFactor = clamp(fogFactor, 0.0, 1.0);

    color = mix(fogColor, color, fogFactor);

    // 1.0 is opacity
    FragColor = vec4(color, 1.0);
}