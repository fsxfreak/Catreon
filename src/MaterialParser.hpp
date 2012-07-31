/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef MATERIAL_PARSER_HPP
#define MATERIAL_PARSER_HPP

#include "stdafx.h"

#include <OgreRoot.h>
#include "framework\AdvancedOgreFramework.hpp"

#include <fstream>

bool parseMaterial(const Ogre::Entity *entity, const std::string materialName)
{
    std::fstream read(std::string("../media/") + materialName);

    if (!read.is_open())
    {
        OgreFramework::getSingletonPtr()->mLog->logMessage(
            std::string("Failed to read material file: ") + materialName + ".");
        return 0;
    }
    else
    {
        int numIterations = 0;
        while (read.good())
        {
            std::string line;
            std::getline(read, line);
                
            if (line.substr(0, 8) == "material")
            {
                line.erase(0, 9);	//erase the word "material ", leaving only the material name
                entity->getSubEntity(numIterations)->setMaterialName(line);
            }
            ++numIterations;
        }
    }
    return 1;
}

#endif
