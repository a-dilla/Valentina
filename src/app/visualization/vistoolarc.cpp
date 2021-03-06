/************************************************************************
 **
 **  @file   vistoolarc.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   15 8, 2014
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentine project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2014 Valentina project
 **  <https://bitbucket.org/dismine/valentina> All Rights Reserved.
 **
 **  Valentina is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  Valentina is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with Valentina.  If not, see <http://www.gnu.org/licenses/>.
 **
 *************************************************************************/

#include "vistoolarc.h"
#include "../geometry/vpointf.h"
#include "../geometry/varc.h"
#include "../container/vcontainer.h"

//---------------------------------------------------------------------------------------------------------------------
VisToolArc::VisToolArc(const VContainer *data, QGraphicsItem *parent)
    :VisPath(data, parent), arcCenter(nullptr), radius(0), f1(0), f2(0)
{
    arcCenter = InitPoint(mainColor, this);
}

//---------------------------------------------------------------------------------------------------------------------
VisToolArc::~VisToolArc()
{}

//---------------------------------------------------------------------------------------------------------------------
void VisToolArc::RefreshGeometry()
{
    if (point1Id > NULL_ID)
    {
        const QSharedPointer<VPointF> first = Visualization::data->GeometricObject<VPointF>(point1Id);
        DrawPoint(arcCenter, first->toQPointF(), supportColor);

        if (qFuzzyCompare(1 + radius, 1 + 0) == false && f1 >= 0 && f2 >= 0 && qFuzzyCompare(1 + f1, 1 + f2) == false)
        {
            VArc arc = VArc (*first, radius, f1, f2);
            DrawPath(this, arc.GetPath(PathDirection::Show), mainColor, Qt::SolidLine, Qt::RoundCap);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VisToolArc::setRadius(const QString &expression)
{
    radius = FindLength(expression);
}

//---------------------------------------------------------------------------------------------------------------------
void VisToolArc::setF1(const QString &expression)
{
    f1 = FindVal(expression);
}

//---------------------------------------------------------------------------------------------------------------------
void VisToolArc::setF2(const QString &expression)
{
    f2 = FindVal(expression);
}
