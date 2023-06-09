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

Class
    linGeomPressureVelocitySolid

Description
    Mathematical model where nonlinear geometry is assumed i.e. large strains
    are assumed, and the total displacement and hydrostatic pressure are the
    primary unknowns. The displacement-pressure coupling is treated implicitly
    in a block-coupled manner. The transpose Laplacian of the momentum
    equations is assembled by decomposing the linear version in the tangential
    and normal components.

    Rhie-Chow stabilisation is applied directly to the pressure and displcement
    fields.

    The stress is calculated by the run-time selectable mechanical law.

Author
    Zeljko Tukovic, FSB Zagreb.  All rights reserved.
    Philip Cardiff, UCD.  All rights reserved.

SourceFiles
    coupledNonLinGeomPressureDisplacementSolid.C

\*---------------------------------------------------------------------------*/

#ifndef coupledNonLinGeomPressureDisplacementSolid_H
#define coupledNonLinGeomPressureDisplacementSolid_H

#include "solidModel.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "pointFields.H"
#include "volVectorNFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace solidModels
{

/*---------------------------------------------------------------------------*\
                        Class linGeomTotalDispSolid Declaration
\*---------------------------------------------------------------------------*/

class coupledNonLinGeomPressureDisplacementSolid
:
    public solidModel
{
    // Private data

        //- Cauchy (true) stress surface field
        surfaceSymmTensorField sigmaf_;

        //- Surface displacement gradient
        surfaceTensorField gradDf_;

        //- Total deformation gradient
        volTensorField F_;

        //- Total deformation gradient surface field
        surfaceTensorField Ff_;

        //- Inverse of the total deformation gradient
        volTensorField Finv_;

        //- Inverse of the total deformation gradient
        surfaceTensorField Finvf_;

        //- Jacobian of total deformation gradient
        volScalarField J_;

        //- Surface Jacobian of total deformation gradient
        surfaceScalarField Jf_;

        //- Implicit stiffness; coefficient of the Laplacian term
        //  The value of this term only affects convergence and not the answer
        //  In the case of Hookean elasticity, (2*mu + lambda) is the optimum
        const volScalarField impK_;

        //- Implicit stiffness surface field
        surfaceScalarField impKf_;

        //- Reciprocal of impK
        //  We divide by impK every time tractionBoundarySnGrad is called so we
        //  will store the reciprocal for effficiency
        const volScalarField rImpK_;

        //- Hydrostatic pressure
        volScalarField p_;

        //- Surface hydrostatic pressure
        surfaceScalarField pf_;

        //- Gradient of hydrostatic pressure
        volVectorField gradp_;

        //- Block vector field for displacement (first entry) and pressure
        //  (second entry)
        volVector4Field Dp_;

        //- Scale factor for Rhie-Chow smoothing term in pressure calculation
        const scalar pressureRhieChowScaleFac_;

        //- Coorection for the finalisation of the momentum equation
        surfaceVectorField correction_;

        //- Tangential component of the normal displacement gradient
        surfaceVectorField tGradDnf_;

        //- Normal component of the normal displacement gradient
        surfaceScalarField nGradDnf_;

        //- Non-linear surface force field
        surfaceVectorField nonLinearForcef_;

        //- Linear surface force field
        surfaceVectorField linearForcef_;

        //- Difusion coeff. for pressure equation
        volScalarField AU_;

        //- Ratio between convective operator and diag. coeffs
        volVectorField HbyA_;

        //- Displacement "flux" surface field
        surfaceScalarField phi_;

        //- Predict new time-step fields using the velocity field
        const Switch predictor_;

    // Private Member Functions

        //- Predict the fields for the next time-step based on the
        //  previous time-steps
        void predict();

        //- Disallow default bitwise copy construct
        coupledNonLinGeomPressureDisplacementSolid
        (
            const coupledNonLinGeomPressureDisplacementSolid&
        );

        //- Disallow default bitwise assignment
        void operator=(const coupledNonLinGeomPressureDisplacementSolid&);


protected:

    // Protected member functions

        //- Return nonlinear geometry enumerator
        virtual nonLinearGeometry::nonLinearType nonLinGeom() const
        {
            return nonLinearGeometry::TOTAL_LAGRANGIAN;
        }

public:

    //- Runtime type information
    TypeName("coupledNonLinearGeometryPressureDisplacement");

    // Constructors

        //- Construct from components
        coupledNonLinGeomPressureDisplacementSolid
        (
            Time& runTime,
            const word& region = dynamicFvMesh::defaultRegion
        );

    // Destructor

        virtual ~coupledNonLinGeomPressureDisplacementSolid()
        {}


    // Member Functions

        // Access

            //- Each solidModel must indicate whether D or DD is the primary
            //  solution variable
            virtual volVectorField& solutionD()
            {
                // This model solves for D
                return D();
            }

        // Edit

            //- Evolve the solid solver and solve the mathematical model
            virtual bool evolve();

            //- Traction boundary surface normal gradient
            virtual tmp<vectorField> tractionBoundarySnGrad
            (
                const vectorField& traction,
                const scalarField& pressure,
                const fvPatch& patch
            ) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace solidModel

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
