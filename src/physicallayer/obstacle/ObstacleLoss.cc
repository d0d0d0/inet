//
// Copyright (C) 2013 OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#include "IRadioMedium.h"
#include "ObstacleLoss.h"
#include "PhysicalObject.h"

namespace inet {

namespace physicallayer {

Define_Module(ObstacleLoss);

ObstacleLoss::ObstacleLoss() :
    environment(NULL)
{
}

void ObstacleLoss::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        medium = check_and_cast<IRadioMedium *>(getParentModule());
        const char *environmentModule = par("environmentModule");
        environment = check_and_cast<PhysicalEnvironment *>(simulation.getModuleByPath(environmentModule));
        leaveIntersectionTrail = par("leaveIntersectionTrail");
    }
}

void ObstacleLoss::printToStream(std::ostream& stream) const
{
    stream << "obstacle loss";
}

double ObstacleLoss::computeDielectricLoss(const Material *material, Hz frequency, m distance) const
{
    double lossTangent = material->getDielectricLossTangent(frequency);
    mps propagationSpeed = material->getPropagationSpeed();
    double factor = std::exp(-atan(lossTangent) * unit(2 * M_PI * frequency * distance / propagationSpeed).get());
    ASSERT(0 <= factor && factor <= 1);
    return factor;
}

double ObstacleLoss::computeReflectionLoss(const Material *incidentMaterial, const Material *refractiveMaterial, double angle) const
{
    // NOTE: based on http://en.wikipedia.org/wiki/Fresnel_equations
    double n1 = incidentMaterial->getRefractiveIndex();
    double n2 = refractiveMaterial->getRefractiveIndex();
    double st = sin(angle);
    double ct = cos(angle);
    double n1ct = n1 * ct;
    double n2ct = n2 * ct;
    double k = sqrt(1 - pow(n1 / n2 * st, 2));
    double n1k = n1 * k;
    double n2k = n2 * k;
    double rs = pow((n1ct - n2k) / (n1ct + n2k), 2);
    double rp = pow((n1k - n2ct) / (n1k + n2ct), 2);
    double r = (rs + rp) / 2;
    double transmittance = 1 - r;
    ASSERT(0 <= transmittance && transmittance <= 1);
    return transmittance;
}

double ObstacleLoss::computeObstacleLoss(Hz frequency, const Coord transmissionPosition, const Coord receptionPosition) const
{
    double totalLoss = 1;
    const std::vector<PhysicalObject *>& objects = environment->getObjects();
    for (std::vector<PhysicalObject *>::const_iterator it = objects.begin(); it != objects.end(); it++) {
        const PhysicalObject *object = *it;
        const Shape *shape = object->getShape();
        const Coord& obstaclePosition = object->getPosition();
        const LineSegment lineSegment(transmissionPosition - obstaclePosition, receptionPosition - obstaclePosition);
        Coord intersection1, intersection2, normal1, normal2;
        if (shape->computeIntersection(lineSegment, intersection1, intersection2, normal1, normal2))
        {
#ifdef __CCANVAS_H
            if (leaveIntersectionTrail) {
                cLayer *layer = environment->getParentModule()->getCanvas()->getDefaultLayer();
                if (intersectionTrail.size() > 100) {
                    cFigure *front = intersectionTrail.front();
                    intersectionTrail.pop_front();
                    layer->removeChild(front);
                    delete front;
                }
                cLineFigure *intersectionLine = new cLineFigure();
                intersectionLine->setStart(cFigure::Point(intersection1.x + obstaclePosition.x, intersection1.y + obstaclePosition.y));
                intersectionLine->setEnd(cFigure::Point(intersection2.x + obstaclePosition.x, intersection2.y + obstaclePosition.y));
                intersectionLine->setLineColor(cFigure::GREY);
                intersectionTrail.push_back(intersectionLine);
                layer->addChild(intersectionLine);
                cLineFigure *normal1Line = new cLineFigure();
                normal1Line->setStart(cFigure::Point(intersection1.x + obstaclePosition.x, intersection1.y + obstaclePosition.y));
                normal1Line->setEnd(cFigure::Point(intersection1.x + normal1.x + obstaclePosition.x, intersection1.y + normal1.y + obstaclePosition.y));
                normal1Line->setLineColor(cFigure::RED);
                intersectionTrail.push_back(normal1Line);
                layer->addChild(normal1Line);
                cLineFigure *normal2Line = new cLineFigure();
                normal2Line->setStart(cFigure::Point(intersection2.x + obstaclePosition.x, intersection2.y + obstaclePosition.y));
                normal2Line->setEnd(cFigure::Point(intersection2.x + normal2.x + obstaclePosition.x, intersection2.y + normal2.y + obstaclePosition.y));
                normal2Line->setLineColor(cFigure::RED);
                intersectionTrail.push_back(normal2Line);
                layer->addChild(normal2Line);
            }
#endif
            const Material *material = object->getMaterial();
            totalLoss *= computeDielectricLoss(material, frequency, m(intersection2.distance(intersection1)));
            if (!normal1.isUnspecified()) {
                double angle1 = (intersection1 - intersection2).angle(normal1);
                totalLoss *= computeReflectionLoss(medium->getMaterial(), material, angle1);
            }
            // TODO: this returns NaN because n1 > n2
//            if (!normal2.isUnspecified()) {
//                double angle2 = (intersection2 - intersection1).angle(normal2);
//                totalLoss *= computeReflectionLoss(material, medium->getMaterial(), angle2);
//            }
        }
    }
    return totalLoss;
}

} // namespace physicallayer

} // namespace inet

