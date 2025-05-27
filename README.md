# Nicole Compiler

![mascot](images/image.png)

Toy Compiler written in c++ using LLVM 19 by Evian Concepción Peña

Tutor: Casiano Rodríguez León

Cotutor: Jose Rodriguez

## Description
`nicole.sh` is a script that compiles files with the `.nc` extension. If changes have been made to the source code, only the modified files will be recompiled.

## Usage
From the parent directory, run:
```sh
./nicole.sh [[options] input_file] | -t
```
Where `input_file` is the main program file with the `.nc` extension (e.g., `helloWorld.nc`).

## Options
Options can appear in any position except `-n`, which must be followed by the output file name.

- `-h | --help` → Displays a brief description of how to use the compiler.
- `-v | --validate` → Forces the program to follow certain validation rules (recommended).
- `-o | --optimize` → Performs optimizations on the generated code.
- `-n | --name output_file` → Allows specifying the name of the output file (default is `a.out`).
- `-p | --printTree` → Prints the Abstract Syntax Tree (AST) in a directory-like structure.
- `-i | --printIR` → Prints the generated Intermediate Representation (IR) code.

- `-t` → Executes the tests of the compiler, also to run the tests no other argyment but `-t`can be passed to the script.

## Usage Examples
Compile a file with optimization and validation, specifying the output executable name:
```sh
./nicole.sh -v -o -n program_out helloWorld.nc
```

Generate the AST and intermediate code without optimization:
```sh
./nicole.sh -p -i helloWorld.nc
```

## Install using docker
Start the server if needed
```sh
sudo systemctl start docker
```

Move to the docker file folder
```sh
cd dockerInstall
```

Create image, since it is a personal token it won't be shown, replace with yours so you can clone the private repo, this will be possible if you are a collaborator
```sh
sudo docker build --build-arg GITHUB_TOKEN=******* -t mi_proyecto .
```

Create a container and execute an example program, remember that --rm will remove the container once the process is finished
```sh
sudo docker run --rm --name mi_contenedor mi_proyecto ./nicole.sh examples/a.nc -p
```

Remove image
```sh
sudo docker rmi mi_proyecto
```

Stop server and socket(if needed)
```sh
sudo systemctl stop docker
sudo systemctl stop docker.socket
```

## Notes
- Only the file containing the `main` function needs to be specified.
- If changes are made to the code (project), only the modified files will be recompiled.
- `-n` must be followed by the output file name.
- A few examples of the languaje can be found at `examples/`

## Requirements
These versions were used during the development phase but they might not be needed specifically and other versions could work
- `clang 19 or g++ 14` for compilation
- `Catch2 3.7` for test
- `LLVM 19` for code generation
- `cmake 3.30.5` for building the project
- `ccache 4.10.2` (optional)
- `ninja 1.12` (optional) for faster compilation
- `clang-tidy 19` (optional) static analyzer
- `cppcheck 2.7` (optional) static analyzer
- `lcov 2.0.1` (optional) for coverage analysis
- `Docker 28.0.4` (optional) in case that the used operating system is not Linux (Fedora)
- `The project only has been tested in Linux`
- `LLD 19.1.7` (optional) compatible with GNU linkers

## Features Implemented

## Features yet to implement

## Folder structure

## Grammar