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
    coupledUnsLinGeomSolid

Description
    Mathematical model where linear geometry is assumed i.e. small strains and
    small rotations are assumed.

    The stress is calculated by the run-time selectable mechanical law.

    A block-coupled solution methodology is employed, in contrast to the
    segregated solution methodology employed in linGeomSolid.

    For more details, see:
    P. Cardiff, Z Tukovic, H. Jasak, and A. Ivanković. A Block-Coupled Finite
    Volume Methodology for Linear Elasticity and Unstructured Meshes, Computers
    and Structures, 2016, 10.1016/j.compstruc.2016.07.004.

Author
    Philip Cardiff, UCD.  All rights reserved.

SourceFiles
    coupledUnsLinGeomSolid.C

\*---------------------------------------------------------------------------*/

#ifndef coupledUnsLinGeomSolid_H
#define coupledUnsLinGeomSolid_H

#include "solidModel.H"
#ifdef FOAMEXTEND
    #include "solidPolyMesh.H"
    #include "BlockSolverPerformance.H"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace solidModels
{

/*---------------------------------------------------------------------------*\
                        Class coupledUnsLinGeomSolid Declaration
\*---------------------------------------------------------------------------*/

class coupledUnsLinGeomSolid
:
    public solidModel
{
    // Private data

#ifdef FOAMEXTEND
        //- The solidPolyMesh stores the additional addressing required for the
        //  extended implicit stencil
        solidPolyMesh extendedMesh_;

        //- Solution vector
        //  This is a field containing the cell displacements followed by the
        //  boundary face displacements
        vectorIOField solutionVec_;
#endif

        //- Stress surface field
        surfaceSymmTensorField sigmaf_;

        //- Surface displacement gradient
        surfaceTensorField gradDf_;

        //- Implicit stiffness; coefficient of the Laplacian term
        //  The value of this term only affects convergence and not the answer
        //  In the case of Hookean elasticity, (2*mu + lambda) is the optimum
        const volScalarField impK_;

        //- Implicit stiffness surface field
        const surfaceScalarField impKf_;

        //- Reciprocal of impK
        //  We divide by impK every time tractionBoundarySnGrad is called so we
        //  will store the reciprocal for effficiency
        const volScalarField rImpK_;

        //- The coupled discretisation assumes Hooke's law so we will store mu
        //  This poses a challenge as what should be done when a material other
        //  than linearElastic is specified; for now, explicit corrections can
        //  be performed
        //- Hooke's law second Lame parameter
        const surfaceScalarField muf_;

        //- Hooke's law first Lame parameter
        const surfaceScalarField lambdaf_;

    // Private Member Functions

        //- Disallow default bitwise copy construct
        coupledUnsLinGeomSolid(const coupledUnsLinGeomSolid&);

        //- Disallow default bitwise assignment
        void operator=(const coupledUnsLinGeomSolid&);


protected:

    // Protected member functions

        //- Return nonlinear geometry enumerator
        virtual nonLinearGeometry::nonLinearType nonLinGeom() const
        {
            return nonLinearGeometry::LINEAR_GEOMETRY;
        }


public:

    //- Runtime type information
    TypeName("coupledUnsLinearGeometry");

    // Constructors

        //- Construct from components
        coupledUnsLinGeomSolid
        (
            Time& runTime,
            const word& region = dynamicFvMesh::defaultRegion
        );


    // Destructor

        virtual ~coupledUnsLinGeomSolid()
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
