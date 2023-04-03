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
    electricLaw

Description
    electric material properties for solids.

Author
    Hrvoje Jasak, Wikki Ltd.  All rights reserved.
    Philip Cardiff, UCD.  All rights reserved.

SourceFiles
    electricLaw.C
    newelectricLaw.C

\*---------------------------------------------------------------------------*/

#ifndef electricLaw_H
#define electricLaw_H

#include "IOdictionary.H"
#include "typeInfo.H"
#include "runTimeSelectionTables.H"
#include "volFields.H"
#include "tmp.H"
#include "autoPtr.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                         Class electricLaw Declaration
\*---------------------------------------------------------------------------*/

class electricLaw
{
    // Private data

        //- Name
        const word name_;

        //- Reference to mesh
        const fvMesh& mesh_;


    // Private Member Functions

        //- Disallow copy construct
        electricLaw(const electricLaw&);

        //- Disallow default bitwise assignment
        void operator=(const electricLaw&);


protected:

        //- Return reference to mesh
        const fvMesh& mesh() const
        {
            return mesh_;
        }

public:

    //- Runtime type information
    TypeName("electricLaw");


    // Declare run-time constructor selection table

        declareRunTimeSelectionTable
        (
            autoPtr,
            electricLaw,
            dictionary,
            (
                const word name,
                const fvMesh& mesh,
                const dictionary& dict
            ),
            (name, mesh, dict)
        );


    // Selectors

        //- Return a reference to the selected electric model
        static autoPtr<electricLaw> New
        (
            const word& name,
            const fvMesh& mesh,
            const dictionary& dict
        );


    // Constructors

        //- Construct from dictionary
        electricLaw
        (
            const word& name,
            const fvMesh& mesh,
            const dictionary& dict
        );


    // Destructor

        virtual ~electricLaw()
        {}


    // Member Functions

        //- Return name
        const word& name() const
        {
            return name_;
        }

        //- Return specific heat capacity
        virtual tmp<volScalarField> C2() const = 0;

        //- Return electric conductivity
        virtual tmp<volScalarField> k2() const = 0;

        //- Return specific heat capacity
        virtual tmp<volScalarField> C() const = 0;

        //- Return electric conductivity
        virtual tmp<volScalarField> k() const = 0;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
