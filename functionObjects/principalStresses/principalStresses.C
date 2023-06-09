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

\*----------------------------------------------------------------------------*/

#include "principalStresses.H"
#include "addToRunTimeSelectionTable.H"
#include "volFields.H"
#include "pointFields.H"
#include "principalStressFields.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(principalStresses, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        principalStresses,
        dictionary
    );
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

bool Foam::principalStresses::writeData()
{
    if (runTime_.outputTime())
    {
        // Lookup stress tensor
        const volSymmTensorField* sigmaPtr = NULL;
        if (mesh_.foundObject<volSymmTensorField>("sigma"))
        {
            sigmaPtr = &(mesh_.lookupObject<volSymmTensorField>("sigma"));
        }
        else if (mesh_.foundObject<volSymmTensorField>("sigmaCauchy"))
        {
            sigmaPtr = &(mesh_.lookupObject<volSymmTensorField>("sigmaCauchy"));
        }
        const volSymmTensorField& sigma = *sigmaPtr;;

        // Calculate and write principal stress fields
        writePrincipalStressFields(sigma);
    }

    return true;
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::principalStresses::principalStresses
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    functionObject(name),
    name_(name),
    runTime_(t),
    mesh_
    (
        runTime_.lookupObject<fvMesh>
        (
            dict.lookupOrDefault<word>("region", "region0")
        )
    )
{
    Info<< "Creating " << this->name() << " function object" << endl;
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::principalStresses::start()
{
    if (runTime_.outputTime())
    {
        return writeData();
    }

    return true;
}


#if FOAMEXTEND > 40
bool Foam::principalStresses::execute(const bool forceWrite)
#else
bool Foam::principalStresses::execute()
#endif
{
    if (runTime_.outputTime())
    {
        return writeData();
    }

    return true;
}


bool Foam::principalStresses::read(const dictionary& dict)
{
    return true;
}

#ifdef OPENFOAMESIORFOUNDATION
bool Foam::principalStresses::write()
{
    return writeData();
}
#endif

// ************************************************************************* //
