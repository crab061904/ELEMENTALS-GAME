# ELEMENTALS-GAME
Markdown

# ELEMENTAL: Card Battle Game

A hybrid multiplayer game project containing a **Unity Client** (C#) and a custom **Game Server** (C).

## 📂 Repository Structure

This repository is divided into two main sections:

### 1. The Unity Project (Root Directory)
The root folder contains the Unity game engine files.
* **`Assets/`**: Contains all scripts, scenes, and game resources.
* **`ProjectSettings/`**: Unity configuration files.
* **`Packages/`**: Unity package dependencies.

### 2. The Server Backend (`elemental-card-battle-upgraded/`)
Located inside the `elemental-card-battle-upgraded` folder, this contains the raw C code for the game server.
* **`server/`**: Core game logic and TCP socket handling.
* **`data/`**: Configuration files (e.g., `skills.json`).
* **`client/`**: A terminal-based testing client.
* **`Makefile`**: Build script for compiling the C code.

---

## ⚠️ Prerequisites (Windows Users)

The game server is written in C and uses Linux-specific networking libraries. **You cannot run the server in the standard Windows Command Prompt.**

### Step 1: Install WSL (Windows Subsystem for Linux)
To run the server on Windows, you must install WSL. Follow these steps:

1.  Open **PowerShell** as Administrator.
2.  Type the following command and press Enter:
    ```powershell
    wsl --install
    ```
3.  **Restart your computer** when the command finishes.
4.  After restarting, the **Ubuntu** terminal window should open automatically. Follow the prompts to create a username and password.

### Step 2: Install Compilers
Once Ubuntu is set up, you need to install the C compilers. Run this command inside your new Ubuntu terminal:

```bash
sudo apt update && sudo apt install build-essential
Type y and press Enter if asked to confirm.

🚀 How to Run the Game
1. Start the Server (Backend)
Do not use the Unity Console for this. You must use your Ubuntu/WSL Terminal.

Navigate to the project folder inside Ubuntu: (Note: Your C: drive is located at /mnt/c/ in WSL)

Bash

cd /mnt/c/Users/YOUR_USERNAME/Documents/ELEMENTAL/elemental-card-battle-upgraded
Compile the code:

Bash

make
Run the server:

Bash

./battle
2. Start the Game (Unity)
Ensure the server (./battle) is running in your terminal.

Open Unity Hub.

Add/Open this repository folder.

Press Play in the Unity Editor.

🛠️ Configuration
You can modify the card stats and skills without changing the code.

Edit elemental-card-battle-upgraded/data/skills.json to balance the game.

Created by [Your Name]


### Next Step
After you save this file, you need to push this change to GitHub so it appears on the websMarkdown

# ELEMENTAL: Card Battle Game

A hybrid multiplayer game project containing a **Unity Client** (C#) and a custom **Game Server** (C).

## 📂 Repository Structure

This repository is divided into two main sections:

### 1. The Unity Project (Root Directory)
The root folder contains the Unity game engine files.
* **`Assets/`**: Contains all scripts, scenes, and game resources.
* **`ProjectSettings/`**: Unity configuration files.
* **`Packages/`**: Unity package dependencies.

### 2. The Server Backend (`elemental-card-battle-upgraded/`)
Located inside the `elemental-card-battle-upgraded` folder, this contains the raw C code for the game server.
* **`server/`**: Core game logic and TCP socket handling.
* **`data/`**: Configuration files (e.g., `skills.json`).
* **`client/`**: A terminal-based testing client.
* **`Makefile`**: Build script for compiling the C code.

---

## ⚠️ Prerequisites (Windows Users)

The game server is written in C and uses Linux-specific networking libraries. **You cannot run the server in the standard Windows Command Prompt.**

### Step 1: Install WSL (Windows Subsystem for Linux)
To run the server on Windows, you must install WSL. Follow these steps:

1.  Open **PowerShell** as Administrator.
2.  Type the following command and press Enter:
    ```powershell
    wsl --install
    ```
3.  **Restart your computer** when the command finishes.
4.  After restarting, the **Ubuntu** terminal window should open automatically. Follow the prompts to create a username and password.

### Step 2: Install Compilers
Once Ubuntu is set up, you need to install the C compilers. Run this command inside your new Ubuntu terminal:

```bash
sudo apt update && sudo apt install build-essential
Type y and press Enter if asked to confirm.

🚀 How to Run the Game
1. Start the Server (Backend)
Do not use the Unity Console for this. You must use your Ubuntu/WSL Terminal.

Navigate to the project folder inside Ubuntu: (Note: Your C: drive is located at /mnt/c/ in WSL)

Bash

cd /mnt/c/Users/YOUR_USERNAME/Documents/ELEMENTAL/elemental-card-battle-upgraded
Compile the code:

Bash

make
Run the server:

Bash

./battle
2. Start the Game (Unity)
Ensure the server (./battle) is running in your terminal.

Open Unity Hub.

Add/Open this repository folder.

Press Play in the Unity Editor.

🛠️ Configuration
You can modify the card stats and skills without changing the code.

Edit elemental-card-battle-upgraded/data/skills.json to balance the game.



### Next Step
After you save this file, you need to push this change to GitHub so it appears on the webs