#pragma once
#include "Config.hpp"
#include "Entity.hpp"
#include "OS.hpp"
#include "File.hpp"

KeyFrame::KeyFrame(KeyFrame *t)
{
    if ((!t) || (t->numPositionKeys() == 0) || (t->numRotationKeys() == 0))
    {

        LogError("Null animation pointer or nor frames");
        return;
    }

    for (u32 i = 0; i < t->numPositionKeys(); i++)
    {
        positionKeyFrames.push_back(t->positionKeyFrames[i]);
    }

    for (u32 i = 0; i < t->numRotationKeys(); i++)
    {
        rotationKeyFrames.push_back(t->rotationKeyFrames[i]);
    }
}

int KeyFrame::GetPositionIndex(float animationTime)
{
    // SDL_Log("KeyFrame time %f  %ld",animationTime,positionKeyFrames.size());
    for (u32 index = 0; index < positionKeyFrames.size() - 1; ++index)
    {
        if (animationTime < positionKeyFrames[index + 1].frame)
            return index;
    }

    DEBUG_BREAK_IF(true);
    return 0;
}

int KeyFrame::GetRotationIndex(float animationTime)
{
    for (u32 index = 0; index < rotationKeyFrames.size() - 1; ++index)
    {
        if (animationTime < rotationKeyFrames[index + 1].frame)
            return index;
    }
    //   SDL_Log("KeyFrame time %f  %ld",animationTime,rotationKeyFrames.size());
    DEBUG_BREAK_IF(true);
    return 0;
}

float KeyFrame::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
    float scaleFactor = 0.0f;
    float midWayLength = animationTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    scaleFactor = midWayLength / framesDiff;
    return scaleFactor;
}

Quaternion KeyFrame::AnimateRotation(float movetime)
{
    if (rotationKeyFrames.size() == 1)
    {
        return rotationKeyFrames[0].rot;
    }
    int currentIndex = GetRotationIndex(movetime);
    int nextIndex = currentIndex + 1;

    float factor = GetScaleFactor(rotationKeyFrames[currentIndex].frame, rotationKeyFrames[nextIndex].frame, movetime);

    //    SDL_Log(" %f %f %f",rotationKeyFrames[currentIndex].frame, rotationKeyFrames[nextIndex].frame,movetime);

    return Quaternion::Slerp(rotationKeyFrames[currentIndex].rot, rotationKeyFrames[nextIndex].rot, factor);
}
Vec3 KeyFrame::AnimatePosition(float movetime)
{
    if (positionKeyFrames.size() == 1)
    {
        return positionKeyFrames[0].pos;
    }
    int currentIndex = GetPositionIndex(movetime);
    int nextIndex = currentIndex + 1;

    float factor = GetScaleFactor(positionKeyFrames[currentIndex].frame, positionKeyFrames[nextIndex].frame, movetime);
    return Vec3::Lerp(positionKeyFrames[currentIndex].pos, positionKeyFrames[nextIndex].pos, factor);
}

void KeyFrame::setPositionKey(int frame, const Vec3 &p)
{
    positionKeyFrames[frame].pos = p;
}

void KeyFrame::setRotationKey(int frame, const Quaternion &q)
{
    rotationKeyFrames[frame].rot = q;
}
void KeyFrame::AddPositionKeyFrame(float frame, const Vec3 &pos)
{
    positionKeyFrames.push_back(PosKeyFrame(pos, frame));
}

void KeyFrame::AddRotationKeyFrame(float frame, const Quaternion &rot)
{

    rotationKeyFrames.push_back(RotKeyFrame(rot, frame));
}

//-------------------------------------------------------------------------------
// Animation
//-------------------------------------------------------------------------------

Animation::Animation(const String &name)
{
    state = Stoped;
    method = 0;
    currentTime = 0.0f;
    fps = 30.0f;
    mode = LOOP;
    isEnd = false;
    this->name = name;
}

bool Animation::Play(u32 mode, float fps)
{
    if (state == Stoped)
    {
        state = Playing;
        this->mode = mode;
        this->fps = fps;
        currentTime = 0.0f;
        return true;
    }
    return false;
}

bool Animation::Stop()
{
    if (state == Playing)
    {
        state = Stoped;
        currentTime = 0.0f;
        isEnd = true;

        return true;
    }
    return false;
}

