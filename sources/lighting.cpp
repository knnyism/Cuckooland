#include <lighting.h>
#include <iostream>
#include <resources.h>

raylib::Texture albedoDefault, aoDefault, metallicDefault, normalDefault, roughnessDefault;

static int lightCount = 0;
static raylib::Shader* shader;

void LoadLighting() {
    shader = new raylib::Shader(
        GetAssetPath("/shaders/pbr.vs"),
        GetAssetPath("/shaders/pbr.fs")
    );

    shader->locs[SHADER_LOC_MAP_ALBEDO] = shader->GetLocation("albedoMap");
    shader->locs[SHADER_LOC_MAP_NORMAL] = shader->GetLocation("normalMap");
    shader->locs[SHADER_LOC_MAP_METALNESS] = shader->GetLocation("metallicMap");
    shader->locs[SHADER_LOC_MAP_ROUGHNESS] = shader->GetLocation("roughnessMap");
    shader->locs[SHADER_LOC_MAP_OCCLUSION] = shader->GetLocation("aoMap");

    shader->locs[SHADER_LOC_MATRIX_VIEW] = shader->GetLocation("matView");
    shader->locs[SHADER_LOC_MATRIX_PROJECTION] = shader->GetLocation("matProjection");
    shader->locs[SHADER_LOC_MATRIX_MODEL] = shader->GetLocation("matModel");
    shader->locs[SHADER_LOC_MATRIX_MVP] = shader->GetLocation("mvp");
    shader->locs[SHADER_LOC_VECTOR_VIEW] = shader->GetLocation("camPos");

    albedoDefault = LoadTextureFromImage(GenImageColor(1, 1, raylib::WHITE));
    aoDefault = LoadTextureFromImage(GenImageColor(1, 1, raylib::WHITE));
    metallicDefault = LoadTextureFromImage(GenImageColor(1, 1, raylib::BLACK));
    normalDefault = LoadTextureFromImage(GenImageColor(1, 1, { 128, 128, 255, 255 }));
    roughnessDefault = LoadTextureFromImage(GenImageColor(1, 1, raylib::WHITE));
}

void UpdateLighting() {
    float cameraPos[3] = { camera->position.x, camera->position.y, camera->position.z };
    shader->SetValue(shader->locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
}

static void UpdateLight(Light light) {
    float color[3] = { light.color.r, light.color.g, light.color.b };

    shader->SetValue(light.positionLoc, &light.pos, SHADER_UNIFORM_VEC3);
    shader->SetValue(light.colorLoc, color, SHADER_UNIFORM_VEC3);
    shader->SetValue(light.targetLoc, &light.target, SHADER_UNIFORM_VEC3);
    shader->SetValue(light.intensityLoc, &light.intensity, SHADER_UNIFORM_FLOAT);
    shader->SetValue(light.typeLoc, &light.type, SHADER_UNIFORM_INT);
    shader->SetValue(light.enabledLoc, &light.enabled, SHADER_UNIFORM_INT);
}

Light CreateLight(LightType type, Vector3 position, Vector3 target, raylib::Color color, float intensity) {
    Light light{};

    light.enabled = true;

    light.type = type;
    light.pos = position;
    light.target = target;
    light.color = color;
    light.intensity = intensity;

    std::string lightIndex = std::to_string(lightCount);

    light.positionLoc = shader->GetLocation("lights[" + lightIndex + "].pos");
    light.targetLoc = shader->GetLocation("lights[" + lightIndex + "].target");
    light.colorLoc = shader->GetLocation("lights[" + lightIndex + "].color");
    light.intensityLoc = shader->GetLocation("lights[" + lightIndex + "].intensity");
    light.typeLoc = shader->GetLocation("lights[" + lightIndex + "].type");
    light.enabledLoc = shader->GetLocation("lights[" + lightIndex + "].on");

    UpdateLight(light);

    lightCount++;

    return light;
}

Material LoadPbrMaterial(Texture albedo, Texture normal, Texture roughness) {
    Material mat = LoadMaterialDefault();
    mat.shader = *shader;

    mat.maps[MATERIAL_MAP_ALBEDO].texture = (albedo.id != 0) ? albedo : albedoDefault;
    mat.maps[MATERIAL_MAP_NORMAL].texture = (normal.id != 0) ? normal : normalDefault;
    mat.maps[MATERIAL_MAP_METALNESS].texture = (roughness.id != 0) ? roughness : roughnessDefault;

    GenTextureMipmaps(&mat.maps[MATERIAL_MAP_ALBEDO].texture);
    GenTextureMipmaps(&mat.maps[MATERIAL_MAP_NORMAL].texture);
    GenTextureMipmaps(&mat.maps[MATERIAL_MAP_METALNESS].texture);
    GenTextureMipmaps(&mat.maps[MATERIAL_MAP_ROUGHNESS].texture);
    GenTextureMipmaps(&mat.maps[MATERIAL_MAP_OCCLUSION].texture);

    return mat;
}
