# Boost-Samples
Boost C++ Libraries Samples

Install Boost C++ ( https://www.boost.org )
 
    1. Download Boost c++ 1.66.0 binary

     - Visual Studio 2015
      32bit https://dl.bintray.com/boostorg/release/1.66.0/binaries/boost_1_66_0-msvc-14.0-32.exe
      64bit https://dl.bintray.com/boostorg/release/1.66.0/binaries/boost_1_66_0-msvc-14.0-64.exe
    
    - Visual Studio 2017
      32bit https://dl.bintray.com/boostorg/release/1.66.0/binaries/boost_1_66_0-msvc-14.1-32.exe
      64bit https://dl.bintray.com/boostorg/release/1.66.0/binaries/boost_1_66_0-msvc-14.1-64.exe
      
    2. Create or Rename Directory Path 
    
     C:\boost_1_66_0


Setting Visual Studio INC/LIB Path

    Setting globally the environment
      
      1. Click the Menu. View > Property Manager
      2. Click the right mouse button and Click Debug | Win32 Microsoft.Cpp.Win32.user or Microsoft.Cpp.x64.user
       and Click Property on the Popup Menu. Click VC++ Directories.
       
       Include Directories : C:\boost_1_66_0
       Library Directories : C:\boost_1_66_0\lib32-msvc-14.0
       Source Directories  : C:\boost_1_66_0\libs
       
