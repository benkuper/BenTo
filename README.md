# BenTo
Official BenTo Light Props Show Authoring Tool

**This software is under heavy development and will hopefully be back and running in September 2018, stronger than ever !**

## The Download Zone

Releases are on the release page.

You can find the bleeding edge version, compiled against latest commits here :

- Linux (x64):  http://benjamin.kuperberg.fr/download/bento/BenTo-linux-x64-bleedingedge.zip

- MacOS :  http://benjamin.kuperberg.fr/download/bento/BenTo-osx-bleedingedge.zip
 
- Windows (x64):  http://benjamin.kuperberg.fr/download/bento/BenTo-win-x64-bleedingedge.zip

## Building the software

BenTo is built on top of the JUCE frameworks. You can find more infos about it there : http://www.juce.com
Some code needs to be on the develop branch of juce, so you can't use the official release of JUCE until the next one.
To build the software, you need to :
  - Clone the develop-local branch of my JUCE fork here :
  
  `git clone --branch=develop-local http://github.com/benkuper/JUCE`
  - Download the dependencies (see below)
  - Launch the Projucer and open BenTo.jucer
  - Link the dependencies
  - Save the jucer
  - Open the solution for you platform in the Builds folder
  - Build the solution from your IDE or make in linux without Code::Blocks

### Dependencies

  Chataigne relies on the OrganicUI JUCE module and juce_timeline that you can find here : https://github.com/benkuper/juce_organicui and https://github.com/benkuper/juce_timeline

  You will need to download, extract and link the Projucer juce_organicui and juce_timeline modules to its parent folder

## The Badge Zone

- Linux (x64):  [![Build Status](https://travis-matrix-badges.herokuapp.com/repos/benkuper/BenTo/branches/master/2)](https://travis-ci.org/benkuper/BenTo)

- MacOS : [![Build Status](https://travis-matrix-badges.herokuapp.com/repos/benkuper/BenTo/branches/master/1)](https://travis-ci.org/benkuper/BenTo)

- Windows (x64) : [![Build status](https://ci.appveyor.com/api/projects/status/r85kfdu2bv4ljxng?svg=true)](https://ci.appveyor.com/project/benkuper/bento)

[![forthebadge](http://forthebadge.com/images/badges/gluten-free.svg)](http://forthebadge.com)

## Screenshots

![Bento](http://benjamin.kuperberg.fr/download/bento.png)
