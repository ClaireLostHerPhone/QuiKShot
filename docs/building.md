# Building QuiKShot

If you want to build QuiKShot yourself for a modern version of Windows, then you will need:

- [Visual Studio (links to the download for Visual Studio 2026 Community)](https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=Community&channel=Stable&version=VS18&source=VSLandingPage&passive=false&cid=2500)
- The "Desktop development with C++" workload for Visual Studio.

...and no other dependencies.

Open up the `quikshot-vs2022.sln` file at the root of the repository, right click the solution in the header, and hit build all. This solution file, despite its name, will work on any version after Visual Studio 2022 just fine. It will probably also work on versions before 2022, too. You will need to move all extension DLLs to the extensions folder alongside `QuiKShot.exe`.

## Building for Legacy Operating Systems

In order to target legacy operating systems, such as any version of Windows before... probably 7...? ...then you'll want either an older version of the Visual Studio toolkit with a modern version of Visual Studio (XP+) or Visual Studio 2005. Visual Studio 2005 is my preferred build toolkit for Windows 95 through XP, although its solution should work fine with 2008 and maybe 2003...? (I haven't tested)

When compiling with Visual Studio 2005, use the `quikshot-vs2005.sln` solution file instead. If you want to compile for NT-based operating systems (Windows NT 4.0+), then select either the `Release` or the `Debug` configuration at the top of the Visual Studio window. If you wish to compile for Windows 95, 98, or Me, then select either the `ReleaseANSI` or the `DebugANSI` configuration instead. [See more about building for Windows 9x-based operating systems, including required patches for Windows 95 support, here.](/building_9x.md)