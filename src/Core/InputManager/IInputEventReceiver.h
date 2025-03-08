#pragma once
#include "InputEvent.h"

class IInputEventReceiver {
public:
    virtual ~IInputEventReceiver() = default;
    
    // Girdi olaylarını işleme metodu
    virtual void ProcessInput(const InputEvent& event) = 0;
};