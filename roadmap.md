# Todo

## Event Feature
- Message Broker
- Event Bus
- Event Structure
- Workflow for Devs to create events and reuse them
- Autocompletion for available events and typing
- Mailbox for each Agent-Instance

## Backend-Plugin Architecture
- Communication VIA ZmQ
- In Prozess
- Audio Speaker...
- Bluetooth

Channels-Loadbalancer...

## Agent Features
- Singleton
- Multiton

## Logging Feature
- Rerun For Logging

## Handle Garbage Collection of Agents

## Polling Data from instances of Agents
1. Watch out locking and thread safety
2. Distribution on multiple hardware. How to access instance data?

## Distribution on multiple machines
1. Synching state?
2. Load balancing?
   Torwart muss weniger für movement rechnen oder spieler am spiel rand haben Auch ressourcen frei fürs rechnen

##  Auto docs
Static code analysis.
Visualize code. Events and data flow. Help developing...


# Needs Testage (Unit Tests)
## Event Feature
- Message Broker
- Event Bus
- Event Structure
- Workflow for Devs to create events and reuse them
- Autocompletion for available events and typing


## Logging Feature
- Rerun For Logging


# Done

## Threading Feature
- Per Agent-Instance
  - Create Main Worker thread
  - Create Event listener thread (only if events are subscribed)

- Mailbox logic
- Priority Logic

## Priority Feature
- Priority Table for Agents

## Threading Feature
- Per Agent-Instance
  - Create Main Worker thread
  - Create Event listener thread (only if events are subscribed)