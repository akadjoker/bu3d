#pragma once
#include "Config.hpp"
#include "Math.hpp"
#include "String.hpp"
#include "Shader.hpp"

class Node 
{
public:
    Node();
    virtual ~Node();
    virtual void Update(float dt);
    virtual void Render(Shader *shader);
    virtual void Release();

    void SetName(const String &name) { m_name = name; }
    const String &GetName() const { return m_name; }
    u32 GetID() const { return m_id; }
    void SetID(u32 id) { m_id = id; }

protected:
    u32 m_id;
    String m_name;
};