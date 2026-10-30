# Matrix Mini Shell (msh)

A lightweight UNIX-style command-line shell written in **C** for Linux. Matrix Mini Shell is a learning project focused on understanding Linux system programming, process management, command execution, inter-process communication, signals, and job control.

The project provides a simple command-line interface and demonstrates how a basic shell works internally using Linux system calls.

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

This project implements a simplified shell in C using Linux system calls and standard library functions. It is designed to provide practical knowledge of how command-line interpreters work internally.

## Features

### Implemented

- [x] Display a default `msh>` prompt
- [x] Customize the prompt using `PS1=`
- [x] Read and parse user input into command arguments
- [x] Identify built-in and external commands
- [x] Execute external programs using `fork()` and `execvp()`
- [x] Wait for foreground child processes using `waitpid()`
- [x] Execute built-in commands:
  - `cd` — change the working directory
  - `pwd` — display the current working directory
  - `echo` — print text and special variables
  - `exit` — terminate the shell
  - `clear` — clear the terminal
- [x] Support special variables:
  - `$?` — previous command exit status
  - `$$` — shell process ID
  - `$SHELL` — shell executable path
- [x] Handle Ctrl+C using `SIGINT`
- [x] Handle Ctrl+Z using `SIGTSTP`
- [x] Execute commands in the background using `&`
- [x] Manage background jobs using `jobs`
- [x] Resume stopped jobs using `bg`
- [x] Bring jobs to the foreground using `fg`
- [x] Track and clean up completed background processes
- [x] Handle child process notifications using `SIGCHLD`
- [x] Support pipelines using the `|` operator
- [x] Support multiple commands in a pipeline

### Planned Improvements

- [ ] Improve command parsing to support quoted arguments
- [ ] Improve handling of invalid and incomplete pipe syntax
- [ ] Implement robust terminal process-group management
- [ ] Improve signal handling and job-control edge cases
- [ ] Expand command and pipeline testing
- [ ] Improve error handling and memory management

## Requirements

- Linux operating system or a Linux environment such as WSL
- GCC compiler
- GNU Make
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
make clean
make
```

Alternatively, compile manually:

```bash
gcc -Wall -Wextra -g main.c shell.c input.c commands.c execute.c jobs.c -o minishell
```

### 3. Start the shell

```bash
./minishell
```

**Note:** The program loads recognized external command names from `external_command.txt`. Run the shell from the project directory so that this file can be found.

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

Clear the terminal:

```bash
clear
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

The prompt changes to the assigned text.

### Special variables

```bash
echo $?
echo $$
echo $SHELL
```

- `$?` displays the previous command's exit status.
- `$$` displays the shell process ID.
- `$SHELL` displays the shell executable path.

### Background execution

Start a process in the background:

```bash
sleep 30 &
```

Display background jobs:

```bash
jobs
```

Resume a stopped job:

```bash
bg
```

Bring a job to the foreground:

```bash
fg
```

### Pipeline execution

Connect multiple commands using the pipe operator:

```bash
ls | wc
ls | grep minishell
ls | grep minishell | wc -l
echo hello | tr a-z A-Z
```

## Project Structure

```text
minishell/
├── main.c
├── shell.c
├── shell.h
├── input.c
├── input.h
├── commands.c
├── commands.h
├── execute.c
├── execute.h
├── jobs.c
├── jobs.h
├── external_command.txt
├── Makefile
└── README.md
```

| File | Responsibility |
|---|---|
| `main.c` | Entry point of the application |
| `shell.c` | Shell initialization, prompt, signals, and main input loop |
| `shell.h` | Shell function declarations |
| `input.c` | Splits input into command arguments |
| `input.h` | Input parsing declarations |
| `commands.c` | Identifies built-in and external commands |
| `commands.h` | Command definitions and declarations |
| `execute.c` | Command execution, process creation, and pipelines |
| `execute.h` | Execution function declarations |
| `jobs.c` | Background job storage and job-list management |
| `jobs.h` | Job structure and function declarations |
| `external_command.txt` | Recognized external command names |
| `Makefile` | Automates compilation |
| `README.md` | Project documentation |

## Linux Concepts

This project explores the following Linux programming concepts:

| Concept | Purpose |
|---|---|
| `fork()` | Creates a child process |
| `execvp()` | Executes an external program |
| `waitpid()` | Waits for a child process and checks its status |
| `pipe()` | Creates a communication channel between processes |
| `dup2()` | Redirects standard input or output |
| `getcwd()` | Retrieves the current working directory |
| `chdir()` | Changes the current working directory |
| `malloc()` | Allocates memory dynamically |
| Signals | Handle events such as Ctrl+C and Ctrl+Z |
| `SIGCHLD` | Notifies the shell about child process state changes |
| Job control | Manages foreground and background processes |
| Exit status | Indicates whether a command succeeded or failed |

## Development Roadmap

The project was developed in the following stages:

1. **Shell fundamentals** — input parsing, prompt, and built-in commands.
2. **Process management** — external commands, `fork()`, `execvp()`, and `waitpid()`.
3. **Shell variables** — `$?`, `$$`, and `$SHELL`.
4. **Signals** — handling Ctrl+C and Ctrl+Z.
5. **Background execution** — supporting `&` and multiple background processes.
6. **Job control** — implementing `jobs`, `bg`, and `fg`.
7. **Pipelines** — connecting multiple processes using `pipe()` and `dup2()`.
8. **Testing and documentation** — verifying functionality and documenting supported behavior.

## Author

**S4NJA9**

- GitHub: [S4NJA9](https://github.com/S4NJA9)

## License

This project is developed for educational purposes. A formal open-source license can be added when one is selected.