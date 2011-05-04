/** This file is part of Fractal Library.
 *
 * Fractal Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fractal Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Fractal Library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Layer.h"

using namespace FL::Trees;

Layer::Layer()
{
}

Node* Layer::getPrevNearestTo(int position) const
{
    if (position == 0 || size() == 0 || front()->end() > position)
        return NULL;

    const_iterator node = begin(), nextNode = begin();
    for (++nextNode; nextNode != end(); ++node, ++nextNode)
    {
        if ((*node)->end() <= position && (*nextNode)->end() > position)
            return *node;
    }
    return back();
}

bool nodeLessTimeCompare(const Node* node1, const Node *node2)
{
    return node1->begin() < node2->begin();
}

bool nodeLessLevelCompare(const Node* node1, const Node *node2)
{
    return node1->level() < node2->level();
}

void Layer::sortByTime()
{
    // Sort needs random access iterator, while Layer is the std::list
    std::vector<Node*> vec(begin(), end());
    std::sort(vec.begin(), vec.end(), nodeLessTimeCompare);
    assign(vec.begin(), vec.end());
}

void Layer::sortByLevel()
{
    // Sort needs random access iterator, while Layer is the std::list
    std::vector<Node*> vec(begin(), end());
    std::sort(vec.begin(), vec.end(), nodeLessLevelCompare);
    assign(vec.begin(), vec.end());
}