Frame *Animation::AddFrame(const String &name)
{
    Frame *frame = new Frame();
    frame->name = name;
    frames.push_back(frame);
    if (framesMap.insert(name, frame))
    {
        LogWarning("Frame %s already exists", name.c_str());
    }
    return frame;
}

bool Animation::IsEnded()
{
    if (state == Stoped)
    {
        return true;
    }
    return isEnd;
}

Animation::~Animation()
{
    for (u32 i = 0; i < frames.size(); i++)
    {
        delete frames[i];
    }
    frames.clear();
}

Frame *Animation::GetFrame(const String &name)
{
    Frame *frame = nullptr;
    if (!framesMap.find(name, frame))
    {
        LogError("Frame %s not found", name.c_str());
        return nullptr;
    }
    return frame;
}

Frame *Animation::GetFrame(int index)
{
    if (index < 0 || index >= (int)frames.size())
    {
        return NULL;
    }
    return frames[index];
}

bool Animation::Save(const String &path)
{

    String final_path = path + this->name + ".anim";

    FileStream stream(final_path, "wb");
    if (!stream.IsOpen())
    {
        LogError("Cannot save %s", final_path.c_str());
        return false;
    }

    stream.WriteChar('A');
    stream.WriteChar('H');
    stream.WriteChar('3');
    stream.WriteChar('D');
    stream.WriteInt(2024);
    stream.WriteUTFString(name);
    stream.WriteDouble(duration);
    stream.WriteDouble(fps);
    u32 countFrames = frames.size();
    stream.WriteInt(countFrames);

    for (u32 i = 0; i < countFrames; i++)
    {
        Frame *frame = frames[i];
        String name = frame->name;
        stream.WriteUTFString(name);
        stream.WriteInt(frame->keys.numPositionKeys());
        stream.WriteInt(frame->keys.numRotationKeys());

        for (u32 j = 0; j < frame->keys.numPositionKeys(); j++)
        {
            stream.WriteFloat(frame->keys.positionKeyFrames[j].frame);
            stream.WriteFloat(frame->keys.positionKeyFrames[j].pos.x);
            stream.WriteFloat(frame->keys.positionKeyFrames[j].pos.y);
            stream.WriteFloat(frame->keys.positionKeyFrames[j].pos.z);
        }
        for (u32 j = 0; j < frame->keys.numRotationKeys(); j++)
        {
            stream.WriteFloat(frame->keys.rotationKeyFrames[j].frame);
            stream.WriteFloat(frame->keys.rotationKeyFrames[j].rot.x);
            stream.WriteFloat(frame->keys.rotationKeyFrames[j].rot.y);
            stream.WriteFloat(frame->keys.rotationKeyFrames[j].rot.z);
            stream.WriteFloat(frame->keys.rotationKeyFrames[j].rot.w);
        }
    }

    stream.Close();

    return true;
}

bool Animation::Load(const String &fileName)
{

    FileStream stream(fileName, "rb");
    if (!stream.IsOpen())
    {
        LogError("Cannot load %s", fileName.c_str());
        return false;
    }

    stream.ReadChar();
    stream.ReadChar();
    stream.ReadChar();
    stream.ReadChar();

    u32 version = stream.ReadInt();

    if (version != 2024)
    {
        LogError("Unsupported version %d", version);
        return false;
    }

    name = stream.ReadUTFString();
    duration = stream.ReadDouble();
    fps = stream.ReadDouble();

    LogInfo("Loading animation (%s) %f %f", name.c_str(), fps, duration);

    u32 countFrames = stream.ReadInt();

    for (u32 i = 0; i < countFrames; i++)
    {

        String name = stream.ReadUTFString();
        Frame *frame = AddFrame(name);

        u32 positions = stream.ReadInt();
        u32 rotations = stream.ReadInt();
        for (u32 j = 0; j < positions; j++)
        {
            float time = stream.ReadFloat();
            Vec3 pos;
            pos.x = stream.ReadFloat();
            pos.y = stream.ReadFloat();
            pos.z = stream.ReadFloat();
            frame->keys.AddPositionKeyFrame(time, pos);
        }
        for (u32 j = 0; j < rotations; j++)
        {
            float time = stream.ReadFloat();
            Quaternion rot;
            rot.x = stream.ReadFloat();
            rot.y = stream.ReadFloat();
            rot.z = stream.ReadFloat();
            rot.w = stream.ReadFloat();
            frame->keys.AddRotationKeyFrame(time, rot);
        }
    }

    stream.Close();
    return true;
}

