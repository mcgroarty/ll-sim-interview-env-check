<!-- README.md - how to run the pre-interview environment check, plus requirements and troubleshooting. -->

# Environment check

Run this before your interview. It confirms your setup so interview time
goes to the problem, not tooling. It is **not** the interview exercise:
you'll get that at the start of the interview.

## Requirements

Please have these installed and working:

- **C++17 compiler:** GCC 9+, Clang 10+, or Xcode Command Line Tools on macOS (`xcode-select --install`).
- **CMake 3.16+**
- **Python 3.6+** (for the check script)
- **telnet or nc:** macOS no longer ships telnet, so run `brew install telnet` or just use `nc`.
- **git**
- **Windows:** use WSL2 (Ubuntu is fine). The code uses POSIX sockets and won't build natively. WSL2 runs natively on ARM64 Windows too. If WSL2 isn't an option, use any Linux VM or a remote Linux/macOS machine you can screen-share from.
- **An editor or IDE you can debug in**
- **Your AI coding tool:** installed, signed in, and working.

## Run it

1. Clone this repo and `cd` into it.
2. Build:

       cmake -B build && cmake --build build

3. Start the program. It prints compiler info, then waits for one connection:

       ./build/env_check

4. In a second terminal, run the check:

       python3 scripts/check.py

   You should see `environment check passed`, and the program exits.
5. Manual test: start `./build/env_check` again, then connect with
   `telnet localhost 7777` or `nc localhost 7777`, type a line, and press
   Enter. The same line should come back, and the program exits.

Both take an optional port argument, e.g. `./build/env_check 7788` and
`python3 scripts/check.py 7788`.

## Troubleshooting

- **"bind: Address already in use":** something else is on port 7777. Check with
  `lsof -i :7777` (macOS/Linux) or `ss -ltnp | grep 7777` (Linux), or pass a different port to both commands.
- **macOS has no telnet:** `brew install telnet`, or use `nc localhost 7777`.
- **Windows:** run everything inside WSL2 (`wsl --install`, then work in the Ubuntu shell). The program uses POSIX sockets and won't build natively.
- **CMake too old:** `cmake --version` must show 3.16 or newer. Upgrade with
  `brew install cmake`, `pip install cmake`, or your package manager.
- **Compiler says C++17 isn't supported:** update GCC (9+), Clang (10+), or Xcode Command Line Tools.

## Final checklist

- [ ] Project builds with no errors
- [ ] `scripts/check.py` prints `environment check passed`
- [ ] Manual telnet/nc echo works
- [ ] You can set a breakpoint and debug `env_check` in your editor/IDE
- [ ] Your AI coding tool is installed, signed in, and working
- [ ] Screen sharing tested with your editor, terminal, and AI tool visible
