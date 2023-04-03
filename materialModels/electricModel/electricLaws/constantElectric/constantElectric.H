/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | foam-extend: Open Source CFD
   \\    /   O peration     | Version:     4.0
    \\  /    A nd           | Web:         http://www.foam-extend.org
     \\/     M anipulation  | For copyright notice see file Copyright
-------------------------------------------------------------------------------
License
    This file is part of foam-extend.

    foam-extend is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    foam-extend is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with foam-extend.  If not, see <http://www.gnu.org/licenses/>.

Class
    constantElectric

Description
    Constant Electric properties

Author
    Hrvoje Jasak, Wikki Ltd.  All rights reserved.
    Philip Cardiff, UCD.  All rights reserved.

SourceFiles
    constantElectric.C

\*---------------------------------------------------------------------------*/

#ifndef constantElectric_H
#define constantElectric_H

#include "electricLaw.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                         Class constantElectric Declaration
\*---------------------------------------------------------------------------*/

class constantElectric
:
    public electricLaw
{
    // Private data

        //- Specific heat capacity
        dimensionedScalar C2_;

        //- Electric conductivity
        dimensionedScalar k2_;

        //- Specific heat capacity
        dimensionedScalar C_;

        //- Electric conductivity
        dimensionedScalar k_;

    // Private Member Functions

        //- Disallow default bitwise copy construct
        constantElectric(const constantElectric&);

        //- Disallow default bitwise assignment
        void operator=(const constantElectric&);


public:

    //- Runtime type information
    TypeName("constant");

    // Static data members


    // Constructors

        //- Construct from dictionary
        constantElectric
        (
            const word& name,
            const fvMesh& mesh,
            const dictionary& dict
        );


    // Destructor

        virtual ~constantElectric();


    // Member Functions

        //- Return specific heat capacity
        virtual tmp<volScalarField> C2() const;

        //- Return Electric conductivity
        virtual tmp<volScalarField> k2() const;

        //- Return specific heat capacity
        virtual tmp<volScalarField> C() const;

        //- Return Electric conductivity
        virtual tmp<volScalarField> k() const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