float Animation::GetDuration()
{
    return duration;
}

float Animation::GetTime()
{
    return currentTime;
}

float Animation::GetFPS()
{
    return fps;
}

int Animation::GetMode()
{

    return mode;
}
u64 Animation::GetState()
{
    return state;
}

void Animation::Force()
{

    if (frames.size() == 0)
    {
        return;
    }

    for (u32 i = 0; i < frames.size(); i++)
    {
        Frame *b = frames[i];
        if (b->keys.numPositionKeys() > 0)
        {
            b->pos = true;
        }

        if (b->keys.numRotationKeys() > 0)
        {
            b->rot = true;
        }
    }
}

void Animation::Update(float elapsed)
{

    if (frames.size() == 0)
    {
        return;
    }
    if (state == Stoped)
    {
        return;
    }
    isEnd = false;

    currentTime += elapsed * fps;

    switch (mode)
    {

    case LOOP:
    {
        state = Playing;
        if (currentTime >= duration)
        {
            isEnd = true;
        }
        currentTime = fmod(currentTime, duration);
        if (currentTime < 0.0f)
        {
            currentTime += duration;
        }
        break;
    }
    case PINGPONG:
    {
        state = Playing;
        currentTime = fmod(currentTime, duration * 2);
        if (currentTime > (duration * 2))
        {
            isEnd = true;
        }
        if (currentTime < 0.0f)
        {
            currentTime += duration * 2;
        }

        if (currentTime > duration)
        {
            currentTime = duration - (currentTime - duration);
            fps = -fps;
        }
        break;
    }
    case ONESHOT:
    {
        if (currentTime >= duration)
        {
            LogWarning("Stop Animation : %s. Duration : %f. CurrentTime : %f", name.c_str(), duration, currentTime);

            Stop();
        }
        break;
    }
    }

    for (u32 i = 0; i < frames.size(); i++)
    {
        Frame *b = frames[i];

        if (b->keys.numPositionKeys() > 0)
        {
            b->position = b->keys.AnimatePosition(currentTime);
            b->pos = true;
        }

        if (b->keys.numRotationKeys() > 0)
        {
            b->orientation = b->keys.AnimateRotation(currentTime);
            b->rot = true;
        }
    }
}

//-------------------------------------------------------------------------------
// Animator
//-------------------------------------------------------------------------------

Animator::Animator(Entity *parent)
{
    currentAnimation = nullptr;

    blendFactor = 0.0f;
    blendTime = 0.0f;
    blending = false;
    currentAnimationName = "";
    entity = parent;
}

Animator::~Animator()
{
    for (u32 i = 0; i < m_animations.size(); i++)
    {
        delete m_animations[i];
    }
}

void Animator::SaveAllFrames(const String &path)
{
    for (u32 i = 0; i < m_animations.size(); i++)
    {
        m_animations[i]->Save(path);
    }
}

Animation *Animator::LoadAnimation(const String &name)
{
    Animation *a = new Animation(name);
    if (a->Load(name))
    {

        m_animations.push_back(a);
        m_animations_map.insert(name, a);
        return a;
    }

    delete a;
    return nullptr;
}
void Animator::Update(float elapsed)
{
    if (blending)
    {
        blendFactor += elapsed / blendTime;

        if (blendFactor >= 0.99f)
        {
            blendFactor = 1.0f;
            blending = false;
            updateAnim(elapsed);
            return;
        }

        updateTrans(blendFactor);
        return;
    }

    updateAnim(elapsed);
}

Animation *Animator::GetAnimation(const String &name)
{
    Animation *animation = nullptr;
    if (!m_animations_map.find(name, animation))
    {
        LogWarning("Animator: Animation %s not found", name.c_str());
        return nullptr;
    }

    return animation;
}

Animation *Animator::GetAnimation(int index)
{
    DEBUG_BREAK_IF(index >= (int)m_animations.size());
    return m_animations[index];
}
u32 Animator::GetTotalAnimations() const
{
    return (u32)m_animations.size();
}

