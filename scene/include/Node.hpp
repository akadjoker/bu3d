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

    void SetCastShadows(bool castShadows) { this->m_castShadows = castShadows; }
    bool GetCastShadows() const { return m_castShadows; }

    void SetShadow(bool shadow) { this->m_shadow = shadow; }
    bool GetShadow() const { return m_shadow; }

    void SetVisible(bool visible) { this->m_visible = visible; }
    bool GetVisible() const { return m_visible; }

protected:
    u32 m_id;
    String m_name;
    bool m_castShadows;
    bool m_shadow;
    bool m_visible ;
};