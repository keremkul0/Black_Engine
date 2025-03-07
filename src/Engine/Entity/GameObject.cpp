//
// Created by Kerem on 3.03.2025.
//
// GameObject.cpp
#include "GameObject.h"
#include "../Component/BaseComponent.h" // tam tanım

void GameObject::Update(float deltaTime)
{
    for (auto& comp : components)
    {
        comp->Update(deltaTime);
    }
}

void GameObject::Draw()
{
    for (auto& comp : components)
    {
        comp->Draw();
    }
}

void GameObject::OnInspectorGUI()
{
    for (auto& comp : components)
    {
        comp->OnInspectorGUI();
    }
}
