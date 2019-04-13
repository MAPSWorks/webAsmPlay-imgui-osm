/**
 ╭━━━━╮╱╱╱╱╱╱╱╱╱╭╮╱╭━━━╮╱╱╱╱╱╱╭╮
 ┃╭╮╭╮┃╱╱╱╱╱╱╱╱╱┃┃╱┃╭━╮┃╱╱╱╱╱╱┃┃
 ╰╯┃┃╰╯╭━╮╭━━╮╭╮┃┃╱┃┃╱╰╯╭━━╮╭━╯┃╭━━╮
 ╱╱┃┃╱╱┃╭╯┃╭╮┃┣┫┃┃╱┃┃╱╭╮┃╭╮┃┃╭╮┃┃┃━┫
 ╱╱┃┃╱╱┃┃╱┃╭╮┃┃┃┃╰╮┃╰━╯┃┃╰╯┃┃╰╯┃┃┃━┫
 ╱╱╰╯╱╱╰╯╱╰╯╰╯╰╯╰━╯╰━━━╯╰━━╯╰━━╯╰━━╯
 // This software is provided 'as-is', without any express or implied
 // warranty.  In no event will the authors be held liable for any damages
 // arising from the use of this software.
 // Permission is granted to anyone to use this software for any purpose,
 // including commercial applications, and to alter it and redistribute it
 // freely, subject to the following restrictions:
 // 1. The origin of this software must not be misrepresented; you must not
 //    claim that you wrote the original software. If you use this software
 //    in a product, an acknowledgment in the product documentation would be
 //    appreciated but is not required.
 // 2. Altered source versions must be plainly marked as such, and must not be
 //    misrepresented as being the original software.
 // 3. This notice may not be removed or altered from any source distribution.

  \author Matthew Tang
  \email trailcode@gmail.com
  \copyright 2018
*/
#pragma once

#include <OpenSteer/PlugIn.h>
#include <OpenSteer/Proximity.h>
#include <webAsmPlay/Zombie.h>

class Network;

namespace OpenSteer
{
    class ProximityDatabase;
}

class ZombiePlugin : public OpenSteer::PlugIn
{
public:

    typedef OpenSteer::AbstractProximityDatabase<OpenSteer::AbstractVehicle *> ProximityDatabase; // TODO Code dup!

    const char * name();

    float selectionOrderSortKey();

    virtual ~ZombiePlugin();

    void open();

    void update(const float currentTime, const float elapsedTime);

    void redraw(const float currentTime, const float elapsedTime);

    void serialNumberAnnotationUtility(const OpenSteer::AbstractVehicle & selected,
                                       const OpenSteer::AbstractVehicle & nearMouse);

    void drawPathAndObstacles();

    void close();

    void reset();

    void handleFunctionKeys(int keyNumber);

    void printMiniHelpForFunctionKeys();

    void addPedestrianToCrowd();

    void removePedestrianFromCrowd();

    void nextPD();

    const OpenSteer::AVGroup & allVehicles();

    static Network * setNetwork(Network * network);
    static Network * getNetwork();

    Zombie::groupType m_crowd;

    typedef Zombie::groupType::const_iterator iterator;

    OpenSteer::Vec3 m_gridCenter;

    ProximityDatabase * m_pd;

    int m_population;

    int m_cyclePD;

private:

};