String Animator::GetCurrentAnimationName()
{
    DEBUG_BREAK_IF(!currentAnimation);
    return currentAnimation->name;
}

float Animator::GetCurrentFrame()
{
    DEBUG_BREAK_IF(!currentAnimation);
    return currentAnimation->GetTime();
}
Animation *Animator::AddAnimation(const String &name)
{
    Animation *a = new Animation(name);

    for (size_t i = 0; i < entity->joints.size(); ++i)
    {
        a->AddFrame(entity->joints[i]->GetName());
    }

    m_animations.push_back(a);
    m_animations_map.insert(name, a);
    return a;
}

void Animator::beginTrans()
{

    if (!currentAnimation)
        return;

    for (size_t i = 0; i < entity->joints.size(); i++)
    {

        Frame *frame = currentAnimation->GetFrame(i);
        if (frame)
        {
            if (frame->keys.positionKeyFrames.size() != 0 && !frame->IgnorePosition)
            {
                frame->src_pos = entity->joints[i]->getLocalPosition();

                frame->dest_pos = frame->keys.positionKeyFrames[0].pos;
            }
            if (frame->keys.rotationKeyFrames.size() != 0 && !frame->IgnoreRotation)
            {
                frame->src_rot = entity->joints[i]->getLocalRotation();
                frame->dest_rot = frame->keys.rotationKeyFrames[0].rot;
            }
        }
    }
}

void Animator::updateTrans(float blend)
{

    if (!currentAnimation)
        return;
    for (size_t i = 0; i < entity->joints.size(); i++)
    {
        Joint *joint = entity->joints[i];

        Frame *frame = currentAnimation->GetFrame(i);
        if (frame)
        {
            if (frame->pos && !frame->IgnorePosition)
                joint->setLocalPosition(Vec3::Lerp(frame->src_pos, frame->dest_pos, blend));

            if (frame->rot && !frame->IgnoreRotation)
                joint->setLocalRotation(Quaternion::Slerp(frame->src_rot, frame->dest_rot, blend));
        }
    }
}

void Animator::updateAnim(float elapsed)
{
    if (!currentAnimation)
        return;
    currentAnimation->Update(elapsed);

    if (currentAnimation->IsEnded())
    {
        return;
    };

    for (size_t i = 0; i < entity->joints.size(); i++)
    {
        Frame *frame = currentAnimation->GetFrame(i);
        Joint *joint = entity->joints[i];
        if (frame)
        {
            if (frame->pos && !frame->IgnorePosition)
            {
                Vec3 pos = Vec3::Lerp(joint->getLocalPosition(), frame->position, elapsed * 0.5f);
                entity->joints[i]->setLocalPosition(pos);
            }
            if (frame->rot && !frame->IgnoreRotation)
                entity->joints[i]->setLocalRotation(frame->orientation);
        }
    }
}

bool Animator::Play(const String &name, int mode, float blendTime)
{


    if (!m_animations_map.find(name, currentAnimation))
    {
        LogWarning("Animator: Animation %s not found", name.c_str());
        return false;
    }

    if (currentAnimation)
    {
        if (currentAnimationName == name && currentAnimation->GetState() == Animation::Playing)
        {

            return false;
        }
    }

    currentAnimationName = name;
    if (currentAnimation)
    {
        currentAnimation->Stop();
    }

    currentAnimation->Play(mode, currentAnimation->GetFPS());
    currentAnimation->Force();

    this->blendTime = blendTime;
    blendFactor = 0.0f;
    blending = true;

    beginTrans();

    return true;
}

void Animator::Stop()
{

    if (currentAnimation)
    {
        currentAnimation->Stop();
    }
}
bool Animator::IsEnded()
{

    if (currentAnimation)
    {
        return currentAnimation->IsEnded();
    }
    return true;
}
bool Animator::IsPlaying()
{

    if (currentAnimation)
    {
        return currentAnimation->GetState() == Animation::Playing;
    }
    return false;
}

void Animator::SetIgnorePosition(const String &name, bool ignore)
{

    for (size_t i = 0; i < m_animations.size(); i++)
    {
        Frame *frame = m_animations[i]->GetFrame(name);
        if (frame)
        {
            frame->IgnorePosition = ignore;
        }
    }
}
void Animator::SetIgnoreRotation(const String &name, bool ignore)
{

    for (size_t i = 0; i < m_animations.size(); i++)
    {
        Frame *frame = m_animations[i]->GetFrame(name);
        if (frame)
        {
            frame->IgnoreRotation = ignore;
        }
    }
}

