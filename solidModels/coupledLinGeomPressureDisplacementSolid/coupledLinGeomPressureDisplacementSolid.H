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
    Mathematical model where linear geometry is assumed i.e. small strains and
    small rotations are assumed, and the total displacement and hydrostatic
    pressure are the primary unknowns. The displacement-pressure coupling is
    treated implicitly in a block-coupled manner.

    Rhie-Chow stabilisation is applied directly to the pressure field.

    The stress is calculated by the run-time selectable mechanical law.

Author
    Philip Cardiff, UCD.  All rights reserved.

SourceFiles
    linGeomPressureVelocitySolid.C

\*---------------------------------------------------------------------------*/

#ifndef coupledLinGeomPressureDisplacementSolid_H
#define coupledLinGeomPressureDisplacementSolid_H

#include "solidModel.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "pointFields.H"
#include "uniformDimensionedFields.H"
#ifdef FOAMEXTEND
    #include "volVectorNFields.H"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace solidModels
{

/*---------------------------------------------------------------------------*\
                        Class linGeomTotalDispSolid Declaration
\*---------------------------------------------------------------------------*/

class coupledLinGeomPressureDisplacementSolid
:
    public solidModel
{
    // Private data

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

        //- Bulk modulus field
        const volScalarField k_;

        //- Reciprocal of bulk modulus field, stored for efficiency
        const volScalarField rK_;

        //- Hydrostatic pressure
        volScalarField p_;

        //- Gradient of hydrostatic pressure
        volVectorField gradp_;

        //- Block vector field for displacement (first entry) and pressure
        //  (second entry)
#ifdef FOAMEXTEND
        volVector4Field Dp_;
#endif

        //- Scale factor for Rhie-Chow smoothing term in pressure calculation
        const scalar pressureRhieChowScaleFac_;


    // Private Member Functions

        //- Disallow default bitwise copy construct
        coupledLinGeomPressureDisplacementSolid
        (
            const coupledLinGeomPressureDisplacementSolid&
        );

        //- Disallow default bitwise assignment
        void operator=(const coupledLinGeomPressureDisplacementSolid&);


protected:

    // Protected member functions

        //- Return nonlinear geometry enumerator
        virtual nonLinearGeometry::nonLinearType nonLinGeom() const
        {
            return nonLinearGeometry::LINEAR_GEOMETRY;
        }


public:

    //- Runtime type information
    TypeName("coupledLinearGeometryPressureDisplacement");

    // Constructors

        //- Construct from components
        coupledLinGeomPressureDisplacementSolid
        (
            Time& runTime,
            const word& region = dynamicFvMesh::defaultRegion
        );

    // Destructor

        virtual ~coupledLinGeomPressureDisplacementSolid()
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
