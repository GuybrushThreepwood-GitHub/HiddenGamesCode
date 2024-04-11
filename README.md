
## What is it ?

This repo contains the source code to the games released by Hidden Games on iOS and Android between the years 2008-2016 more or less.

For archival and historial purposes we thought it would be the right thing to do by releasing the source code and data to the games so that people can both have fun revisiting the games they may have played back in those years and maybe there would be some educational value being able to see how the games were built (with the caveat that this is really old code now and hasn't been touched for years).

## Who are Hidden Games ?

Hidden Games was John Murray and Gareth West (https://se7en23.co.uk/). 

* John did the programming/tools and design.
* Gaz did every piece of art/model/texture and design.

On some games we had collaborations with other friends.
* Julian Green - Character Artwork in AirCadets
* Matthew West - Logo Design AirCadets & Cabby, Voices in Cabby
* Nicki Kythreotis - Voices in Cabby
* Ian Tomlinson & Michael Lehane - Title Music for Prisoner 84

## History

We started working on the first iOS game AirCadets in September 2008 after the launch of the App Store in July 2008. The initial code base was used from previous demos we had done between ourselves, when the iPhone came along with the App Store and a much more open environment for developers we decided to put our heads down and started creating.

The games were mainly built on PC with a completely custom 'engine' (using the term engine very loosely) code so that we could port it to about anything that could compile C/C++. We did final testing and builds on the actual devices for iOS and Android so we could get the performance just right for each game.

## Games

The source code here are for the games...

* AirCadets (Originally released December 5th 2008 on iOS - internally known as H1) 
* Cabby (Originally released November 16th 2009 on iOS - internally known as H2)
* Prisoner 84 (Originally released July 15th 2010 on iOS - internally known as H4)
* Firewall (Originally released February 25th 2013 on iOS - internally known as H8)

The first three games were released and built around the original first generation iPhone / iPod Touch with HD updates when the first iPad was released in 2010. We tried to always keep the apps up to date with new hardware (e.g. AppleTV) until it became too costly as they never really made us much money even at their height to cover hardware costs.

We are obviously still very proud of what we did back then for a two man team and always enjoyed interacting with players when we got emails and reviews.

## Builds

We've put together an initial set of binary Windows 64 bit only builds for each game, along with their original data in another repo (https://github.com/GuybrushThreepwood-GitHub/HiddenGamesData) to keep everything separated (we did it this way when we started with SVN). We used lua for most of our scripting and logic for the games and when we released we binarized everything for speed and a bit of security, by default we've now left the lua as pure text so you can read and try to understand what it does.

There are other platforms support for some of the games (PC / MacOS / RaspberryPi) along with the original files for the iOS and Android builds if you want to play around with trying to recover them on modern xcode / android studio.

## Prerequisites

You'll need the redistributable for Visual Studio C++ 2022 here (https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170#visual-studio-2015-2017-2019-and-2022)

Plus you'll need to supply an OpenAL32.dll which can be installed here (https://www.openal.org/downloads/oalinst.zip)

## Docs

If we fancy remembering and documenting how each game worked, we'll try and build some documentation, but as it is, this readme is as far as we'll go. Most of the projects worked in the same way to an extent and they are very plainly written so once you understand one, the others are highly similar.

## Some handy keyboard inputs

### Aircadets
* WASD - Fly
* Space - Shoot
* XZ - Change speed
* P - Pause
* M - Menu
* 1-4 - Change environment
* 9 - Enable debug camera
* IJKL - Move debug camera
* Right Shift - Move debug camera faster
* F1 - Disable UI
* Left Mouse - Click/Touch
* Escape - Quit

### Cabby
* WASD - Control Taxi
* Space - Extend/Retract legs
* P - Pause
* 1-4 - Change environment
* 9 - Enable debug camera
* IJKL - Move debug camera
* Right Shift - Move debug camera faster
* Left Mouse - Click/Touch
* Escape - Quit

### Prisoner84
* WASD - Control Prisoner84
* Space - Shoot
* R - Reload
* E - Use
* Arrow keys - Change pitch
* Left Mouse - Click/Touch
* 1 - Swap weapon
* 3 - Toggle HUD
* 4 - Toggle fog
* 5 - Enable debug camera
* IJKL - Move debug camera
* Backspace - Toggle collision
* Escape - Quit

### Firewall
* Left Mouse - Click/Touch
* Left Mouse + move - Move firewall
* P - Pause
* Escape - Quit
  
## Screenshots

### Aircadets
![ac](https://github.com/GuybrushThreepwood-GitHub/HiddenGamesCode/assets/2954404/17f8a7fa-ed4b-47f8-a934-a4ed86324cb3)

### Cabby
![cabby](https://github.com/GuybrushThreepwood-GitHub/HiddenGamesCode/assets/2954404/92b99c31-bdf9-4dcf-b49a-919066447175)

### Prisoner84
![p84](https://github.com/GuybrushThreepwood-GitHub/HiddenGamesCode/assets/2954404/2b419b1a-8415-4a6b-8abf-c226fad70fc1)

### Firewall
![firewall](https://github.com/GuybrushThreepwood-GitHub/HiddenGamesCode/assets/2954404/80ddaf66-2ef7-47b7-a4f8-991a67ee96a1)


## Script Change Example

To launch Prisoner 84 in the original low res, low poly mode as it was released in 2010. Go to the folder lua/boot.lua 

Change the following values as written here...

`screenWidth = 320;`
`screenHeight = 480;`
`hiResMode = false;`

You should now see...

![p84-original](https://github.com/GuybrushThreepwood-GitHub/HiddenGamesCode/assets/2954404/ac7415e4-d9e3-4b16-b547-4068288ad946)