//********************************************************************************************************************
// JOINT
//********************************************************************************************************************

//********************************************************************************************************************
// Entity
//********************************************************************************************************************
Entity::Entity()
{

    
    animator = new Animator(this);

    bones.reserve(MAX_BONES);

    for (u32 i = 0; i < MAX_BONES; i++)
    {
        bones.push_back(Mat4::Identity());
    }
}

void Entity::UpdateAnimation(float dt)
{

    //   SDL_Log("CurrentTime : %f Duration : %f TicksPerSecond : %f DeltaTime : %f",m_CurrentTime,m_Duration,m_TicksPerSecond,m_DeltaTime);
    animator->Update(dt);
    for (u32 i = 0; i < joints.size(); i++)
    {

        Joint *b = joints[i];
        b->Update(dt);
        //Mat4::fastMult43(bones[i], b->AbsoluteTransformation, b->offset);
    }
}

bool Entity::Save(const String &name)
{

    FileStream stream(name, "wb");
    if (!stream.IsOpen())
    {
        LogError("Cannot save %s", name.c_str());
        return false;
    }

    // stream.WriteChar('A');
    // stream.WriteChar('H');
    // stream.WriteChar('3');
    // stream.WriteChar('D');
    // stream.WriteInt(2024);

    // u32 material = materials.size();

    // stream.WriteInt(material);

    // for (u32 i = 0; i < material; i++)
    // {
    //     Material *mat = materials[i];
    //     stream.WriteUTFString(mat->name);
    //     stream.WriteUTFString(mat->diffuse);
    // }

    // u32 coutMesh = surfaces.size();
    // stream.WriteInt(coutMesh);
    // for (u32 i = 0; i < coutMesh; i++)
    // {
    //     SkinSurface *surf = surfaces[i];
    //     stream.WriteUTFString(surf->name);
    //     stream.WriteInt(surf->material);
    //     stream.WriteInt(surf->vertices.size());
    //     stream.WriteInt(surf->faces.size() / 3);

    //     for (u32 j = 0; j < surf->vertices.size(); j++)
    //     {
    //         stream.WriteFloat(surf->vertices[j].pos.x);
    //         stream.WriteFloat(surf->vertices[j].pos.y);
    //         stream.WriteFloat(surf->vertices[j].pos.z);

    //         stream.WriteFloat(surf->vertices[j].normal.x);
    //         stream.WriteFloat(surf->vertices[j].normal.y);
    //         stream.WriteFloat(surf->vertices[j].normal.z);

    //         stream.WriteFloat(surf->vertices[j].uv.x);
    //         stream.WriteFloat(surf->vertices[j].uv.y);

    //         stream.WriteFloat(surf->vertices[j].weights[0]);
    //         stream.WriteInt(surf->vertices[j].bones[0]);

    //         stream.WriteFloat(surf->vertices[j].weights[1]);
    //         stream.WriteInt(surf->vertices[j].bones[1]);

    //         stream.WriteFloat(surf->vertices[j].weights[2]);
    //         stream.WriteInt(surf->vertices[j].bones[2]);

    //         stream.WriteFloat(surf->vertices[j].weights[3]);
    //         stream.WriteInt(surf->vertices[j].bones[3]);
    //     }

    //     for (u32 j = 0; j < (surf->faces.size() / 3); j++)
    //     {
    //         stream.WriteInt(surf->faces[j * 3 + 0]);
    //         stream.WriteInt(surf->faces[j * 3 + 1]);
    //         stream.WriteInt(surf->faces[j * 3 + 2]);
    //     }
    // }

    // u32 countJoints = joints.size();
    // stream.WriteInt(countJoints);
    // for (u32 i = 0; i < countJoints; i++)
    // {
    //     Joint *joint = joints[i];
    //     std::string name = joint->name;
    //     std::string parent = joint->parentName;

    //     stream.WriteUTFString(name);
    //     stream.WriteUTFString(parent);

    //     stream.WriteFloat(joint->position.x);
    //     stream.WriteFloat(joint->position.y);
    //     stream.WriteFloat(joint->position.z);

    //     stream.WriteFloat(joint->orientation.x);
    //     stream.WriteFloat(joint->orientation.y);
    //     stream.WriteFloat(joint->orientation.z);
    //     stream.WriteFloat(joint->orientation.w);

    //     stream.WriteFloat(joint->scale.x);
    //     stream.WriteFloat(joint->scale.y);
    //     stream.WriteFloat(joint->scale.z);

    //     stream.WriteFloat(joint->offset.x[0]);
    //     stream.WriteFloat(joint->offset.x[1]);
    //     stream.WriteFloat(joint->offset.x[2]);
    //     stream.WriteFloat(joint->offset.x[3]);

    //     stream.WriteFloat(joint->offset.x[4]);
    //     stream.WriteFloat(joint->offset.x[5]);
    //     stream.WriteFloat(joint->offset.x[6]);
    //     stream.WriteFloat(joint->offset.x[7]);

    //     stream.WriteFloat(joint->offset.x[8]);
    //     stream.WriteFloat(joint->offset.x[9]);
    //     stream.WriteFloat(joint->offset.x[10]);
    //     stream.WriteFloat(joint->offset.x[11]);

    //     stream.WriteFloat(joint->offset.x[12]);
    //     stream.WriteFloat(joint->offset.x[13]);
    //     stream.WriteFloat(joint->offset.x[14]);
    //     stream.WriteFloat(joint->offset.x[15]);
    // }

    stream.Close();
    return true;
}

