#include "Node.hpp"



Node::Node()
{
    m_id = 0;
    m_name = "";
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