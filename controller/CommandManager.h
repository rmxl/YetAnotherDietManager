#ifndef COMMAND_MANAGER_H
#define COMMAND_MANAGER_H

#include "Command.h"
#include <vector>
#include <memory>

/**
 * Manages the execution and undoing of commands
 */
class CommandManager {
public:
    CommandManager();
    
    // Execute a command and add it to the history
    void executeCommand(const CommandPtr& command);
    
    // Undo the last command
    bool undo();
    
    // Redo the last undone command
    bool redo();
    
    // Check if undo/redo is available
    bool canUndo() const;
    bool canRedo() const;
    
    // Get the command history
    std::vector<CommandPtr> getHistory() const;
    
    // Clear the command history
    void clearHistory();
    
private:
    std::vector<CommandPtr> history;
    std::vector<CommandPtr> redoStack;
};

#endif // COMMAND_MANAGER_H
