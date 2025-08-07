# MuseIO Concert Management System - AI Coding Agent Instructions

## Architecture Overview

MuseIO is a C++ concert management system built on a **template-based modular architecture** with 15 specialized modules. The system uses sophisticated memory management patterns and custom binary serialization for enterprise-grade performance.

### Core Design Patterns

**BaseModule Template System**: All entity modules inherit from `BaseModule<EntityType, IdType>` providing standardized CRUD operations. Each module implements three pure virtual methods:
```cpp
virtual IdType getEntityId(const std::shared_ptr<EntityType>& entity) const = 0;
virtual void loadEntities() = 0;
virtual bool saveEntities() = 0;
```

**Dual-Layer API Pattern**: Each module provides both class-based and namespace-based access:
- `VenueModule` class for direct instantiation
- `VenueManager` namespace with static functions wrapping a singleton instance

**Smart Pointer Hierarchy**: Extensive use of `std::shared_ptr<T>` for entity ownership with `std::weak_ptr<T>` to prevent circular references in bidirectional relationships (e.g., Concert ↔ ConcertTicket).

## Key Development Workflows

### Building & Testing
- **Build Command**: Use VS Code task "C/C++: g++.exe build active file" or manual: `g++ -fdiagnostics-color=always -g file.cpp -o file.exe`
- **Data Persistence**: All entities auto-save to `../../data/*.dat` files using binary serialization
- **Testing Pattern**: Individual test files in `src/testers/` validate each module independently

### Module Development Pattern
1. **Define Model**: Add structs to `models.hpp` with proper enum types
2. **Implement Module**: Inherit from `BaseModule<YourEntity>` 
3. **Add Namespace Wrapper**: Create `YourEntityManager` namespace for simplified access
4. **Binary Serialization**: Implement `loadEntities()` and `saveEntities()` with `readBinary()/writeBinary()` helpers

## Data Model & Relationships

**Central Entity**: `Model::Concert` contains shared_ptrs to all related entities (venue, performers, attendees, etc.)

**Key Enums**: `EventStatus`, `TicketStatus`, `PaymentStatus`, `TaskStatus`, `AttendeeType` - always use enum classes, not raw integers

**DateTime Handling**: Use `Model::DateTime` with `iso8601String` field - call `Model::DateTime::now()` for timestamps

**Memory Safety**: 
- Use `std::weak_ptr<T>` for parent→child references (e.g., `attendee` field in `Feedback`)
- Use `std::shared_ptr<T>` for ownership relationships
- Never use raw pointers except in `AuthModule` for security-critical memory operations

## Security & Authentication

**AuthModule Special Case**: Uses raw pointers and XOR encryption for credential protection:
```cpp
char* secureMemory;           // Direct memory access
unsigned char* encryptionKey; // XOR operations
static constexpr size_t CREDENTIAL_BLOCK_SIZE = 256;
```

**Critical**: AuthModule bypasses smart pointer patterns for security - don't "modernize" this code.

## File Organization

**Headers**: `src/include/*.hpp` - all implementation in headers (template-heavy codebase)
**Tests**: `src/testers/*Test.cpp` - comprehensive test coverage for each module  
**Data**: `data/*.dat` - binary persistence files (relative path `../../data/` from modules)
**Main**: `src/main.cpp` - portal dispatch logic (management vs user interfaces)

## Common Patterns & Conventions

**Search Functions**: Always case-insensitive using `std::transform` + `std::tolower`
**Error Handling**: Return `nullptr` for not found, `false` for operation failure
**ID Generation**: Use `generateNewId()` method (max existing ID + 1)
**Entity Updates**: Optional parameters pattern - empty strings/zero values mean "no change"
**Vector Collections**: Prefer `findByPredicate()` with lambdas over manual iteration

## Integration Points

**UI Layer**: `UIManager` namespace provides pre-formatted console strings
**Cross-Module**: Access via namespace functions (e.g., `VenueManager::createVenue()`)
**Persistence**: Automatic on module destruction, explicit via `saveEntities()`
**Validation**: Entity relationships maintained through shared_ptr navigation

## Performance Considerations

**Binary I/O**: Custom `readString()/writeString()` using length-prefixed format
**Memory Management**: RAII pattern ensures automatic cleanup
**Container Choice**: `std::vector` for collections, `std::unordered_map` for lookups
**Template Specialization**: Module inheritance minimizes code duplication while maintaining type safety

---
*Focus on leveraging the existing template architecture rather than recreating patterns. The system prioritizes type safety and performance over syntactic convenience.*
