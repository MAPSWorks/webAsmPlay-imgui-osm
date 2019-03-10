/**
 ╭━━━━╮╱╱╱╱╱╱╱╱╱╭╮╱╭━━━╮╱╱╱╱╱╱╭╮
 ┃╭╮╭╮┃╱╱╱╱╱╱╱╱╱┃┃╱┃╭━╮┃╱╱╱╱╱╱┃┃
 ╰╯┃┃╰╯╭━╮╭━━╮╭╮┃┃╱┃┃╱╰╯╭━━╮╭━╯┃╭━━╮
 ╱╱┃┃╱╱┃╭╯┃╭╮┃┣┫┃┃╱┃┃╱╭╮┃╭╮┃┃╭╮┃┃┃━┫
 ╱╱┃┃╱╱┃┃╱┃╭╮┃┃┃┃╰╮┃╰━╯┃┃╰╯┃┃╰╯┃┃┃━┫
 ╱╱╰╯╱╱╰╯╱╰╯╰╯╰╯╰━╯╰━━━╯╰━━╯╰━━╯╰━━╯
 //
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

#include <unordered_set>
#include <unordered_map>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Attributes.h>
#include <geoServer/AttributeStatistics.h>

using namespace std;

namespace
{
    unordered_map<string, unordered_set<string> > lineStringAttrStats;
}

void AttributeStatistics::addLineStrings(const vector<AttributedLineString> & lineStrings)
{
    dmess("AttributeStatistics::addLineStrings " << lineStrings.size());

    for(auto & ls : lineStrings)
    {
        Attributes * attrs = get<0>(ls);

        for(const auto & i : attrs->strings) { lineStringAttrStats[i.first].insert(i.second) ;}
    }

	/*
    dmess("lineStringAttrStats " << lineStringAttrStats.size());

    for(const auto & i : lineStringAttrStats)
    {
        cout << "key: " << i.first << endl;

        for(const auto & j : i.second) { cout << "    " << j << endl ;}
    }
	*/
}