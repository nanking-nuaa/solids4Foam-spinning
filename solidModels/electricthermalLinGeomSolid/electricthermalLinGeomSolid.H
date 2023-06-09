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
    electricthermalLinGeomSolid

Description
    Mathematical model where linear geometry is assumed i.e. small strains and
    small rotations are assumed.

    The heat equation and momentum equation are solved in a strongly coupled
    fashion i.e. just like in solidDisplacementFoam.

    The stress is calculated by the run-time selectable mechanical law.

Author
    Philip Cardiff, UCD.  All rights reserved.

SourceFiles
    electricthermalLinGeomSolid.C

\*---------------------------------------------------------------------------*/

#ifndef electricthermalLinGeomSolid_H
#define electricthermalLinGeomSolid_H

#include "solidModel.H"
//#include "thermalModel.H"
#include "electricModel.H"
#ifdef OPENFOAMESIORFOUNDATION
    #include "SolverPerformance.H"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace solidModels
{

/*---------------------------------------------------------------------------*\
                        Class electricthermalLinGeomSolid Declaration
\*---------------------------------------------------------------------------*/

class electricthermalLinGeomSolid
:
    public solidModel
{
    // Private data

        //- Electric model //
        electricModel electric_;   

        //- The electric model parameter2//
        volScalarField k2_;   

        //- The electric model parameter1//
        volScalarField CC_;  

	//- Electric potenial //
	volScalarField G_;

	//- Thermal model
 //       thermalModel thermal_;

        //- The product of density and specific heat
        volScalarField rhoC_;

        //- Thermal conductivity
        volScalarField k_;

        //- Temperature
        volScalarField T_;

        //- Temperature gradient
        volVectorField gradT_;

        //- Absolute temperature tolerance (in degrees)
        const scalar absTTol_;

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

    // Private Member Functions

        //- Check if the momentum equation is converged /*****need to be modified?
        bool converged
        (
            const int iCorr,
#ifdef OPENFOAMESIORFOUNDATION
            const SolverPerformance<vector>& solverPerfD,
            const SolverPerformance<scalar>& solverPerfp,
#else
            const lduSolverPerformance& solverPerfD,
            const lduSolverPerformance& solverPerfT,
 //           const lduSolverPerformance& solverPerfG,
#endif
            const volVectorField& D,
            const volScalarField& T
//            const volScalarField& G
        );

        //- Disallow default bitwise copy construct
        electricthermalLinGeomSolid(const electricthermalLinGeomSolid&);

        //- Disallow default bitwise assignment
        void operator=(const electricthermalLinGeomSolid&);


protected:

    // Protected member functions

        //- Return nonlinear geometry enumerator
        virtual nonLinearGeometry::nonLinearType nonLinGeom() const
        {
            return nonLinearGeometry::LINEAR_GEOMETRY;
        }


public:

    //- Runtime type information
    TypeName("electricthermalLinearGeometry");

    // Constructors

        //- Construct from components
        electricthermalLinGeomSolid
        (
            Time& runTime,
            const word& region = dynamicFvMesh::defaultRegion
        );


    // Destructor

        virtual ~electricthermalLinGeomSolid()
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

            //- Return const access to the cell temperature field
            const volScalarField& T() const
            {
                return T_;
            }

            //- Return non-const access to the cell temperature field
            volScalarField& T()
            {
                return T_;
            }

            //- Return const access to the cell electric potenial field //
            const volScalarField& G() const
            {
                return G_;
            }

            //- Return non-const access to the cell electric potenial field //
            volScalarField& G()
            {
                return G_;
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

            //- Write fields
            virtual void writeFields(const Time& runTime);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace solidModel

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
