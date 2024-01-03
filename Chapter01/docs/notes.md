# Importance of Software Architecture and Principles of Great Design

It will focus on the key aspects to keep in mind when designing the architecture of a C++ solution.

## Understanding software architecture

- When you write some codes or project , you have to deal with which component interact with others. When desinging the architecture you have to think big picture. 
  
- On a small scale , every single building looks okay , but they dont combine into sensible  bigger picture.
  
- The software architecture of a system is the set of structures needed to
reason about the system, which comprise software elements, relations
among them, and properties of both.

- Enterprise architecture deals with the whole company or even a group of companies. When thinking about enterprise architecture, ypu should be looking at how all the systems in company behave and cooperate with each other. Its concerned about the alignment between business and IT
- Solution architecture is less abstract than its enterprise counterpart.Usually, solution architecture is concerned with one specific system and the way it interacts with its surroundings. 
- Software arhitecture is even more concrete than solution architecture. It concentrates on a specific project , the technologies it uses and how it interacts with other projects.
- Infrasturucture architecture is , as the name suggests, concerned about the infrasture that software will use. It defines the deployment environment and strategy,how the application will scale , fail handling , site reliability and other.

## Learning the importance of proper architecture

If after several months or even years of development you still want your software to retain its qualities, you need to take some steps earlier in the process.

### Software decay

Software decay, sometimes also
called erosion, occurs when the implementation decisions don't correspond
to the planned architecture. All such differences should be treated as
technical debt.

### Accidental Arhitecture

Generally speaking, if your
software is getting tightly coupled, perhaps with circular dependencies, but
wasn't like that in the first place, it's an important signal to put more
conscious effort into how the architecture looks


### Exploring the fundamentals of good architecture

- Architecture context
- Stakeholders
- Business and technical environments

### Developing architecture using Agile principles

## Reference
1.<https://www.linkedin.com/pulse/software-entropy-continuous-state-decay-dennis-van-wattingen/>