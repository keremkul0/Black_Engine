#ifndef SELECTION_MANAGER_H
#define SELECTION_MANAGER_H

#include <memory>
#include <functional>
#include <vector>
#include "Engine/Entity/GameObject.h"

// Selection events type definitions
using SelectionChangedCallback = std::function<void(std::shared_ptr<GameObject>)>;

/**
 * @brief Manages selection state across the editor
 * 
 * This singleton class maintains the currently selected GameObject and notifies
 * listeners when the selection changes.
 */
class SelectionManager {
public:
    // Get the singleton instance
    static SelectionManager& GetInstance();
    
    // Delete copy constructor and assignment operator
    SelectionManager(const SelectionManager&) = delete;
    SelectionManager& operator=(const SelectionManager&) = delete;
    
    // Set the currently selected object and notify listeners
    void SetSelectedObject(const std::shared_ptr<GameObject>& object);
    
    // Get the currently selected object
    [[nodiscard]] std::shared_ptr<GameObject> GetSelectedObject() const;
    
    // Clear the current selection
    void ClearSelection();
    
    // Register a listener to be notified when selection changes
    void AddSelectionChangedListener(const SelectionChangedCallback& callback);
    
    // Remove a previously registered listener
    void RemoveSelectionChangedListener(const SelectionChangedCallback& callback);

private:
    // Private constructor for singleton
    SelectionManager() = default;
    
    // Currently selected object
    std::shared_ptr<GameObject> m_SelectedObject = nullptr;
    
    // List of registered listeners
    std::vector<SelectionChangedCallback> m_Listeners;
    
    // Notify all listeners of a selection change
    void NotifyListeners();
};

#endif // SELECTION_MANAGER_H