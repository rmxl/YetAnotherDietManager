#include "CommandManager.h"

CommandManager::CommandManager() {}

void CommandManager::executeCommand(const CommandPtr& command) {
    command->execute();
    history.push_back(command);
    
    // Clear the redo stack when a new command is executed
    redoStack.clear();
}

bool CommandManager::undo() {
    if (history.empty()) {
        return false;
    }
    
    CommandPtr lastCommand = history.back();
    history.pop_back();
    
    lastCommand->undo();
    redoStack.push_back(lastCommand);
    
    return true;
}

bool CommandManager::redo() {
    if (redoStack.empty()) {
        return false;
    }
    
    CommandPtr command = redoStack.back();
    redoStack.pop_back();
    
    command->execute();
    history.push_back(command);
    
    return true;
}

bool CommandManager::canUndo() const {
    return !history.empty();
}

bool CommandManager::canRedo() const {
    return !redoStack.empty();
}

std::vector<CommandPtr> CommandManager::getHistory() const {
    return history;
}

void CommandManager::clearHistory() {
    history.clear();
    redoStack.clear();
}
