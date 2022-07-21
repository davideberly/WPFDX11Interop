## About ##

This repository contains code for interoperability between Windows
Presentation Foundation (which is based on Direct3D 9) and Direct3D 10
or Direct3D 11. Portions of the code are modifications of the Microsoft
sample code [WPFDXInterop](https://github.com/Microsoft/WPFDXInterop).
The modified files reference the Microsoft license terms (The MIT
License). The other files reference the Boost license.

The application code does nothing interesting other than recreating
the render target and view when a WPF window resize occurs. The render
target shares the WPF back buffer. The render target is cleared. The
goal is to provide the skeleton framework for WPF-D3D10/11
interoperability. You can be creative and fill in the application code
as necessary for your own rendering needs.

The projects are for Microsoft Visual Studio 2022. The WPF project uses
the .NET 6 framework. The managed project uses .NET 4.5. Modify as needed
for your own applications.