bool Entity::Load(const String &name)
{

    FileStream stream(name, "rb");
    if (!stream.IsOpen())
    {
        LogError("Cannot load %s", name.c_str());
        return false;
    }

    stream.ReadChar();
    stream.ReadChar();
    stream.ReadChar();
    stream.ReadChar();

    u32 version = stream.ReadInt();

    if (version != 2024)
    {
        LogError("Unsupported version %d", version);
        return false;
    }

    u32 countMaterial = stream.ReadInt();

    for (u32 i = 0; i < countMaterial; i++)
    {
        Material *mat = new Material();
        String name = stream.ReadUTFString();
        String diffuse = stream.ReadUTFString();

        Logger::Instance().Info("Material %s texture %s", name.c_str(), diffuse.c_str());
      //  mat->texture = TextureManager::Instance().Load(mat->diffuse.c_str());
        materials.push_back(mat);
    }

    u32 countSurfaces = stream.ReadInt();
    for (u32 i = 0; i < countSurfaces; i++)
    {

        SkinSurface *surf = AddSurface();
        surf->name = stream.ReadUTFString();
        surf->material = stream.ReadInt();
        u32 countVertices = stream.ReadInt();
        u32 countFaces = stream.ReadInt();

        LogInfo("Surface %s %d %d %d", surf->name.c_str(), surf->material, countVertices, countFaces);

        for (u32 j = 0; j < countVertices; j++)
        {
            SkinVertex vertex;

            vertex.pos.x = stream.ReadFloat();
            vertex.pos.y = stream.ReadFloat();
            vertex.pos.z = stream.ReadFloat();

            vertex.normal.x = stream.ReadFloat();
            vertex.normal.y = stream.ReadFloat();
            vertex.normal.z = stream.ReadFloat();

            vertex.uv.x = stream.ReadFloat();
            vertex.uv.y = stream.ReadFloat();

            vertex.weights[0] = stream.ReadFloat();
            vertex.bones[0] = stream.ReadInt();

            vertex.weights[1] = stream.ReadFloat();
            vertex.bones[1] = stream.ReadInt();

            vertex.weights[2] = stream.ReadFloat();
            vertex.bones[2] = stream.ReadInt();

            vertex.weights[3] = stream.ReadFloat();
            vertex.bones[3] = stream.ReadInt();
            surf->AddVertex(vertex);
        }

        for (u32 j = 0; j < countFaces; j++)
        {
            int a = stream.ReadInt();
            int b = stream.ReadInt();
            int c = stream.ReadInt();
            surf->AddFace(a, b, c);
        }

        surf->Build();
    }

    u32 countJoints = stream.ReadInt();
    for (u32 i = 0; i < countJoints; i++)
    {
        Joint *node = new Joint();
        String name = stream.ReadUTFString();
        String parentName = stream.ReadUTFString();

        //  LogInfo("Joint %s parent %s", node->name.c_str(), node->parentName.c_str());
        Vec3 position;
        Vec3 scale;
        Quaternion orientation;

        position.x = stream.ReadFloat();
        position.y = stream.ReadFloat();
        position.z = stream.ReadFloat();

        orientation.x = stream.ReadFloat();
        orientation.y = stream.ReadFloat();
        orientation.z = stream.ReadFloat();
        orientation.w = stream.ReadFloat();

        scale.x = stream.ReadFloat();
        scale.y = stream.ReadFloat();
        scale.z = stream.ReadFloat();

        float m[16];

        m[0] = stream.ReadFloat();
        m[1] = stream.ReadFloat();
        m[2] = stream.ReadFloat();
        m[3] = stream.ReadFloat();

        m[4] = stream.ReadFloat();
        m[5] = stream.ReadFloat();
        m[6] = stream.ReadFloat();
        m[7] = stream.ReadFloat();

        m[8] = stream.ReadFloat();
        m[9] = stream.ReadFloat();
        m[10] = stream.ReadFloat();
        m[11] = stream.ReadFloat();

        m[12] = stream.ReadFloat();
        m[13] = stream.ReadFloat();
        m[14] = stream.ReadFloat();
        m[15] = stream.ReadFloat();

        node->offset.set(m);

        //  node->parent = this;

        joints.push_back(node);

        for (u32 i = 0; i < joints.size(); i++)
        {
            if (joints[i]->GetName() == node->parentName)
            {
                node->setParent(joints[i]);
                break;
            }
        }
        node->Update(1.0f);
    }

    stream.Close();

    return true;
}

