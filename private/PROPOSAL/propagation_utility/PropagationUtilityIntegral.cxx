
#include <cmath>
#include <functional>

#include "PROPOSAL/crossection/CrossSection.h"
#include "PROPOSAL/crossection/parametrization/Parametrization.h"
#include "PROPOSAL/propagation_utility/PropagationUtilityIntegral.h"
#include "PROPOSAL/crossection/parametrization/Parametrization.h"

#include "PROPOSAL/Constants.h"
#include "PROPOSAL/Logging.h"

using namespace PROPOSAL;

/******************************************************************************
 *                              Utility Integral                              *
 ******************************************************************************/

UtilityIntegral::UtilityIntegral(CrossSectionList cross, const ParticleDef& p_def)
    : UtilityDecorator(cross, p_def)
    , integral_(IROMB, IMAXS, IPREC2)
{
}

double UtilityIntegral::Calculate(double ei, double ef, double rnd)
{
    return integral_.IntegrateWithRandomRatio(ei, ef,
        std::bind(
            &UtilityIntegral::FunctionToIntegral, this, std::placeholders::_1),
        4, -rnd);
}

double UtilityIntegral::GetUpperLimit(double ei, double rnd)
{
    (void)ei;
    (void)rnd;

    return integral_.GetUpperLimit();
}

/******************************************************************************
 *                            Utility Displacement                            *
 ******************************************************************************/
UtilityIntegralDisplacement::UtilityIntegralDisplacement(
    CrossSectionList cross, const ParticleDef& p_def)
    : UtilityIntegral(cross, p_def)
{
}

double UtilityIntegralDisplacement::FunctionToIntegral(double energy)
{
    double result = 0.0;
    for (const auto& cross : crosssections)
        result += cross->CalculatedEdx(energy);

    return -1.0 / result;
}

double UtilityIntegralDisplacment::Calculate(double ei, double ef, double rnd)
{
    if(ef > mass)
        return UtilityIntegral::Calculate(ei, ef, rnd) / lifetime;

    return UtilityIntegral::Calculate(ei, mass, rnd) / lifetime;
}

/******************************************************************************
 *                            Utility Interaction                            *
 ******************************************************************************/

UtilityIntegralInteraction::UtilityIntegralInteraction(
    CrossSectionList cross, const ParticleDef&)
    : UtilityIntegral(cross)
{
}

double UtilityIntegralInteraction::FunctionToIntegral(double energy)
{
    double total_rate = 0.0;
    for (const auto& cross : crosssections)
        total_rate += cross->CalculatedNdx(energy);

    return displacement_->FunctionToIntegral(energy) * total_rate;
}

/******************************************************************************
 *                            Utility Decay                            *
 ******************************************************************************/

UtilityIntegralDecay::UtilityIntegralDecay(
    CrossSectionList cross, const ParticleDef& p_def)
    : UtilityIntegral(cross, p_def)
    , lifetime(p_def.lifetime)
    , mass(p_def.mass)
{
}

double UtilityIntegralDecay::FunctionToIntegral(double energy)
{
    if (lifetime < 0)
        return 0;
    double square_momentum = (energy - mass) * (energy + mass);
    double particle_momentum = std::sqrt(std::max(square_momentum, 0.0));

    double aux = 1.0
        / std::max(
              (particle_momentum / mass) * SPEED, PARTICLE_POSITION_RESOLUTION);
    return displacement_->FunctionToIntegral(energy) * aux;
}

double UtilityIntegralDecay::Calculate(double ei, double ef, double rnd)
{
    if(ef > mass)
        return UtilityIntegral::Calculate(ei, ef, rnd) / lifetime;

    return UtilityIntegral::Calculate(ei, mass, rnd) / lifetime;
}

/******************************************************************************
 *                            Utility Time                            *
 ******************************************************************************/

UtilityIntegralTime::UtilityIntegralTime(
    CrossSectionList cross, const ParticleDef& p_def)
    : UtilityIntegral(cross, p_def)
    , mass(p_def.mass)
{
}

double UtilityIntegralTime::FunctionToIntegral(double energy)
{
    double square_momentum{ (energy - mass) * (energy + mass) };
    double particle_momentum{ std::sqrt(std::max(square_momentum, 0.0)) };

    return energy / (particle_momentum * SPEED)
        * displacement_->FunctionToIntegral(energy);
}

/******************************************************************************
 *                            Utility ContRand                            *
 ******************************************************************************/

UtilityIntegralContRand::UtilityIntegralContRand(
    CrossSectionList cross, const ParticleDef&)
    : UtilityIntegral(cross, p_def)
{
}

double UtilityIntegralContRand::FunctionToIntegral(double energy)
{
    double sum = 0.0;
    for (const auto& cross : crosssections)
        sum += cross->CalculatedE2dx(energy);

    return displacement_->FunctionToIntegral(energy) * sum;
}

/******************************************************************************
 *                             Utility Scattering                             *
 ******************************************************************************/

UtilityIntegralScattering::UtilityIntegralScattering(
    CrossSectionList cross, const ParticleDef& p_def)
    : UtilityIntegral(cross, p_def)
    , mass(p_def.mass)
{
}

double UtilityIntegralScattering::FunctionToIntegral(double energy)
{
    double square_momentum = (energy - mass) * (energy + mass);
    double aux = energy / square_momentum;

    return displacement_->FunctionToIntegral(energy) * aux * aux;
}
