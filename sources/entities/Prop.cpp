#include <entities/Prop.h>

void Prop::Load(const string path, Vec3 position, Quat rotation, f32 overrideMass) {
    model.Load(path, position, rotation, overrideMass);
}

void Prop::Destroy() {
    body_interface->RemoveBody(model.bodyId);
}

void Prop::Tick() {
    model.Tick();
}

void Prop::Render() {
    model.Render();
}