# Building QuiKShot for Windows 9x

Building QuiKShot for the Windows 9x family of operating systems requires building under Visual Studio 2005 using the ReleaseANSI or DebugANSI configurations. These configurations primarily exist to compile for the Multi-Byte Character Set (MBCS), also known as ANSI, because Windows 9x does not support Unicode. However, they also have other changes for Windows 9x support in mind.

## About ANSI Builds

ANSI builds support Windows 98 out of the box. You can just move the executable to a Windows 98 PC and run it without any additional changes. Windows 95 requires additional modifications to the executable outside of Visual Studio in order to run it.

ANSI builds completely lack support for certain features which are only available on Windows NT, such as WIC codec enumeration or opening the screenshot in an external editor (which takes advantage of file type association lists which are only available since Windows 2000). Such features are often disabled by preprocessor flags, so they are not compiled into the ANSI binary at all. This also means that ANSI builds have limited functionality even when running on Windows NT.

## Windows 95 Support

In order to support Windows 95 when building with Visual Studio 2005, a few other changes must be made to the executable binary.

Visual Studio 2005's C/C++ runtime library links to `IsDebuggerPresent` from the library `kernel32.dll`. This is the only such call from the CRT that is not available on Windows 95, and so it must be patched out. I do this in two steps:

1. Stubbing Out All Call Sites in IDA
2. Replacing the Import Name with a Function That Does Exist in CFF Explorer

### Stubbing Out All Call Sites in IDA

Open up your `quikshot.exe` in IDA and search for `IsDebuggerPresent` in the imports. Go to X-refs for the import, and stub out the call sites where they are present. I do this by replacing the call with `xor eax, eax`, followed by enough `nop` instructions to fill the rest of the bytes comprising the function call.

This step may not be necessary, as the only call sites concern Dr. Watson (the Windows crash debugger), but I prefer to sever the import entirely to make sure that nothing goes wrong.

### Replacing the Import Name with a Function That Does Exist in CFF Explorer

Then, find where `IsDebuggerPresent` is listed under imports from `kernel32.dll` in CFF Explorer, and replace it with the name of an import that does exist. You can just copy the name right above it. The only thing that matters here is removing the name so the module loader doesn't try to load it. Since all references to the import address are severed from the previous step, it doesn't really matter what it is.

### Writing Code with Windows 95 Support in Mind

Windows 95 already supports a lot of the modern Win32 API, so there is not much concern. Very little of QuiKShot's existing code had to be modified to accommodate Windows 95 when support was first introduced. However, there are notable features that Windows 95 does not support that Windows 98 *does*, such as multiple monitors. The easiest way to test for Windows 95's support of a feature is by testing a compiled executable on Windows 95 and making sure that it loads without dynamic linkage errors. If QuiKShot can load at all, then it will probably function.