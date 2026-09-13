# Unite — The Ashen Bell

A playable C++ / raylib two-player boss prototype with ENet online co-op.
The host plays **Warden**; the joining player plays **Archer**.
All visuals are original procedural placeholder shapes drawn by raylib: no paid assets,
sprite downloads, accounts, or backend services are needed.

Your existing archer prototype remains the `unite` executable, with its source files
unchanged. The new game is `unite_boss`, in `boss/`.

## First encounter

The Bellkeeper has 100 health and an armored core.

1. The gold slam circle locks onto the Warden's position.
2. Stay inside the circle and tap **E** just before its countdown reaches zero.
   Parry lasts **0.28 seconds**; holding the button does not repeat it.
3. A successful parry exposes the blue core for **1.6 seconds**.
4. The Archer holds **left mouse / E** for at least **0.65 seconds**, aims at the
   boss, then releases during the opening. Start charging before the parry.
5. A correctly aimed charged shot does **25 damage**, consuming the opening.
   Four successful coordinated attacks win.
6. After recovery, red ashfall circles lock onto both players. Move out or dodge.
   Below half health, windups and ashfall become faster.

Both players have three health. Either player's death ends the attempt.
The host can press **R** at any time to restart. A successful dodge avoids damage
but does not expose the core. The Warden must actually be inside the slam radius
to parry it. There is no facing requirement for this first prototype.

| Control | Action |
| --- | --- |
| WASD | Move |
| Mouse | Aim (Archer) / stationary dodge direction |
| E or left mouse | Warden: tap to parry; Archer: hold to charge, release to fire |
| Space | Dodge; 0.22 seconds invulnerable, 0.85 seconds cooldown |
| R | Host restarts the stage |
| F1 | Toggle player hitbox outlines |
| Escape | Quit |

The Archer's shot is instant for this prototype. Dodge movement is faster than
normal movement. Window focus loss clears held inputs.

## Dependencies and licensing

