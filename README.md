# BGL to DSL dictionary converter

Converts **BGL** dictionaries to text files in **DSL** markup format

Based on
- **dictconv**: https://sourceforge.net/projects/ktranslator/files/dictconv/0.2, https://packages.debian.org/buster/dictconv
- **pyglossary**: https://github.com/ilius/pyglossary

## Usage
`bgl_to_dsl Babylon_Dict.bgl` - will create **Babylon_Dict.dsl** dictionary file

## Build
### Windows
- Set path to point to the MinGW binaries folder in `r.bat`
- Run `r b`
- This will create `bgl_to_dsl.exe`

### Linux
- Run `make`
- Use `bgl_to_dsl` binary

## Resources
**MinGW** releases: https://github.com/niXman/mingw-builds-binaries/releases.  
Tested with **x86_64-14.2.0-release-win32-seh-msvcrt-rt_v12-rev1**.