void Entity::Release()
{
    
    if (animator != nullptr)
    {
        delete animator;
        animator = nullptr;
    }

    for (u32 i = 0; i < materials.size(); i++)
    {
        materials[i]->Release();
        delete materials[i];
    }
    for (u32 i = 0; i < joints.size(); i++)
    {
        joints[i]->Release();
        delete joints[i];
    }

    for (u32 i = 0; i < surfaces.size(); i++)
    {
        surfaces[i]->Release();
        delete surfaces[i];
    }
}

Animation *Entity::LoadAnimation(const String &name)
{
    return animator->LoadAnimation(name);
}

void Entity::Play(const String &name, int mode, float blendTime)
{

    animator->Play(name, mode, blendTime);
}

void Entity::SetTexture(int layer, Texture2D *texture)
{
    DEBUG_BREAK_IF(layer >= (int)materials.size());
  //  materials[layer]->texture = texture;
}

Mat4 Entity::GetBone(u32 index) const
{
    DEBUG_BREAK_IF(index >= bones.size());
    return bones[index];
}

void Entity::Render(Shader *shader)
{

    for (u32 i = 0; i < surfaces.size(); i++)
    {
        SkinSurface *surf = surfaces[i];
      //  Material *material = materials[surf->material];
      ///  material->Bind();
        surf->Render();
    }

    

    for (u32 i = 0; i < joints.size(); i++)
    {
        joints[i]->Render(shader);
    }
}

void Entity::RenderNoMaterial(Shader *shader)
{
    for (u32 i = 0; i < surfaces.size(); i++)
    {
        SkinSurface *surf = surfaces[i];
        surf->Render();
    }

    

    for (u32 i = 0; i < joints.size(); i++)
    {
        joints[i]->Render(shader);
    }
}
Joint *Entity::GetJoint(u32 id)
{
    DEBUG_BREAK_IF(id >= joints.size());
    return joints[id];
}

Joint *Entity::FindJoint(const char *name)
{
    for (u32 i = 0; i < joints.size(); i++)
    {
        if (strcmp(joints[i]->GetName().c_str(), name) == 0)
        {
            return joints[i];
        }
    }
    return nullptr;
}
int Entity::GetJointIndex(const char *name)
{
    for (u32 i = 0; i < joints.size(); i++)
    {
        if (strcmp(joints[i]->GetName().c_str(), name) == 0)
        {
            return i;
        }
    }
    return -1;
}
u32 Entity::GetTotalJoints() const
{
    return (u32)joints.size();
}
u32 Entity::GetTotalSurfaces() const
{
    return (u32)surfaces.size();
}
u32 Entity::GetTotalBones() const
{
    return (u32)bones.size();
}
u32 Entity::GetTotalMaterials() const
{
    return  (u32) materials.size();
}
Material *Entity::AddMaterial()
{
    Material *material = new Material();
    materials.push_back(material);
    return material;
}

