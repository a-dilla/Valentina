/************************************************************************
 **
 **  @file   vtoolcutspline.cpp
 **  @author Roman Telezhinsky <dismine@gmail.com>
 **  @date   15 12, 2013
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentine project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2013 Valentina project
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

#include "vtoolcutspline.h"
#include "../../container/calculator.h"

const QString VToolCutSpline::ToolType = QStringLiteral("cutSpline");
const QString VToolCutSpline::AttrSpline = QStringLiteral("spline");

VToolCutSpline::VToolCutSpline(VDomDocument *doc, VContainer *data, const qint64 &id, const QString &formula,
                               const qint64 &splineId, const qint64 &spl1id, const qint64 &spl2id,
                               const Tool::Sources &typeCreation, QGraphicsItem *parent)
    :VToolPoint(doc, data, id, parent), formula(formula), splineId(splineId),
      dialogCutSpline(QSharedPointer<DialogCutSpline>()), firstSpline(), secondSpline(), spl1id(spl1id), spl2id(spl2id)
{
    Q_ASSERT_X(splineId > 0, Q_FUNC_INFO, "splineId <= 0");
    Q_ASSERT_X(spl1id > 0, Q_FUNC_INFO, "spl1id <= 0");
    Q_ASSERT_X(spl2id > 0, Q_FUNC_INFO, "spl2id <= 0");

    firstSpline = new VSimpleSpline(spl1id, &currentColor, &factor);
    Q_ASSERT(firstSpline != 0);
    RefreshSpline(firstSpline, spl1id, SimpleSpline::ForthPoint);
    firstSpline->setParentItem(this);
    connect(firstSpline, &VSimpleSpline::Choosed, this, &VToolCutSpline::SplineChoosed);

    secondSpline = new VSimpleSpline(spl2id, &currentColor, &factor);
    Q_ASSERT(secondSpline != 0);
    RefreshSpline(secondSpline, spl2id, SimpleSpline::FirstPoint);
    secondSpline->setParentItem(this);
    connect(secondSpline, &VSimpleSpline::Choosed, this, &VToolCutSpline::SplineChoosed);

    if (typeCreation == Tool::FromGui)
    {
        AddToFile();
    }
}

void VToolCutSpline::setDialog()
{
    Q_ASSERT(dialogCutSpline.isNull() == false);
    VPointF point = VAbstractTool::data.GetPoint(id);
    dialogCutSpline->setFormula(formula);
    dialogCutSpline->setSplineId(splineId, id);
    dialogCutSpline->setPointName(point.name());
}

void VToolCutSpline::Create(QSharedPointer<DialogCutSpline> &dialog, VMainGraphicsScene *scene,
                            VDomDocument *doc, VContainer *data)
{
    QString pointName = dialog->getPointName();
    QString formula = dialog->getFormula();
    qint64 splineId = dialog->getSplineId();
    Create(0, pointName, formula, splineId, 5, 10, scene, doc, data, Document::FullParse, Tool::FromGui);
}

void VToolCutSpline::Create(const qint64 _id, const QString &pointName,
                            const QString &formula, const qint64 &splineId, const qreal &mx, const qreal &my,
                            VMainGraphicsScene *scene, VDomDocument *doc, VContainer *data,
                            const Document::Documents &parse, const Tool::Sources &typeCreation)
{
    VSpline spl = data->GetSpline(splineId);
    Calculator cal(data);
    QString errorMsg;
    qreal result = cal.eval(formula, &errorMsg);
    if (errorMsg.isEmpty())
    {
        QPointF spl1p2, spl1p3, spl2p2, spl2p3;
        QPointF point = spl.CutSpline(toPixel(result), spl1p2, spl1p3, spl2p2, spl2p3);

        qint64 id = _id;
        qint64 spl1id = 0;
        qint64 spl2id = 0;
        if (typeCreation == Tool::FromGui)
        {
            id = data->AddPoint(VPointF(point.x(), point.y(), pointName, mx, my));
            spl1id = id + 1;
            spl2id = id + 2;

            VSpline spline1 = VSpline(data->DataPoints(), spl.GetP1(), spl1p2, spl1p3, id, spl.GetKcurve());
            spl1id = data->AddSpline(spline1);
            data->AddLengthSpline(spline1.name(), toMM(spline1.GetLength()));

            VSpline spline2 = VSpline(data->DataPoints(), id, spl2p2, spl2p3, spl.GetP4(), spl.GetKcurve());
            spl2id = data->AddSpline(spline2);
            data->AddLengthSpline(spline2.name(), toMM(spline2.GetLength()));
        }
        else
        {
            data->UpdatePoint(id, VPointF(point.x(), point.y(), pointName, mx, my));

            spl1id = id + 1;
            spl2id = id + 2;

            VSpline spline1 = VSpline(data->DataPoints(), spl.GetP1(), spl1p2, spl1p3, id, spl.GetKcurve());
            data->UpdateSpline(spl1id, spline1);
            data->AddLengthSpline(spline1.name(), toMM(spline1.GetLength()));

            VSpline spline2 = VSpline(data->DataPoints(), id, spl2p2, spl2p3, spl.GetP4(), spl.GetKcurve());
            data->UpdateSpline(spl2id, spline2);
            data->AddLengthSpline(spline2.name(), toMM(spline2.GetLength()));

            if (parse != Document::FullParse)
            {
                doc->UpdateToolData(id, data);
            }
        }
        //VDrawTool::AddRecord(id, Tool::CutSplineTool, doc);
        if (parse == Document::FullParse)
        {
            VToolCutSpline *point = new VToolCutSpline(doc, data, id, formula, splineId, spl1id, spl2id, typeCreation);
            scene->addItem(point);
            connect(point, &VToolPoint::ChoosedTool, scene, &VMainGraphicsScene::ChoosedItem);
            connect(point, &VToolPoint::RemoveTool, scene, &VMainGraphicsScene::RemoveTool);
            connect(scene, &VMainGraphicsScene::NewFactor, point, &VToolPoint::SetFactor);
            doc->AddTool(id, point);
            doc->AddTool(spl1id, point);
            doc->AddTool(spl2id, point);
            doc->IncrementReferens(splineId);
        }
    }
}

void VToolCutSpline::FullUpdateFromFile()
{
    QDomElement domElement = doc->elementById(QString().setNum(id));
    if (domElement.isElement())
    {
        formula = domElement.attribute(AttrLength, "");
        splineId = domElement.attribute(AttrSpline, "").toLongLong();
    }
    RefreshGeometry();
}

void VToolCutSpline::FullUpdateFromGui(int result)
{
    if (result == QDialog::Accepted)
    {
        QDomElement domElement = doc->elementById(QString().setNum(id));
        if (domElement.isElement())
        {
            domElement.setAttribute(AttrName, dialogCutSpline->getPointName());
            domElement.setAttribute(AttrLength, dialogCutSpline->getFormula());
            domElement.setAttribute(AttrSpline, QString().setNum(dialogCutSpline->getSplineId()));
            emit FullUpdateTree();
        }
    }
    dialogCutSpline.clear();
}

void VToolCutSpline::SplineChoosed(qint64 id)
{
    emit ChoosedTool(id, Scene::Spline);
}

void VToolCutSpline::ChangedActivDraw(const QString &newName)
{
    if (nameActivDraw == newName)
    {
        currentColor = Qt::black;
        firstSpline->setFlag(QGraphicsItem::ItemIsSelectable, true);
        firstSpline->setAcceptHoverEvents(true);
        secondSpline->setFlag(QGraphicsItem::ItemIsSelectable, true);
        secondSpline->setAcceptHoverEvents(true);
    }
    else
    {
        currentColor = Qt::gray;
        firstSpline->setFlag(QGraphicsItem::ItemIsSelectable, false);
        firstSpline->setAcceptHoverEvents(false);
        secondSpline->setFlag(QGraphicsItem::ItemIsSelectable, false);
        secondSpline->setAcceptHoverEvents(false);
    }
    firstSpline->setPen(QPen(currentColor, widthHairLine/factor));
    secondSpline->setPen(QPen(currentColor, widthHairLine/factor));
    VToolPoint::ChangedActivDraw(newName);
}

void VToolCutSpline::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    ContextMenu(dialogCutSpline, this, event);
}

void VToolCutSpline::AddToFile()
{
    VPointF point = VAbstractTool::data.GetPoint(id);
    QDomElement domElement = doc->createElement(TagName);

    AddAttribute(domElement, AttrId, id);
    AddAttribute(domElement, AttrType, ToolType);
    AddAttribute(domElement, AttrName, point.name());
    AddAttribute(domElement, AttrMx, toMM(point.mx()));
    AddAttribute(domElement, AttrMy, toMM(point.my()));

    AddAttribute(domElement, AttrLength, formula);
    AddAttribute(domElement, AttrSpline, splineId);

    AddToCalculation(domElement);
}

void VToolCutSpline::RefreshGeometry()
{
    RefreshSpline(firstSpline, spl1id, SimpleSpline::ForthPoint);
    RefreshSpline(secondSpline, spl2id, SimpleSpline::FirstPoint);
    VToolPoint::RefreshPointGeometry(VDrawTool::data.GetPoint(id));
}

void VToolCutSpline::RefreshSpline(VSimpleSpline *spline, qint64 splid, SimpleSpline::Translation tr)
{
    VSpline spl = VAbstractTool::data.GetSpline(splid);
    QPainterPath path;
    path.addPath(spl.GetPath());
    path.setFillRule( Qt::WindingFill );
    if(tr == SimpleSpline::FirstPoint)
    {
        path.translate(-spl.GetPointP1().toQPointF().x(), -spl.GetPointP1().toQPointF().y());
    }
    else
    {
        path.translate(-spl.GetPointP4().toQPointF().x(), -spl.GetPointP4().toQPointF().y());
    }
    spline->setPath(path);
}