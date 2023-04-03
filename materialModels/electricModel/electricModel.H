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
    electricModel

Description
    electric material properties for solids.

Author
    Hrvoje Jasak, Wikki Ltd.  All rights reserved.

SourceFiles
    electricModel.C

\*---------------------------------------------------------------------------*/

#ifndef electricModel_H
#define electricModel_H

#include "IOdictionary.H"
#include "typeInfo.H"
#include "runTimeSelectionTables.H"
#include "volFields.H"
#include "tmp.H"
#include "electricLaw.H"
#include "Switch.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                         Class electricModel Declaration
\*---------------------------------------------------------------------------*/

class electricModel
:
    public IOdictionary
{
    // Private data

        //- Const reference to the mesh
        const fvMesh& mesh_;

        //- electric law
        autoPtr<electricLaw> lawPtr_;


    // Private Member Functions

        //- Disallow copy construct
        electricModel(const electricModel&);

        //- Disallow default bitwise assignment
        void operator=(const electricModel&);


public:

    //- Runtime type information
    TypeName("electricModel");

    // Constructors

        //- Construct from dictionary
        electricModel(const fvMesh& mesh);


    // Destructor

        virtual ~electricModel()
        {}


    // Member Functions

        //- Return const reference to the mesh
        const fvMesh& mesh() const
        {
            return mesh_;
        }

        //- Return electric law
        const electricLaw& law() const
        {
            return lawPtr_();
        }

        //- Return specific heat capacity
        tmp<volScalarField> C2() const
        {
            return lawPtr_->C2();
        }

        //- Return electric conductivity
        tmp<volScalarField> k2() const
        {
            return lawPtr_->k2();
        }

        //- Return specific heat capacity
        tmp<volScalarField> C() const
        {
            return lawPtr_->C();
        }

        //- Return electric conductivity
        tmp<volScalarField> k() const
        {
            return lawPtr_->k();
        }

        //- Read electricProperties dictionary
        virtual bool read();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
