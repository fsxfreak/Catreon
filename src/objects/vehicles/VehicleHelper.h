/*
author: Leon Cheung <me[at]leoncheung.com>
Copyright (C) 2012

This program is released under the terms of
GNU Lesser General Public License version 3.0
available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef VEHICLE_HELPER_H
#define VEHICLE_HELPER_H

#include "stdafx.h"
#include <vector>
#include "objects\vehicles\Vehicle.h"

class VehicleHelper
{
private:
    std::vector<Vehicle*> mVehicles;

public:
    VehicleHelper();
    ~VehicleHelper();

    void addVehicle(const Vehicle& vehicle);

};

#endif