- C++20 compiler, CMake 3.20+, Git.
- [raylib](https://github.com/raysan5/raylib), zlib license.
- [ENet](https://github.com/lsalzman/enet), MIT license.
- Python 3 only for the optional network smoke test.

CMake uses installed raylib if available; otherwise it fetches raylib 5.5.
ENet 1.3.18 is fetched at commit
`2662c0de09e36f2a2030ccc2c528a3e4c9e8138a`.
The first configure needs internet access. Later builds use the cached sources.
Keep the dependencies' license notices when distributing a build; their license
files are in the fetched source directories under `build/_deps/`.

## Ubuntu / this WSL folder

Run these commands **inside an Ubuntu terminal**:

```bash
sudo apt update
sudo apt install -y build-essential cmake git python3 \
  libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev \
  libglu1-mesa-dev libxcursor-dev libxinerama-dev
cd ~/dev/unite
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
./build/boss/unite_boss --practice
```

Your WSL installation already had raylib 6.0 installed when this was developed.
The build accepts that installed package. WSL graphics require a working WSLg
session; if no window opens, test from the Ubuntu terminal and check `echo $DISPLAY`.
Headless tests do not require a graphics session.

## Native Windows

Install Git, CMake, and Visual Studio Community with **Desktop development with C++**
(the free Community edition is subject to Microsoft's eligibility terms).
Use a **native Windows checkout** and an x64 Native Tools Command Prompt:

```bat
cmake -S . -B build-win -A x64
cmake --build build-win --config Debug
build-win\boss\Debug\unite_boss.exe --practice
```

Do not reuse a WSL build directory for Windows builds. For a first internet
playtest on Windows, a native Windows build is simpler to expose than WSL's
additional virtual network layer. Native Windows has not been validated in this
workspace; the source and ENet dependency support both platforms.

## Play with two clients on one computer

In two Ubuntu terminals:

```bash
# Terminal 1: Warden / host
./build/boss/unite_boss --host

# Terminal 2: Archer / client
./build/boss/unite_boss --join 127.0.0.1
```

The boss waits until the Archer connects. Use this to check connection and rendering;
only the focused window receives keyboard/mouse input. For solo gameplay testing:

```bash
./build/boss/unite_boss --practice
```

Practice runs locally without opening a network socket. The bot charges, shoots
during openings, and moves away from ashfall. You control the Warden's parry timing.

## Play over LAN or the internet

Both players should build the same revision.

1. Host runs `unite_boss --host` (default **UDP 7777**).
2. Allow the executable / UDP port through the host's firewall.
3. On a LAN, the partner joins the host's LAN IPv4 address:
   `unite_boss --join 192.168.1.50` (replace the example).
4. Over the internet, forward **UDP 7777** on the host's router to the host
   computer's LAN address, then have the partner join the router's public IPv4.
   Reserve the host's LAN address so the forwarding target stays valid.
5. To change ports, pass e.g. `--port 7788` to both processes and forward that port.

Use `ipconfig` on Windows or `ip -4 addr` on Linux to find local addresses.
`127.0.0.1` only reaches the same computer/network namespace.

There is **no automatic NAT traversal, matchmaking, relay, or lobby service**.
CGNAT / some double-NAT connections will prevent ordinary port forwarding:
use a mutually reachable private network/VPN or a host with an accessible public
address. A VPN/service may have separate usage terms or costs.

WSL2 commonly adds its own NAT between Ubuntu and Windows. Forwarding to Windows
alone may not reach the Linux UDP socket; Windows TCP `portproxy` is not a UDP fix.
Use native Windows hosting for the easiest Windows setup, or explicitly configure
WSL networking and firewall routing for your machine.

A disconnect pauses the host in the waiting state; a new partner starts a fresh
encounter. The disconnected client must relaunch to reconnect. Only one partner
is accepted. The protocol has no authentication or encryption: this is a
friends-only prototype, not a public competitive service.

## Fast development loop

```bash
cmake --build build --target unite_boss -j
./build/boss/unite_boss --practice
```

- Edit `boss/simulation.hpp` for timings, movement, damage, boss phases, and bot logic.
- Edit `boss/main.cpp` for input, HUD, arena, and placeholder character drawing.
- Edit `boss/protocol.hpp` for explicit versioned packet serialization/validation.
- Edit `boss/network.hpp` for ENet transport and connection handling.
- Update `wire::Magic` on incompatible protocol changes; rebuild both players.

Simulation runs at a fixed 60 Hz. The host owns movement, boss logic, health,
cooldowns, and combo decisions. The client sends reliable ordered input on channel
0; snapshots use sequenced unreliable packets on channel 1 at 30 Hz. Inputs include
held action state, so the host detects press/release transitions. State is serialized
explicitly, not by copying raw C++ structs. Packets are size/range/version checked.

The client smooths displayed positions but **does not predict movement or rewind
combat**. You will feel round-trip latency on the Archer, and packet loss can delay
reliable inputs. This implementation establishes the co-op mechanic and networking
baseline; it is not finished action-game netcode. Test with your actual partner
before tightening the timing window. Host-side timers remain authoritative.

There is no audio, save system, gamepad support, progression, or finished sprite
animation in this first stage.

## Checks

```bash
cmake --build build -j
ctest --test-dir build --output-on-failure
python3 boss/smoke_test.py build/boss/unite_boss
```

The first test checks timed parries, charged/uncharged/missed shots, armor, dodging,
victory/defeat, expired openings, and malformed packet handling.
The Python test launches real headless host/client processes on UDP 17777,
then checks connection, client-driven movement, and replicated state.
Pass a different test port as a second argument if needed.

Headless mode is a test harness (the client moves right automatically), not a
dedicated production server:

```bash
./build/boss/unite_boss --host --headless --seconds 10
```

Manual two-person check: verify waiting, connection, one successful combo,
failed parry damage, ashfall dodge, victory, death/restart, and partner disconnect.

## Validation in this workspace

Built both executables with GCC 13.3 and installed raylib 6.0 on Ubuntu/WSL.
Simulation/protocol tests and the real ENet localhost smoke test passed.
Launched the graphical practice mode and inspected its arena screenshot.
Native Windows and two-machine internet play remain untested.

For a reproducible screenshot, run from the build directory:

```bash
cd build
./boss/unite_boss --practice --seconds 2 --capture
```

This writes `boss-preview.png` in the current directory.
