# 🎮 Project Goldfish (UE5 C++ Game)

**Project Goldfish** is a third-person action game developed in **Unreal Engine 5 using C++**, focusing on AI-driven enemy behavior, custom data structures, and classic algorithms integrated directly into gameplay systems.

The project demonstrates practical usage of **game AI**, **pathfinding**, **behavior trees**, **combat mechanics**, and **custom-built containers** instead of relying solely on STL.

---

## 🚀 Features

- 🧠 **Enemy AI System**
  - Behavior Tree–based logic (chasing, attacking, searching)
  - Dynamic decision-making using custom tasks
- 🗺️ **Pathfinding**
  - A* pathfinding implementation for enemy navigation
- ⚔️ **Combat System**
  - Weapon firing, damage handling, and projectile logic
- 📦 **Custom Data Structures**
  - Custom Hash Map
  - Custom Priority Queue
  - Custom Stack
- 📊 **Algorithms Integrated into Gameplay**
  - Searching algorithms
  - Sorting algorithms
- 🎯 **Enemy Director**
  - Controls enemy spawning and behavior flow
- 🧩 **Pickup & Weapon Components**
  - Modular pickup and weapon systems

---

## 🛠️ Technologies Used

- Unreal Engine 5
- C++
- Behavior Trees & Blackboards
- Object-Oriented Programming
- Game AI Algorithms

---
### Custom Data Structures

* *Custom Hash Map (CustomHashMap):* Implemented to provide *O(1)* average access time for enemy lookups by unique ID, bypassing the overhead of standard array linear searches.

* *Priority Queue (CustomPriorityQueue):* A Min-Heap implementation used to dynamically rank enemies based on "Threat Level" (a calculation of distance to player vs. enemy strength), ensuring the AI system always processes the most critical targets first.

* Custom Stack (CustomStack): A LIFO (Last-In, First-Out) structure utilized by the GameStateManager to handle the Undo/Redo history for game states.

---
## 📁 Project Structure
```bash
Source/
└── project_goldfish/  
    ├── Enemy.*                     # Core enemy logic and behavior  
    ├── EnemyDirector.*             # Manages enemy spawning and control  
    ├── Weapon.*                    # Weapon base logic  
    ├── TP_WeaponComponent.*        # Player weapon component  
    ├── TP_PickUpComponent.*        # Pickup system  
    ├── project_goldfishProjectile.*# Projectile handling  
    ├── AStarPathfinding.h           # A* pathfinding implementation 
    ├── SearchAlgorithms.h           # Searching algorithms  
    ├── SortingAlgorithms.h          # Sorting algorithms  
    ├── CustomHashMap.h              # Custom hash map  
    ├── CustomPriorityQueue.h        # Custom priority queue  
    ├── CustomStack.h                # Custom stack  
    ├── BTT_Attack.*                 # Behavior Tree attack task  
    ├── BTT_ChasePlayer.*            # Behavior Tree chase task  
    ├── BTT_FindPlayerLocation.*     # Behavior Tree search task  
    ├── project_goldfishGameMode.*   # Game mode logic  
    └── Target.cs                    # Unreal build targets  

```

---

## 🤖 AI System Overview

- Enemies use **Behavior Trees** for decision-making.
- Custom **Behavior Tree Tasks** handle:
  - Player detection
  - Chasing logic
  - Attacking behavior
- **A\*** pathfinding ensures efficient navigation toward the player.
- An **Enemy Director** coordinates enemy behavior and flow during gameplay.

---

## 🎮 Gameplay Mechanics

- Player can pick up and use weapons.
- Enemies dynamically react to player position.
- Projectile-based combat system.
- Modular components allow easy extension and experimentation.

---

## 🧪 Educational Focus

This project was built to:
- Strengthen understanding of **game AI**
- Apply **data structures and algorithms** in real-time systems
- Practice **UE5 C++ architecture**
- Explore performance-conscious gameplay design

---

## ▶️ How to Run

1. Clone the repository:
   ```bash
   git clone https://github.com/Uzair-32/DSA_Project
2. Open the project in Unreal Engine 5  
3. Generate Visual Studio project files  
4. Build and run from the editor

