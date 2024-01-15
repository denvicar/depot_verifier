# Depot verifier
Steam manifest files verifier written in C++. To build it's needed to link openssl and protobuf dlls.

## Usage
You have to configure depot_verifier.ini with the 3 directories needed:
- AcfDir: directory that contains the acf(s) of the games to verify
- ManifestDir: directory that contains the manifest(s) needed to verify the game files
- GameDir: directory that contains the game directories. The names are expected to be equal to "installdir" in the acf file

You can then use the script by running:
```
depot_verifier.exe appid [appid...]
```
You can use multiple appids and the script will fetch the corresponding acfs, find the manifests and verify each file using the sha1 digest.
