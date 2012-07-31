/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef MATERIALPARSER_HPP
#define MATERIALPARSER_HPP

#include "stdafx.h"

#include <OgreRoot.h>
#include "framework\AdvancedOgreFramework.hpp"

#include <fstream>

bool parseMaterial(const Ogre::Entity *entity, const std::string materialName);

//implementation in MaterialParser.cpp

#endif
