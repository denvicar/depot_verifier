# Depot verifier
Steam manifest files verifier written in C++. To build it's needed to link protobuf dlls. The provided binaries are statically linked, so nothing else is required to use them.

## Usage
There are 2 modes of usage:
- Ini configuration: allows checking integrity for multiple games (all those whose appid is specified when executing the command)
- Command line flags: specifying a game directory and the relevant manifests allows for integrity checking of a single game. It's not required to have a depot_verifier.ini in this case.

### Ini configuration
You have to configure depot_verifier.ini with the 3 directories needed:
- AcfDir: directory that contains the acf(s) of the games to verify
- ManifestDir: directory that contains the manifest(s) needed to verify the game files
- GameDir: directory that contains the game directories. The names are expected to be equal to "installdir" in the acf file

You can then use the script by running:
```
depot_verifier.exe appid [appid...]
```
You can use multiple appids and the script will fetch the corresponding acfs, find the manifests and verify each file using the sha1 digest.

### Command line flags
You have to specify in the command line the flags:
- -g/--game: provide the game directory without the trailing forward/backward slash;
- -m/--manifests: provide the manifests to check against

So the command will be:
```
depot_verifier.exe -g "C:\Games\Half Life 2" -m "C:\Manifests\....manifest"
```

## Build
You can use CMake to build the project, just clone the repo:

```
git clone https://github.com/denvicar/depot_verifier.git
cd depot_verifier
git submodule update --init --recursive
```

Then from the root dir run:

```
cmake -S. -Bbuild
cmake --build build
```

Add `-DVCPKG_TARGET_TRIPLET=x64-windows-static` to the first command for a static build.

Refer to Cmake documentation to customize the build type.

## Format

Build the custom format target to use clang-format to format the source code. Clang-format has to be installed on the system.

```
cmake --build build --target format
```
