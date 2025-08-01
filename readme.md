# Soccer Robot Framework for HTWK-Leipzig

## Architecture

In this architecture, the main components are the Agents. The team defines many of Agents, each with its own
responsibilities. The Agents communicate with each other through a Message Broker, which allows them to send and receive
messages asynchronously. But can also call methods on each other for synchronous communication.

This framework helps creating following features:

1. Define fields for Agents to store data.
2. Define Events Agents can emit and subscribe to.
3. Define Methods Agents can call on each other.
4. Define the behavior of Agents in response to events and method calls.
5. Define a standard behavior for Agents, that they will run when they are started.
6. Debugging and Logging features to track the behavior of Agents.

Each Agent has its own mailbox. Events are queued in the mailbox. The mailbox will be sorted by priority so the most
important events are processed first. The Agent will process the events in the mailbox in a loop, until the mailbox is
empty.

## Data Flow

1. Each Agent can poll data from other Agents by accessing their fields.

2. They can also emit events, which other Agents can subscribe to.
   That allows for a flexible and dynamic communication between Agents.

3. Agents can also call methods on each other, which allows for synchronous communication.

## Agent

An Agent is a class that represents an entity in the system.

You can define them as singletons, so that only one instance of the Agent exists in the system.
But you can also define them as multiple instances, so that each instance has its own state.

In order to access the Agent instance you can Use the static methods of you Agent class to get the instance or create a
new one.

For each agent you can define:

- Fields: These are the data that the Agent can store and access.
- EventHandler: These are functions that are called when an event is emitted.
- Methods: These are functions that can be called on the Agent by other Agents.
- StartBehavior: This is a function that is called when the Agent is started. It can be used to use an Agent as a
  Function that processes something finishes and then releases it self.
- Number of Instances: This defines how many instances of the Agent should be created.
- Mailbox Size: This defines how many events can be queued in the mailbox of the Agent.
- Priority: This a table that maps the event emitter ID to a priority value. The higher the value, the higher the
  priority of the event.
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


## General
Always check if pointers are null befor using them.
Add asserts to your code to ensure that everything is as expected. `<assert.h>`
Use the `assert` macro to check for conditions that should never happen. This will help you catch bugs early in the development.
Assertions are not checked in release builds, so you can use them to check for conditions that should never happen in production.

## Define an Agent

### Define Fields

Watch out for race conditions when accessing fields from multiple Agents. To avoid issues use...

### Define Methods

#### Init Method

Function to call after the Agent is created. But before the Agents Event threads (main and event listener) are started.
This method is called synchronously. So time consuming operations will slow down the agent creation process.

#### Main Method

Be aware that the main method is called in another thread, that can take some time to start.
This method is called asynchronously. So you can do time consuming operations here without blocking the Agent creation.

You probably want to add breakpoints to the main method, such as `boost::this_thread::interruption_point();`. So you can 
cancel the thread if you want to stop the Agent. You need to do this when you use a infinite loop in the main method. 
Otherwise the Agent will not be disposed.
         `
#### Custom Methods

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

#### this can be changed during runtime:

```c++
class FreshAgent: public Agent<FreshAgent>
{
    SET_POOL_SIZE_LIMIT(100) // Set the maximum number of instances to 100
};
// later

FreshAgent::max_pool_size = 50; // Change the maximum number of instances to 50
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

To discard an Agent call kill(), this will unregister every resources so the smart pointer will release the agent.

```c++
auto agent = MyAgent::spawnNewAgent();
agent.kill() // Releases all resources and the object becomes unusable and will crash your program.
```

## Define Events

## Subscribe to Events in Agents
when subscribing to an event and you use the this keyword, make sure to check that it will not be null. Or use the this_from_shared_ptr() method to get a shared pointer to the Agent instance.

            
```c++
// BAD !!
on<MyEvent>([this](auto event) {
    // this could be nullptr
    this->antwort_auf_alles = event->antwort_auf_alles;
});
         
// GOOD !!

on<MyEvent>([this](auto event) {
    if (!this) {
        // Handle the case where this is nullptr
        return;
    }
    this->antwort_auf_alles = event->antwort_auf_alles;
}); 

// or use the this_from_shared_ptr() method


on<MyEvent>([this_ptr = shared_from_this()](auto event)
{
    assert(this_ptr != nullptr && "this should not be null in on<MyEvent>()");
    this_ptr->antwort_auf_alles = event->antwort_auf_alles;
});

```

## Emit Events
## Creating Backends

## Call Methods on Agents

## Load Balancing

If there are multiple instances of an Agent, and you dont care which instance processes the event, you can grab any:

```c++
class RoundRobinTestAgent : public Agent<RoundRobinTestAgent>
{
    inline static int max_pool_size = 7;
};

// later ----

auto a = RoundRobinTestAgent::spawnNewAgent();
auto b = RoundRobinTestAgent::spawnNewAgent();

auto agent1 = RoundRobinTestAgent::pool.getAgentLoadBalanced();
 ```

## Distribution of Agents across multiple machines

## Debugging and Logging

## Writing Unit tests

# Decisions

- We use boost:thread instead of std::thread, because boost:thread allows to interrupt threads, which is necessary for
  the Agent framework to work properly.
- We use ZeroMQ as Message Broker, because it handles the asynchronous communication between Agents.
- The Agent Class is more like Interface for the user to easly interact with it. What spawns the Agent and will handle 
- the liftime is the Pool. There it will be Possible to spawn new Agents on Other machines. Passing the methods manually is a Design Decision. You could derive from Pool<T> but this is really hard to debug 
because you cant see the pool. Also it become impossible to hide some functions from the pool to the user. So Pool<T> will be a private static member of the Agent class.
- 
- Should i follow the pattern
```c++
class MyAgent : public Agent<MyAgent>
{
    void init(){
        on<MyCoolEvent>([](const MyCoolEvent& event) {
            // Handle the event
        })
        emit<MyCoolEvent>(MyCoolEvent{ /* event data */ });
        
    }
};
```
or should i follow the pattern

```c++
class MyAgent : public Agent<MyAgent>
{
    void init(){
        MyCoolEvent.subscribe(this,[](const MyCoolEvent& event) {
            // Handle the event
        })
        MyCoolEvent.emit(this,MyCoolEvent{ /* event data */ });
        
    }
};

```

- I choose the first pattern because it is more readable and easier to understand.

- An event is a message that can be sent over different backends. So ill follow some kind of a plugin architecture. 
Where per Event the developer can choose the backend to use. The Question if the backend should be static class or 
should be an instance. The downside of a instance is that its not that easy to pass to the Event definition. The upside is that its more flexible and 
can be changed at runtime and is easier to test, and easer to extend.
1. Static class
```c++

class MyEvent : public Event<MyEvent, LocalBackend>
{
};


class MyEvent : public Event<MyEvent, ZmqBackend>
{
};

```
2. Instance
```c++ 
class MyEvent : public Event<MyEvent>
{
    inline static Backend backend = LocalBackend::getInstance();
    
};

class MyEvent : public Event<MyEvent>
{
    inline static Backend backend = ZmqBackend::getInstance();
    
};
```
3. ZeroMQ as default backend if and only if it is faster then hard wireing and directly calling the methods


---
4. There will be no feature for defining multiple Events for one on<>() clause because, you wouldnt be able to convert the payload, it could be possible if you would accept void as input parameter but this is really rare.
   1. not planned : specify backends and Events, all events are first, then there will be the bagends like Event+Event*+Backend*