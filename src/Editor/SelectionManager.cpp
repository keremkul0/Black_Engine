#include "SelectionManager.h"
#include <algorithm>
#include <iostream>

SelectionManager& SelectionManager::GetInstance() {
    static SelectionManager instance;
    return instance;
}

void SelectionManager::SetSelectedObject(const std::shared_ptr<GameObject>& object) {
    try {
        // If object is already selected, do nothing
        if (m_SelectedObject == object) {
            return;
        }
        
        std::cout << "--------- Selection Change ---------" << std::endl;
        
        // Clear selection state on previously selected object
        if (m_SelectedObject) {
            m_SelectedObject->isSelected = false;
            std::cout << "Deselected: " << m_SelectedObject->GetName() << std::endl;
        }
        
        // Update selected object
        m_SelectedObject = object;
        
        // Set selection state on newly selected object
        if (m_SelectedObject) {
            m_SelectedObject->isSelected = true;
            std::cout << "Selected: " << m_SelectedObject->GetName() << std::endl;
        } else {
            std::cout << "Selection cleared" << std::endl;
        }
        
        // Log the number of registered listeners
        std::cout << "Notifying " << m_Listeners.size() << " listeners of selection change" << std::endl;
        
        // Notify all listeners of the selection change
        NotifyListeners();
        
        std::cout << "----------------------------------" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in SetSelectedObject: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception in SetSelectedObject" << std::endl;
    }
}

std::shared_ptr<GameObject> SelectionManager::GetSelectedObject() const {
    return m_SelectedObject;
}

void SelectionManager::ClearSelection() {
    SetSelectedObject(nullptr);
}

void SelectionManager::AddSelectionChangedListener(const SelectionChangedCallback& callback) {
    m_Listeners.push_back(callback);
}

void SelectionManager::RemoveSelectionChangedListener(const SelectionChangedCallback& callback) {
    // Note: This simplified implementation removes all listeners with the same address
    // For more complex scenarios, you might need a better identifier for callbacks
    m_Listeners.erase(
        std::remove_if(m_Listeners.begin(), m_Listeners.end(),
            [&callback](const auto& listener) {
                // Compare function pointers (simplistic approach)
                return &callback == &listener; 
            }
        ),
        m_Listeners.end()
    );
}

void SelectionManager::NotifyListeners() {
    for (const auto& listener : m_Listeners) {
        try {
            listener(m_SelectedObject);
        }
        catch (const std::exception& e) {
            std::cerr << "Exception in selection listener callback: " << e.what() << std::endl;
        }
        catch (...) {
            std::cerr << "Unknown exception in selection listener callback" << std::endl;
        }
    }
}