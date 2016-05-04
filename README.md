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

	Visual Studio 2010:
		- Copy the headers from `boost\boost_1_55_0.7z\boost_1_55_0\boost` to `C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\include\boost`
		- Copy the libs from `boost\VS2010` to `C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\lib`

	Visual Studio 2013:
		- Copy the headers from `boost\boost_1_55_0.7z\boost_1_55_0\boost` to `C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\include\boost`
		- Copy the libs from `boost\VS2013` to `C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\lib`

	Visual Studio 2015:
		- Copy the headers from `boost\boost_1_60_0.7z\boost_1_60_0\boost` to `C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\include\boost`
		- Copy the libs from `boost\VS2015` to `C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\lib`
		*Note: using the 12.0 toolchain so copy to the 12.0 directory (as shown)
		
3. Open `MovieExplorer\VS2010\MovieExplorer.sln`, `MovieExplorer\VS2013\MovieExplorer.sln`, or `MovieExplorer\VS2015\MovieExplorer.sln` depending on your Visual Studio version

4. Hit build