void Entity::AddJoint(Joint *joint)
{
    joints.push_back(joint);
}

SkinSurface *Entity::AddSurface()
{
    SkinSurface *surface = new SkinSurface();
    surfaces.push_back(surface);
    return surface;
}
Animator *Entity::GetAnimator()
{

    return animator;
}



//***********************************************************************************************************
void SkinSurface::AddVertex(const SkinVertex &vertex)
{
    vertices.push_back(vertex);
}

void SkinSurface::AddVertex(const Vec3 &v)
{
    SkinVertex vertex;
    vertex.pos = v;

    vertex.weights[0] = 0.0f;
    vertex.bones[0] = -1;

    vertex.weights[1] = 0.0f;
    vertex.bones[1] = -1;

    vertex.weights[2] = 1.0f;
    vertex.bones[2] = -1;

    vertex.weights[3] = 0.0f;
    vertex.bones[3] = -1;

    vertices.push_back(vertex);
}

void SkinSurface::AddVertex(const Vec3 &v, const Vec2 &uv)
{
    SkinVertex vertex;
    vertex.pos = v;
    vertex.uv = uv;

    vertex.weights[0] = 0.0f;
    vertex.bones[0] = -1;

    vertex.weights[1] = 0.0f;
    vertex.bones[1] = -1;

    vertex.weights[2] = 1.0f;
    vertex.bones[2] = -1;

    vertex.weights[3] = 0.0f;
    vertex.bones[3] = -1;

    vertices.push_back(vertex);
}

void SkinSurface::AddVertex(const Vec3 &v, const Vec2 &uv, const Vec3 &normal)
{
    SkinVertex vertex;
    vertex.pos = v;
    vertex.uv = uv;
    vertex.normal = normal;

    vertex.weights[0] = 0.0f;
    vertex.bones[0] = -1;

    vertex.weights[1] = 0.0f;
    vertex.bones[1] = -1;

    vertex.weights[2] = 1.0f;
    vertex.bones[2] = -1;

    vertex.weights[3] = 0.0f;
    vertex.bones[3] = -1;

    vertices.push_back(vertex);
}

void SkinSurface::AddFace(int a, int b, int c)
{
    faces.push_back(a);
    faces.push_back(b);
    faces.push_back(c);
}

void SkinSurface::AddIndex(int a)
{
    faces.push_back(a);
}

void SetVertexBoneData(SkinVertex &vertex, int boneID, float weight)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
    {
        if (vertex.bones[i] < 0)
        {
            vertex.weights[i] = weight;
            vertex.bones[i] = boneID;
            break;
        }
    }
}



void SkinSurface::Build()
{

glGenVertexArrays(1, &VAO);
glGenBuffers(1, &VBO);
glGenBuffers(1, &EBO);

glBindVertexArray(VAO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);

glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SkinVertex), &vertices[0], GL_STATIC_DRAW);

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(unsigned int), &faces[0], GL_STATIC_DRAW);

glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkinVertex), (void *)0);

// uv
glEnableVertexAttribArray(1);
glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SkinVertex), (void *)offsetof(SkinVertex, uv));


// normal

glEnableVertexAttribArray(2);
glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(SkinVertex), (void *)offsetof(SkinVertex, normal));

// ids
glEnableVertexAttribArray(5);
glVertexAttribIPointer(5, 4, GL_INT, sizeof(SkinVertex), (void *)offsetof(SkinVertex, bones));

// weights
glEnableVertexAttribArray(6);
glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(SkinVertex), (void *)offsetof(SkinVertex, weights));


glBindVertexArray(0);
}

void SkinSurface::Release()
{
glDeleteVertexArrays(1, &VAO);
glDeleteBuffers(1, &VBO);
glDeleteBuffers(1, &EBO);
}

void SkinSurface::Render()
{
glBindVertexArray(VAO);
glDrawElements(GL_TRIANGLES,(GLsizei) faces.size(), GL_UNSIGNED_INT, 0);
glBindVertexArray(0);
}