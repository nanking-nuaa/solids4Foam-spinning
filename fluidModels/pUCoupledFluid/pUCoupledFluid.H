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
    pUCoupledFluid

Description
    pUCoupledFoam fluid model

Author
    Philip Cardiff, UCD. All rights reserved.
    Zeljko Tukovic, FSB Zagreb.  All rights reserved.

SourceFiles
    pUCoupledFluid.C

\*---------------------------------------------------------------------------*/

#ifndef pUCoupledFluid_H
#define pUCoupledFluid_H

#include "fluidModel.H"
#include "fvCFD.H"
#ifndef OPENFOAMESIORFOUNDATION
    #include "fvBlockMatrix.H"
    #include "singlePhaseTransportModel.H"
    #include "RASModel.H"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace fluidModels
{

/*---------------------------------------------------------------------------*\
                        Class pUCoupledFluid Declaration
\*---------------------------------------------------------------------------*/

class pUCoupledFluid
:
    public fluidModel
{
    // Private data

#ifndef OPENFOAMESIORFOUNDATION
        //- Transport properties model
        singlePhaseTransportModel laminarTransport_;

        //- Turbulence model
        autoPtr<incompressible::RASModel> turbulence_;

        //- Block vector field for velocity and pressure
        volVector4Field Up_;

        //- Reciprocal of momentum diagonal coefficient
        volScalarField rAU_;

        // Transport properties dictionary
        IOdictionary transportProperties_;

        //- Density
        dimensionedScalar rho_;

        //- Maximum residual for convergence checking
        scalar maxResidual_;

        //- Convergence tolerance
        scalar convergenceCriterion_;
#endif

    // Private Member Functions

        //- Disallow default bitwise copy construct
        pUCoupledFluid(const pUCoupledFluid&);

        //- Disallow default bitwise assignment
        void operator=(const pUCoupledFluid&);


public:

    //- Runtime type information
    TypeName("pUCoupledFluid");


    // Constructors

        //- Construct from components
        pUCoupledFluid
        (
            Time& runTime,
            const word& region = dynamicFvMesh::defaultRegion
        );


    // Destructor

        virtual ~pUCoupledFluid()
        {}


    // Member Functions

        // Access

            //- Patch viscous force (N/m2)
            virtual tmp<vectorField> patchViscousForce
            (
                const label patchID
            ) const;

            //- Patch pressure force (N/m2)
            virtual tmp<scalarField> patchPressureForce
            (
                const label patchID
            ) const;

        // Edit

            //- Evolve the fluid model
            virtual bool evolve();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fluidModels
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
