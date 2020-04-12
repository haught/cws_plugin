# CWS output plugin for CHITUBOX
This project is completely based on [NJ3D/CWS-plugin-32bit](https://github.com/JNJ3D/CWS-plugin-32bit) with a goal of allowing non-Windows based operating systems to use the plugin.

## Requirements

To build this project you will need a C++ compiler and the zip utility.

## Building

```console
foo@bar:~$ make
```

## Adding to CHITUBOX
After building you will have a .CHplugin file. 

From CHITUBOX go to **|||->Help->Plugins->Import Plugin** and add the .CHplugin file. After you Slice, you will have the option to export to the CWS format.

## Errata
This has only been tested on MacOS 10.15 and CHITUBOX V1.6.3.