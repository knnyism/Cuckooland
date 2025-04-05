#include <game.h>

Mat44 cameraMatrix = Mat44::sTranslation(Vec3::sReplicate(10000));

Entity* FindEntity(EntityId id)
{
    Entity* ent = entities.buckets[id.bucket]->data[id.index];

    if (ent->IsAlive && ent->id == id) {
        return ent;
    }

    return nullptr;
}

void DestroyEntity(Entity* ent)
{
    std::cout << "ENT: DESTROY: at bucket " << ent->id.bucket << " with id " << ent->id.index << std::endl;
    ent->IsAlive = false;
    ent->Destroy();
}

namespace game {
    Sound3D* playingSound3Ds[MAX_SOUNDS];

    void SetCameraMatrix(Mat44 matrix) {
        cameraMatrix = matrix;

        Vec3 pos = matrix.GetTranslation();
        Quat rot = matrix.GetRotation().GetQuaternion();

        Vec3 target = pos + rot * Vec3::sAxisZ();
        Vec3 up = rot * Vec3::sAxisY();

        camera->SetPosition({ pos.GetX(), pos.GetY(), pos.GetZ() });
        camera->SetTarget({ target.GetX() , target.GetY(), target.GetZ() });
        camera->SetUp({ up.GetX(), up.GetY(), up.GetZ() });
    }

    Mat44 GetCameraMatrix() {
        return cameraMatrix;
    }

    void Model::Load( std::string path, Vec3 position, Quat rotation, f32 overrideMass, f32 overrideFriction, EMotionType motionType, ObjectLayer layer) {
        raylibModel = GetModel(path);
        BodyCreationSettings bcs = BodyCreationSettings(new ConvexHullShapeSettings(*GetShape(path)), position, rotation, motionType, layer);

        std::cout << "Layer: " << layer << std::endl;

        if (overrideMass != -1.0f) {
            std::cout << "Overriding mass: " << overrideMass << std::endl;
            bcs.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
            bcs.mMassPropertiesOverride.mMass = overrideMass;
        }

        bcs.mFriction = overrideFriction;

        body = body_interface->CreateBody(bcs);
        bodyId = body->GetID();

        body_interface->AddBody(bodyId, EActivation::Activate);
    }
    void Model::Tick() {
        positionState.Set(body_interface->GetPosition(bodyId));
        rotationState.Set(body_interface->GetRotation(bodyId));
    }
    void Model::Render() {
        Vec3 position = positionState.Get();
        Quat rotation = rotationState.Get().Normalized(); // Needed cause... IDK GetAxisAngle throws a fit

        Vec3 rotationAxis;
        f32 rotationAngle;
        rotation.GetAxisAngle(rotationAxis, rotationAngle);

        raylibModel->Draw(
            { position.GetX(), position.GetY(), position.GetZ() },
            { rotationAxis.GetX(), rotationAxis.GetY(), rotationAxis.GetZ() },
            rotationAngle * RAD2DEG
        );
    }

    void Sound::Play() {
        isStopped = false;

        if (maxVariations > 1)
        {
            soundIndex = rand() % maxInstances;
        }
        else
        {
            soundIndex++;
            if (soundIndex >= maxInstances)
                soundIndex = 0;
        }

        currentSound = soundArray[soundIndex++];

        currentSound->SetVolume(volume);
        currentSound->Play();
    }

    void Sound::Stop() {
        isStopped = true;

        for (u8 i = 0; i < maxInstances; i++) {
            if (soundArray[i] == nullptr)
                continue;

            if (soundArray[i]->IsPlaying())
                soundArray[i]->Stop();
        }
    }

    void Sound3D::Play() {
        for (u8 i = 0; i < maxInstances; i++) {
            if (soundArray[i] == nullptr)
                continue;

            if (soundArray[i]->IsPlaying())
                continue;

            for (u8 j = 0; j < MAX_SOUNDS; j++) {
                if (playingSound3Ds[j] == nullptr) {
                    playingSound3Ds[j] = this;
                    break;
                }
            }

            break;
        }

        Sound::Play();
        Update();
    }

    void Sound3D::Update() {
        Vec3 cameraPos = cameraMatrix.GetTranslation();

        f32 distance = (cameraPos - position).Length();

        if (distance < 1.0f) {
            currentSound->SetVolume(volume);
            currentSound->SetPan(0.5f);
            return;
        }

        f32 normalizedDistance = min(distance / maxDistance, 1.0f);
        f32 distanceVolume = 1.0f - normalizedDistance;

        // Calculate stereo panning based on the relative position of the sound source and the camera
        Vec3 soundDirection = (position - cameraPos).Normalized();
        f32 panning = soundDirection.Dot(cameraMatrix.GetAxisX());

        // Set volume and panning
        currentSound->SetVolume(volume * distanceVolume);
        currentSound->SetPan(panning + 0.5f);
    }

    void Sound3D::Stop() {
        Sound::Stop();
    }

    void UpdateSound3Ds() {
        for (u8 i = 0; i < MAX_SOUNDS; i++) {
            Sound3D* sound = playingSound3Ds[i];
            if (sound != nullptr) {
                sound->isPlaying = sound->currentSound->IsPlaying();

                if (sound->isPlaying) {
                    sound->Update();
                }
                else {
                    if (sound->isLooped) {
                        sound->Play();
                    }

                    playingSound3Ds[i] = nullptr;
                }
            }
        }
    }
}