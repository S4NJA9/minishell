# Matrix Mini Shell (msh)

A lightweight UNIX-style command-line shell written in **C** for Linux. Matrix Mini Shell is a learning project focused on understanding Linux system programming, process management, command execution, inter-process communication, and job control.

The project provides a Matrix-inspired terminal interface and is being developed incrementally toward a feature-rich mini shell.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Requirements](#requirements)
- [Build and Run](#build-and-run)
- [Usage Examples](#usage-examples)
- [Project Structure](#project-structure)
- [Linux Concepts](#linux-concepts)
- [Development Roadmap](#development-roadmap)
- [Author](#author)
- [License](#license)

## Overview

A UNIX shell acts as an interface between the user and the operating system. It reads commands, interprets input, starts programs, and manages their execution.

This project implements a simplified shell in C using Linux system calls and standard library functions. It is intended to build practical knowledge of how shells work internally.

## Features

### Implemented

- [x] Matrix-style green startup banner and terminal prompt
- [x] Display the current working directory
- [x] Display the current time in the prompt
- [x] Customize the prompt using `PS1=`
- [x] Execute external programs using `fork()` and `execvp()`
- [x] Wait for foreground child processes using `waitpid()`
- [x] Execute built-in commands:
  - `cd` — change the working directory
  - `pwd` — display the current working directory
  - `echo` — print text
  - `exit` — terminate the shell
- [x] Display the previous command's exit status in the prompt
- [x] Support pipelines using the `|` operator

### Planned

- [ ] Default prompt in the required `msh>` format
- [ ] Expand special variables: `$?`, `$$`, and `$SHELL`
- [ ] Forward Ctrl+C (`SIGINT`) appropriately
- [ ] Handle Ctrl+Z (`SIGTSTP`) for foreground processes
- [ ] Run commands in the background using `&`
- [ ] Implement `jobs`, `bg`, and `fg`
- [ ] Track and clean up completed background processes
- [ ] Handle child termination using `SIGCHLD`
- [ ] Improve command parsing and error handling
- [ ] Support and test pipelines with multiple commands

> Features in the planned list are not yet claimed as implemented.

## Requirements

- Linux operating system or a Linux environment such as WSL
- GCC compiler
- GNU Make (optional; if a Makefile is added)
- Git (for version control)

## Build and Run

### 1. Clone the repository

Replace the placeholder URL with your GitHub repository URL.

```bash
git clone YOUR_GITHUB_REPOSITORY_URL
cd minishell
```

### 2. Compile the project

Run the following command from the project directory:

```bash
gcc -Wall -Wextra main.c shell.c input.c execute.c commands.c -o minishell
```

### 3. Start the shell

```bash
./minishell
```

**Note:** The program currently loads external command names from `external_command.txt`. Run the shell from the project directory so that the file can be found.

## Usage Examples

### Built-in commands

Display the current directory:

```bash
pwd
```

Change directory:

```bash
cd /tmp
```

Print a message:

```bash
echo Hello World
```

Exit the shell:

```bash
exit
```

### External commands

List files:

```bash
ls -l
```

Display the number of entries:

```bash
ls | wc -l
```

### Customize the prompt

```bash
PS1=matrix$
```

The prompt text changes while the shell continues to display the time and working directory.

### Command exit status

The prompt displays the status of the previous command:

- `0` — command succeeded
- A nonzero value — command failed

For example, an unknown command currently returns status `127`.

## Project Structure

```text
minishell/
├── main.c
├── shell.c
├── shell.h
├── input.c
├── input.h
├── execute.c
├── execute.h
├── commands.c
├── commands.h
├── external_command.txt
├── .gitignore
└── README.md
```

| File | Responsibility |
|---|---|
| `main.c` | Entry point of the application |
| `shell.c` | Startup banner, prompt, and main input loop |
| `shell.h` | Shell function declarations |
| `input.c` | Splits input into command arguments |
| `input.h` | Input parsing declarations |
| `execute.c` | Built-in execution, process creation, and pipelines |
| `execute.h` | Execution function declarations |
| `commands.c` | Identifies built-in and external commands |
| `commands.h` | Command definitions and declarations |
| `external_command.txt` | Recognized external command names |
| `.gitignore` | Excludes generated files from Git |
| `README.md` | Project documentation |

## Linux Concepts

This project explores the following Linux programming concepts:

| Concept | Purpose |
|---|---|
| `fork()` | Create a child process |
| `execvp()` | Execute an external program |
| `waitpid()` | Wait for a child process and inspect its status |
| `pipe()` | Create a communication channel between processes |
| `dup2()` | Redirect standard input or output |
| `getcwd()` | Retrieve the current working directory |
| `chdir()` | Change the current working directory |
| Signals | Handle events such as Ctrl+C and Ctrl+Z |
| Job control | Manage foreground and background processes |
| Exit status | Determine whether a command succeeded or failed |

## Development Roadmap

The project is being developed in stages:

1. **Shell fundamentals** — input parsing, prompt, and built-in commands.
2. **Process management** — external commands, `fork()`, `execvp()`, and `waitpid()`.
3. **Pipelines** — connect multiple processes using `pipe()` and `dup2()`.
4. **Signals** — handle Ctrl+C and Ctrl+Z correctly.
5. **Background execution** — support `&` and multiple background jobs.
6. **Job control** — implement `jobs`, `bg`, and `fg`.
7. **Shell variables** — support `$?`, `$$`, and `$SHELL`.
8. **Testing and documentation** — verify error handling and document supported behavior.

## Author

**S4NJA9**

- GitHub: [S4NJA9](https://github.com/)

## License

This project is developed for educational purposes. A formal open-source license can be added when one is selected.

