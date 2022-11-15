# BenTo
Official BenTo Light Props Show Authoring Tool

**This software is under heavy development and is subject to frequent changes !**

## Documentation

The documentation can be found at: https://bkuperberg.gitbook.io/bento-docs/

## Discord

If you have questions please join our discord server: https://discord.gg/kedAeCbKUM

## The Download Zone

Releases are on the release page : https://github.com/benkuper/BenTo/releases

You can find the bleeding edge version, compiled against latest commits here :

- Linux (x64):  http://benjamin.kuperberg.fr/bento/download/BenTo-linux-x64-bleedingedge.AppImage
- MacOS :  http://benjamin.kuperberg.fr/bento/download/BenTo-osx-bleedingedge.pkg
- Windows (x64):  http://benjamin.kuperberg.fr/bento/download/BenTo-win-x64-bleedingedge.exe

Additionally, you can download a set of example scripts that you can then use as Script Blocks in the software : http://benjamin.kuperberg.fr/bento/download/BenTo-Example-Scripts.zip

## Building the software

BenTo is built on top of the JUCE frameworks. You can find more infos about it there : http://www.juce.com
Some code needs to be on the develop branch of juce, so you can't use the official release of JUCE until the next one.
To build the software, you need to :
  - Clone the develop-local branch of my JUCE fork here :
  
  `git clone --branch=develop-local http://github.com/benkuper/JUCE`
  - Download the dependencies (see below)
  - Launch the Projucer and open BenTo.jucer
  - Link the dependencies on the modules section
  - Save the jucer
  - Open the solution for you platform in the Builds folder
  - Build the solution from your IDE or make in linux without Code::Blocks

### Dependencies

  Chataigne relies on the OrganicUI JUCE module and juce_timeline that you can find here :  
  - https://github.com/benkuper/juce_organicui
  - https://github.com/benkuper/juce_timeline
  - https://github.com/benkuper/juce_sharedtexture
  
  You will need to clone those repositories and link the Projucer juce_organicui and juce_timeline modules to its parent folder

## The Badge Zone

- Linux (x64):  [![Build Status](https://travis-matrix-badges.herokuapp.com/repos/benkuper/BenTo/branches/master/2)](https://travis-ci.org/benkuper/BenTo)

- MacOS : [![Build Status](https://travis-matrix-badges.herokuapp.com/repos/benkuper/BenTo/branches/master/1)](https://travis-ci.org/benkuper/BenTo)

- Windows (x64) : [![Build status](https://ci.appveyor.com/api/projects/status/r85kfdu2bv4ljxng?svg=true)](https://ci.appveyor.com/project/benkuper/bento)

[![forthebadge](http://forthebadge.com/images/badges/gluten-free.svg)](http://forthebadge.com)


## Bento Remote (smartphone app)

There is also a smartphone app that covers the functionality of the OSC command API: https://github.com/jonglissimo/bento-remote

## Screenshots

![Bento](http://benjamin.kuperberg.fr/download/bento.png)
