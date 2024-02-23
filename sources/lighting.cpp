#include <lighting.h>


void LoadLighting() {
    shader = new raylib::Shader(
        TextFormat("resources/shaders/glsl%i/pbr.vs", GLSL_VERSION),
        TextFormat("resources/shaders/glsl%i/pbr.fs", GLSL_VERSION)
    );

    // Load PBR shader and setup all required locations
    shader->locs[SHADER_LOC_MAP_ALBEDO] = shader->GetLocation("albedoMap");
    shader->locs[SHADER_LOC_MAP_METALNESS] = shader->GetLocation("mraMap");
    shader->locs[SHADER_LOC_MAP_NORMAL] = shader->GetLocation("normalMap");

    shader->locs[SHADER_LOC_MAP_EMISSION] = shader->GetLocation("emissiveMap");
    shader->locs[SHADER_LOC_COLOR_DIFFUSE] = shader->GetLocation("albedoColor");

    // Setup additional required shader locations, including lights data
    shader->locs[SHADER_LOC_VECTOR_VIEW] = shader->GetLocation("viewPos");

    int lightsCountLoc = shader->GetLocation("numOfLights");
    int maxLightCount = MAX_LIGHTS;
    shader->SetValue(lightsCountLoc, &maxLightCount, SHADER_UNIFORM_INT);

    float ambientIntensity = 0.02f;
    raylib::Color ambientColor(26, 32, 135, 255);
    Vector3 ambientColorNormalized = { (float)ambientColor.r / 255.0f, (float)ambientColor.g / 255.0f, (float)ambientColor.b / 255.0f };

    shader->SetValue(shader->GetLocation("ambientColor"), &ambientColorNormalized, SHADER_UNIFORM_VEC3);
    shader->SetValue(shader->GetLocation("ambient"), &ambientIntensity, SHADER_UNIFORM_FLOAT);

    Vector2 floorTextureTiling{ 0.5f, 0.5f };
    shader->SetValue(shader->GetLocation("tiling"), &floorTextureTiling, SHADER_UNIFORM_VEC2);
}

Light CreateLight(int type, Vector3 position, Vector3 target, raylib::Color color, float intensity) {
    Light light{ 0 };
    return light;
}
