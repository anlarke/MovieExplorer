# Movie Explorer

View information about the movies stored on your computer. (Windows)

## Screenshots

![](https://raw.github.com/anlarke/MovieExplorer/master/screenshots/screenshot1.jpg)

![](https://raw.github.com/anlarke/MovieExplorer/master/screenshots/screenshot2.jpg)

## Download

Please head over to the [Releases Page](https://github.com/anlarke/MovieExplorer/releases).

## Building

The following steps will lead to a successful build (assuming you're working on Windows 7 64-bit with Visual Studio 2010/2013 installed).

1. Download/fork the source

2. Install [boost](http://www.boost.org/) from the provided subdirectory
   - Copy the headers from `boost\boost_1_55_0.7z\boost_1_55_0\boost` to `C:\Program Files (x86)\Microsoft Visual Studio XX.X\VC\include\boost`
   - Copy the libs from `boost\VS2010` or `boost\VS2013` to `C:\Program Files (x86)\Microsoft Visual Studio XX.X\VC\lib`, depending on your Visual Studio version

3. Open `MovieExplorer\VS2010\MovieExplorer.sln` or `MovieExplorer\VS2013\MovieExplorer.sln` depending on your Visual Studio version

4. Hit build
