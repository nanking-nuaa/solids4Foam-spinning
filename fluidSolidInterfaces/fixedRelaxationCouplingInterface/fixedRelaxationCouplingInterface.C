/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

\*---------------------------------------------------------------------------*/

#include "fixedRelaxationCouplingInterface.H"
#include "addToRunTimeSelectionTable.H"
#include "movingWallPressureFvPatchScalarField.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

namespace fluidSolidInterfaces
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(fixedRelaxationCouplingInterface, 0);
addToRunTimeSelectionTable
(
    physicsModel, fixedRelaxationCouplingInterface, fluidSolidInteraction
);
addToRunTimeSelectionTable
(
    fluidSolidInterface, fixedRelaxationCouplingInterface, dictionary
);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

fixedRelaxationCouplingInterface::fixedRelaxationCouplingInterface
(
    Time& runTime,
    const word& region
)
:
    fluidSolidInterface(typeName, runTime, region),
    relaxationFactor_
    (
        fsiProperties().lookupOrDefault<scalar>("relaxationFactor", 0.01)
    ),
    predictSolid_(fsiProperties().lookupOrDefault<bool>("predictSolid", true))
{}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool fixedRelaxationCouplingInterface::evolve()
{
    initializeFields();

    updateInterpolatorAndGlobalPatches();

    scalar residualNorm = 0;

    if (predictSolid_)
    {
        updateForce();

        solid().evolve();

        residualNorm =
            updateResidual();
    }

    do
    {
        outerCorr()++;

        // Transfer the displacement from the solid to the fluid
        updateDisplacement();

        // Move the fluid mesh
        moveFluidMesh();

        // Solve fluid
        fluid().evolve();

        // Transfer the force from the fluid to the solid
        updateForce();

        // Solve solid
        solid().evolve();

        // Calculate the FSI residual
        residualNorm = updateResidual();

        // Optional: write residuals to file
        if (writeResidualsToFile() && Pstream::master())
        {
            residualFile()
                << runTime().value() << " "
                << outerCorr() << " "
                << residualNorm << endl;
        }
    }
    while (residualNorm > outerCorrTolerance() && outerCorr() < nOuterCorr());

    solid().updateTotalFields();

    // Optional: correct fluid mesh to avoid build-up of interface position
    // errors
    if (additionalMeshCorrection())
    {
        // Transfer the displacement from the solid to the fluid, where we will
        // use no relaxation; in that way, we can force the solid and fluid
        // interfaces to stay aligned
        forAll(fluid().globalPatches(), interfaceI)
        {
            fluidZonesPointsDisplsPrev()[interfaceI] =
                fluidZonesPointsDispls()[interfaceI];

            fluidZonesPointsDispls()[interfaceI] += residuals()[interfaceI];
        }

        // Move the fluid mesh
        moveFluidMesh();
    }

    return 0;
}


void fixedRelaxationCouplingInterface::updateDisplacement()
{
    Info<< nl << "Time = " << fluid().runTime().timeName()
        << ", iteration: " << outerCorr() << endl;

    Info<< "Current fsi under-relaxation factor: "
        << relaxationFactor_ << endl;

    forAll(fluid().globalPatches(), interfaceI)
    {
        fluidZonesPointsDisplsPrev()[interfaceI] =
            fluidZonesPointsDispls()[interfaceI];

        fluidZonesPointsDispls()[interfaceI] +=
            relaxationFactor_*residuals()[interfaceI];
    }

    // Update movingWallPressure boundary conditions, if found
    fluidSolidInterface::updateMovingWallPressureAcceleration();

    // Update elasticWallPressure boundary conditions, if found
    fluidSolidInterface::updateElasticWallPressureAcceleration();

    // Make sure that displacement on all processors is equal to one
    // calculated on master processor
    fluidSolidInterface::syncFluidZonePointsDispl(fluidZonesPointsDispls());
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fluidSolidInterfaces

} // End namespace Foam

// ************************************************************************* //
