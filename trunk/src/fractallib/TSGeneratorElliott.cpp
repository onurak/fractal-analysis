/* Author: Vladimir Belikov
 *
 * This file is part of Fractal Library.
 *
 * Fractal Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fractal Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Fractal Libray.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "TSGeneratorElliott.h"

using namespace FL;

TSGeneratorElliott::TSGeneratorElliott()
{
    /* генерируем строительные блоки */
    TSGenerator::Item item;

    // impulse up
    item.name = "im";
    item.no = 1;
    item.lengths.clear();
    item.directions.clear();
    item.conditions.clear();
    item.subitems.clear();
    item.add(UP, 20);
    item.add(DOWN, 10);
    item.add(UP, 30);
    item.add(DOWN, 10);
    item.add(UP, 20);
    item.subitems.push_back("im");
    item.subitems.push_back("cr");
    item.subitems.push_back("im");
    item.subitems.push_back("cr");
    item.subitems.push_back("im");
    item.conditions.push_back(Condition(PREVIOUS_NOT, "im", 1));
    item.conditions.push_back(Condition(PREVIOUS_NOT, "im", 2));
    addTemplate(item);

    // impulse down
    item.name = "im";
    item.no = 2;
    item.lengths.clear();
    item.directions.clear();
    item.conditions.clear();
    item.subitems.clear();
    item.add(DOWN, 20);
    item.add(UP, 10);
    item.add(DOWN, 30);
    item.add(UP, 10);
    item.add(DOWN, 20);
    item.subitems.push_back("cr");
    item.subitems.push_back("im");
    item.subitems.push_back("cr");
    item.subitems.push_back("im");
    item.subitems.push_back("cr");
    item.conditions.push_back(Condition(PREVIOUS_NOT, "im", 1));
    item.conditions.push_back(Condition(PREVIOUS_NOT, "im", 2));
    addTemplate(item);

    // correction down
    item.lengths.clear();
    item.directions.clear();
    item.conditions.clear();
    item.subitems.clear();
    item.name = "cr";
    item.no = 1;
    item.add(DOWN, 20);
    item.add(UP, 10);
    item.add(DOWN, 25);
    item.subitems.push_back("cr");
    item.subitems.push_back("im");
    item.subitems.push_back("cr");
    item.conditions.push_back(Condition(PREVIOUS, "im", 1));
    addTemplate(item);

    // correction up
    item.lengths.clear();
    item.directions.clear();
    item.conditions.clear();
    item.subitems.clear();
    item.name = "cr";
    item.no = 2;
    item.add(UP, 20);
    item.add(DOWN, 10);
    item.add(UP, 25);
    item.subitems.push_back("im");
    item.subitems.push_back("cr");
    item.subitems.push_back("im");
    item.conditions.push_back(Condition(PREVIOUS, "im", 2));
    addTemplate(item);

    // ZZ
    item.lengths.clear();
    item.directions.clear();
    item.conditions.clear();
    item.subitems.clear();
    item.name = "ZZ";
    item.no = 1;
    item.subitems.push_back("im");
    item.subitems.push_back("cr");
    item.subitems.push_back("im");
    addTemplate(item);

    // ET
    item.lengths.clear();
    item.directions.clear();
    item.conditions.clear();
    item.subitems.clear();
    item.name = "ET";
    item.no = 1;
    item.subitems.push_back("cr");
    item.subitems.push_back("cr");
    item.subitems.push_back("cr");
    addTemplate(item);

    // error
    item.lengths.clear();
    item.directions.clear();
    item.conditions.clear();
    item.subitems.clear();
    item.name = "error";
    item.no = 1;
    item.add(DOWN, 10);
    item.add(DOWN, 10);
    item.subitems.push_back("cr");
    item.subitems.push_back("cr");
    item.subitems.push_back("im");
    //addTemplate(item);

    item.lengths.clear();
    item.directions.clear();
    item.conditions.clear();
    item.subitems.clear();
    item.name = "error";
    item.no = 2;
    item.add(DOWN, 1);
    item.add(UP, 2);
    item.add(DOWN, 1);
    item.subitems.push_back("cr");
    item.subitems.push_back("cr");
    item.subitems.push_back("im");
    //addTemplate(item);


    setErrorFactor(1);
}
