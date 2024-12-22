#include "Node.hpp"



Node::Node()
{
    m_id = 0;
    m_name = "";
    m_castShadows = false;
    m_shadow = false;
    m_visible = true;
}

Node::~Node()
{
}

void Node::Update(float dt)
{
    (void)dt;
}

void Node::Render(Shader *shader)
{
    (void)shader;
}


void Node::Release()    
{
 
}