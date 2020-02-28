
#include "PROPOSAL/Logging.h"
#include <PROPOSAL/crossection/factories/PhotoPairFactory.h>

#include "PROPOSAL/propagation_utility/PropagationUtility.h"

#include "PROPOSAL/crossection/CrossSection.h"
#include "PROPOSAL/crossection/parametrization/Parametrization.h"

using namespace PROPOSAL;

/******************************************************************************
 *                            Propagation utility                              *
 ******************************************************************************/

Utility::Definition::Definition(std::shared_ptr<Crosssections> corsssection,
    std::shared_ptr<Scattering> scattering = nullptr,
    std::shared_ptr<InterpolationDef> inter_def = nullptr)
    : crosssection(crosssection)
    , scattering(scattering)
    , inter_def(inter_def)
    , mass(crosssection.first()
               .GetParticleDef()
               .mass); // if particle energy would be kinetic energy, this copy
                       // is redundant
{
    if (!inter_def) {
        log_warn("No interpolation definition defined. Integral will not be "
                 "approximate by interpolants. Performance will be poor.");

        displacement_calculator.reset(
            new UtilityIntegralDisplacement(crosssection));
        interaction_calculator.reset(
            new UtilityIntegralInteraction(crosssection));
        decay_calculator.reset(new UtilityIntegralDecay(crosssection))
    } else {
        displacement_calculator.reset(
            new UtilityInterpolantDisplacement(crosssection, inter_def));
        interaction_calculator.reset(
            new UtilityInterpolantInteraction(crosssection, inter_def));
        decay_calculator.reset(
            new UtilityInterpolantDecay(crosssection, inter_def))
    }

    if (!scattering) {
        log_debug("No scattering defined. Partilce will only be deflected if a "
                  "stochastic deflection is in the crosssection implemented.");
    }

    if (!cont_rand) {
        log_debug("No continous randomization used.");
    } else {
        if (!inter_def) {
            cont_rand.reset(new UtilityIntegralContRand(crosssection))
        } else {
            cont_rand.reset(
                new UtilityInterpolantContRand(crosssection, inter_def))
        }
    }

    if (!exact_time) {
        log_debug("No exact time calculator used.");
    }
}
else
{
    if (!inter_def) {
        cont_rand.reset(new UtilityIntegralTime(crosssection))
    } else {
        cont_rand.reset(new UtilityInterpolantTime(crosssection, inter_def))
    }
}
}

std::ostream& PROPOSAL::operator<<(
    std::ostream& os, PROPOSAL::Utility::Definition const& util_definition)
{
    std::stringstream ss;
    ss << " Utility Definition (" << &util_definition << ") ";
    os << Helper::Centered(60, ss.str()) << '\n';

    for (const auto& crosssection : util_definition->crosssection) {
        os << crosssection << std::endl;
    }
    if (scattering) {
        os << scattering << std::endl;
    };
    if (inter_def) {
        os << inter_def << std::endl;
    }
    if (cont_rand) {
        os << cont_rand << std::endl;
    }
    if (exact_time) {
        os << exact_time << std::endl;
    };
    os << Helper::Centered(60, "");
    return os;
}

// -------------------------------------------------------------------------
// // Constructors
// -------------------------------------------------------------------------

Utility::Utility(std::unique_ptr<Utility::Definition> utility_def)
    : utility_def(std::move(utility_def))
{
}

int Utility::TypeInteraction(
    const double energy, const std::array<double, 2> rnd)
{
    std::array<double, crosssections_.size()> rates;
    for (const auto& cross : crosssections)
        rates = crosssections->CalculatedNdx(energy, rnd[1]);

    double total_rate{ std::accumulate(rates.begin(), rates.end(), 0) };
    log_debug("Total rate = %f, total rate weighted = %f", total_rate,
        total_rate * rnd[0]);

    double rates_sum = 0;
    for (size_t i = 0; i < rates.size(); i++) {
        rates_sum += rates[i];
        if (rates_sum >= total_rate * rnd[0])
            return crosssection[i]->GetType();
    }

    throw std::logic_error("Something get wrong in total rate calculation.");
}

double Utility::EnergyStochasticloss(
    const int type, const double energy, const std::array<double, 2> rnd)
{
    for (const auto& cross : crosssections) {
        if (cross->GetType() == type)
            return cross->CalculateStochasticLoss(energy, rnd[0], rnd[1]);
    }
    throw std::logic_error(
        "Something get wrong with stochastic loss calculation.");
}

int Utility::EnergyDecay(const double energy, const double rnd)
{
    double rndd = -std::log(rnd);
    double rnddMin = 0;

    rnddMin = decay_calculator_->Calculate(energy, mass, rndd);

    // evaluating the energy loss
    if (rndd >= rnddMin || rnddMin <= 0)
        return mass;

    return decay_calculator->GetUpperLimit(energy, rndd);
}

double Utility::EnergyInteraction(const double energy, const double rnd)
{
    double rndi = -std::log(rnd);
    double rndiMin = 0;

    // solving the tracking integral
    rndiMin = interaction_calculator_->Calculate(energy, mass, rndi);

    if (rndi >= rndiMin || rndiMin <= 0)
        return mass;

    return interaction_calculator_->GetUpperLimit(energy, rndi);
}

double EnergyRandomize(const double initial_energy, const double final_energy, const double rnd)
{
    if (cont_rand) {
        return cont_rand->Randomize(initial_energy, final_energy, distance, rnd);
    } else {
        return final_energy;
    }
}

double Utility::ElapsedTime(const double initial_energy,
    const double final_energy, const double distance)
{
    if (exact_time) {
        return exact_time->Calculate(initial_energy, final_energy, distance);
    } else {
        return displacement / SPEED;
    }
}

Directions ScatterDirection(const double displacement, const double initial_energy,
    const double final_energy, Vector3D& position, Vector3D& direction)
{
    return scattering->Scatter(
        displacement, initial_energy, final_energy, position, direction);
}

double LengthContinuous(const double initial_energy, const double final_energy, const double border_length)
{
    displacement_calculator_->Calculate(initial_energy, final_energy, border_length);
}
