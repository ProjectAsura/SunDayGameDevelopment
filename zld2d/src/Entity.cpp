﻿//-----------------------------------------------------------------------------
// File : Entity.h
// Desc : Entity Object.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <Entity.h>
#include <Component.h>


///////////////////////////////////////////////////////////////////////////////
// Entity class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
Entity::~Entity()
{
    auto itr = m_Components.begin();
    while(itr != m_Components.end())
    {
        auto ptr = itr->second;
        itr->second = nullptr;
        delete ptr;
        itr = m_Components.erase(itr);
    }
    m_Components.clear();
}

//-----------------------------------------------------------------------------
//      更新処理を行います.
//-----------------------------------------------------------------------------
void Entity::Update(UpdateContext& context)
{
    for(auto& itr : m_Components)
    { itr.second->OnUpdate(context); }
}

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void Entity::Draw(SpriteSystem& sprite)
{
    for(auto& itr : m_Components)
    { itr.second->OnDraw(sprite); }
}

//-----------------------------------------------------------------------------
//      コンポーネントを追加します.
//-----------------------------------------------------------------------------
void Entity::AddComponent(uint32_t id, Component* component)
{
    if (component == nullptr)
    { return; }

    if (HasComponent(id))
    { RemoveComponent(id); }

    m_Components[id] = component;
    m_Components[id]->m_Entity = this;
}

//-----------------------------------------------------------------------------
//      コンポーネントを削除します.
//-----------------------------------------------------------------------------
void Entity::RemoveComponent(uint32_t id)
{
    if (!HasComponent(id))
    { return; }

    auto component = m_Components[id];
    m_Components.erase(id);

    delete component;
    component = nullptr;
}

//-----------------------------------------------------------------------------
//      コンポーネントを取得します.
//-----------------------------------------------------------------------------
Component* Entity::GetComponent(uint32_t id) const
{
    if (!HasComponent(id))
    { return nullptr; }

    return m_Components.at(id);
}

//-----------------------------------------------------------------------------
//      コンポーネントを保持するかチェックします.
//-----------------------------------------------------------------------------
bool Entity::HasComponent(uint32_t id) const
{ return m_Components.find(id) != m_Components.end(); }

//-----------------------------------------------------------------------------
//      バウンディングボックスを設定します.
//-----------------------------------------------------------------------------
void Entity::SetBox(const Box& value)
{ m_Box = value; }

//-----------------------------------------------------------------------------
//      バウンディングボックスを返却します.
//-----------------------------------------------------------------------------
const Box& Entity::GetBox() const
{ return m_Box; }

//-----------------------------------------------------------------------------
//      状態を設定します.
//-----------------------------------------------------------------------------
void Entity::SetState(Entity::STATE value)
{ m_State = value; }

//-----------------------------------------------------------------------------
//      状態を取得します.
//-----------------------------------------------------------------------------
Entity::STATE Entity::GetState() const
{ return m_State; }

//-----------------------------------------------------------------------------
//      疑似乱数を取得します.
//-----------------------------------------------------------------------------
asdx::PCG& Entity::GetRandom()
{ return m_Random; }

//-----------------------------------------------------------------------------
//      メッセージ受信時の処理です.
//-----------------------------------------------------------------------------
void Entity::OnMessage(const Message& msg)
{
    for(auto& itr : m_Components)
    { itr.second->OnReceive(msg); }
}
