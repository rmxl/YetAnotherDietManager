#ifndef COMMAND_H
#define COMMAND_H

#include <memory>
#include <string>

/**
 * Command interface for implementing the Command pattern for undo operations
 */
class Command {
public:
    virtual ~Command() = default;
    
    // Execute the command
    virtual void execute() = 0;
    
    // Undo the command
    virtual void undo() = 0;
    
    // Get a description of the command
    virtual std::string getDescription() const = 0;
};

// Use shared pointer for managing commands
typedef std::shared_ptr<Command> CommandPtr;

#endif // COMMAND_H
