# Soccer Robot Framework for HTWK-Leipzig
## Architecture
In this architecture, the main components are the Agents. The team defines many of Agents, each with its own responsibilities. The Agents communicate with each other through a Message Broker, which allows them to send and receive messages asynchronously. But can also call methods on each other for synchronous communication.

This framework helps creating following features:
1. Define fields for Agents to store data.
2. Define Events Agents can emit and subscribe to.
3. Define Methods Agents can call on each other.
4. Define the behavior of Agents in response to events and method calls.
5. Define a standard behavior for Agents, that they will run when they are started.
6. Debugging and Logging features to track the behavior of Agents.

Each Agent has its own mailbox. Events are queued in the mailbox. The mailbox will be sorted by priority so the most important events are processed first. The Agent will process the events in the mailbox in a loop, until the mailbox is empty.

## Data Flow
1. Each Agent can poll data from other Agents by accessing their fields.

2. They can also emit events, which other Agents can subscribe to. 
That allows for a flexible and dynamic communication between Agents.

3. Agents can also call methods on each other, which allows for synchronous communication.

## Agent

An Agent is a class that represents an entity in the system.

You can define them as singletons, so that only one instance of the Agent exists in the system.
But you can also define them as multiple instances, so that each instance has its own state.

In order to access the Agent instance you can Use the static methods of you Agent class to get the instance or create a new one.

For each agent you can define:
- Fields: These are the data that the Agent can store and access.
- EventHandler: These are functions that are called when an event is emitted.
- Methods: These are functions that can be called on the Agent by other Agents.
- StartBehavior: This is a function that is called when the Agent is started. It can be used to use an Agent as a Function that processes something finishes and then releases it self.
- Number of Instances: This defines how many instances of the Agent should be created.
- Mailbox Size: This defines how many events can be queued in the mailbox of the Agent.
- Priority: This a table that maps the event emitter ID to a priority value. The higher the value, the higher the priority of the event.
- 

## Messages
  A message is an Event that is sent from one Agent to another that has Subscribed to it.
...

# Tutorial
## Install
Requirements:
- vcpkg (opensourcve C/C++ Packagemanager)
- cmake

### Windows
### Linux
### NixOS
## Define an Agent
### Define Fields
Watch out for race conditions when accessing fields from multiple Agents. To avoid issues use...

### Define Methods
### Define Start Behavior

### Singleton or Multiton
To specify if a Agent should have multiple instances you can specify the max_pool_size like:
Singleton (Default):
```c++

class FreshAgent: public Agent<FreshAgent>
{
};
```
Multiton:
```c++

class FreshAgent: public Agent<FreshAgent>
{
    SET_POOL_SIZE_LIMIT(100)
};
```

### Define Mailbox Size
### Define Priority
### Define Deconstructor
To release Resources you have allocated, make sure to release them in the Deconstructor like:
```c++
class MyAgent : public Agent<MyAgent>
    {
        ~MyAgent() override
        {
            //Release Ressources
        }
    };
```

### Make Sure to handle lifetime of Agents
To release Resources you have allocated, make sure to release them in the Deconstructor like:
```c++
auto agent = MyAgent::spawnNewAgent();
agent.kill() // Releases all resources and the object becomes unusable and will crash your program.
```

## Define Events
## Subscribe to Events in Agents
## Emit Events
## Call Methods on Agents

## Load Balancing

## Distribution of Agents across multiple machines

## Debugging and Logging
## Writing Unit tests


# Decisions

- We use boost:thread instead of std::thread, because boost:thread allows to interrupt threads, which is necessary for the Agent framework to work properly.
- We use ZeroMQ as Message Broker, because it handles the asynchronous communication between Agents.