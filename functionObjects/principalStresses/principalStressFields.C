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

#include "principalStressFields.H"

// * * * * * * * * * * * * * * * * * * Functions * * * * * * * * * * * * * * //

void Foam::calculateEigenValues
(
    const symmTensor& sigma,
    scalar& sigmaMax,
    scalar& sigmaMid,
    scalar& sigmaMin,
    vector& sigmaMaxDir,
    vector& sigmaMidDir,
    vector& sigmaMinDir
)
{
    const vector eValues = eigenValues(sigma);
    const tensor eVectors = eigenVectors(sigma);

    label iMax = -1;
    label iMid = -1;
    label iMin = -1;
    const scalar a = eValues[0];
    const scalar b = eValues[1];
    const scalar c = eValues[2];

    if (a < b)
    {
        if (a < c)
        {
            if (b < c)
            {
                // a < b
                // a < c
                // b < c
                // a < b < c
                iMin = 0;
                iMid = 1;
                iMax = 2;
            }
            else
            {
                // a < b
                // a < c
                // b > c
                // a < c < b
                iMin = 0;
                iMid = 2;
                iMax = 1;
            }
        }
        else
        {
            // a < b
            // a > c
            // c < a < b
            iMin = 2;
            iMid = 0;
            iMax = 1;
        }
    }
    else
    {
        if (b < c)
        {
            if (a < c)
            {
                // a > b
                // b < c
                // a < c
                // b < a < c
                iMin = 1;
                iMid = 0;
                iMax = 2;
            }
            else
            {
                // a > b
                // b < c
                // a > c
                // b < c < a
                iMin = 1;
                iMid = 2;
                iMax = 0;
            }
        }
        else
        {
            // a > b
            // b > c
            // c < b < a
            iMin = 2;
            iMid = 1;
            iMax = 0;
        }
    }

    sigmaMax = eValues[iMax];
    sigmaMid = eValues[iMid];
    sigmaMin = eValues[iMin];
    sigmaMaxDir = vector(eVectors[3*iMax], eVectors[3*iMax + 1], eVectors[3*iMax + 2]);
    sigmaMidDir = vector(eVectors[3*iMid], eVectors[3*iMid + 1], eVectors[3*iMid + 2]);
    sigmaMinDir = vector(eVectors[3*iMin], eVectors[3*iMin + 1], eVectors[3*iMin + 2]);
}


void Foam::writePrincipalStressFields(const volSymmTensorField& sigma)
{
    const fvMesh& mesh = sigma.mesh();
    const Time& runTime = mesh.time();

    // Maximum (most positive/tensile) principal stress
    volScalarField sigmaMax
    (
        IOobject
        (
            "sigmaMax",
            runTime.timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionedScalar("sigmaMax", dimPressure, 0.0)
    );

    volVectorField sigmaMaxDir
    (
        IOobject
        (
            "sigmaMaxDir",
            runTime.timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionedVector("sigmaMaxDir", dimPressure, vector::zero)
    );

    // Middle principal stress
    volScalarField sigmaMid
    (
        IOobject
        (
            "sigmaMid",
            runTime.timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionedScalar("sigmaMid", dimPressure, 0.0)
    );

    volVectorField sigmaMidDir
    (
        IOobject
        (
            "sigmaMidDir",
            runTime.timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionedVector("sigmaMidDir", dimPressure, vector::zero)
    );

    // Minimum principal (most negative/compressive) stress
    volScalarField sigmaMin
    (
        IOobject
        (
            "sigmaMin",
            runTime.timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionedScalar("sigmaMin", dimPressure, 0.0)
    );

    volVectorField sigmaMinDir
    (
        IOobject
        (
            "sigmaMinDir",
            runTime.timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionedVector("sigmaMinDir", dimPressure, vector::zero)
    );

    // References to internalFields for efficiency
    const symmTensorField& sigmaI = sigma.internalField();
#ifdef OPENFOAMESIORFOUNDATION
    scalarField& sigmaMaxI = sigmaMax.primitiveFieldRef();
    scalarField& sigmaMidI = sigmaMid.primitiveFieldRef();
    scalarField& sigmaMinI = sigmaMin.primitiveFieldRef();
    vectorField& sigmaMaxDirI = sigmaMaxDir.primitiveFieldRef();
    vectorField& sigmaMidDirI = sigmaMidDir.primitiveFieldRef();
    vectorField& sigmaMinDirI = sigmaMinDir.primitiveFieldRef();
#else
    scalarField& sigmaMaxI = sigmaMax.internalField();
    scalarField& sigmaMidI = sigmaMid.internalField();
    scalarField& sigmaMinI = sigmaMin.internalField();
    vectorField& sigmaMaxDirI = sigmaMaxDir.internalField();
    vectorField& sigmaMidDirI = sigmaMidDir.internalField();
    vectorField& sigmaMinDirI = sigmaMinDir.internalField();
#endif

    forAll (sigmaI, cellI)
    {
        calculateEigenValues
        (
            sigmaI[cellI],
            sigmaMaxI[cellI],
            sigmaMidI[cellI],
            sigmaMinI[cellI],
            sigmaMaxDirI[cellI],
            sigmaMidDirI[cellI],
            sigmaMinDirI[cellI]
        );
    }

    forAll(sigma.boundaryField(), patchI)
    {
        if
        (
            !sigma.boundaryField()[patchI].coupled()
          && mesh.boundaryMesh()[patchI].type() != "empty"
        )
        {
            const symmTensorField& pSigma = sigma.boundaryField()[patchI];
#ifdef OPENFOAMESIORFOUNDATION
            scalarField& pSigmaMax = sigmaMax.boundaryFieldRef()[patchI];
            scalarField& pSigmaMid = sigmaMid.boundaryFieldRef()[patchI];
            scalarField& pSigmaMin = sigmaMin.boundaryFieldRef()[patchI];
            vectorField& pSigmaMaxDir = sigmaMaxDir.boundaryFieldRef()[patchI];
            vectorField& pSigmaMidDir = sigmaMidDir.boundaryFieldRef()[patchI];
            vectorField& pSigmaMinDir = sigmaMinDir.boundaryFieldRef()[patchI];
#else
            scalarField& pSigmaMax = sigmaMax.boundaryField()[patchI];
            scalarField& pSigmaMid = sigmaMid.boundaryField()[patchI];
            scalarField& pSigmaMin = sigmaMin.boundaryField()[patchI];
            vectorField& pSigmaMaxDir = sigmaMaxDir.boundaryField()[patchI];
            vectorField& pSigmaMidDir = sigmaMidDir.boundaryField()[patchI];
            vectorField& pSigmaMinDir = sigmaMinDir.boundaryField()[patchI];
#endif

            forAll(pSigmaMax, faceI)
            {
                calculateEigenValues
                (
                    pSigma[faceI],
                    pSigmaMax[faceI],
                    pSigmaMid[faceI],
                    pSigmaMin[faceI],
                    pSigmaMaxDir[faceI],
                    pSigmaMidDir[faceI],
                    pSigmaMinDir[faceI]
                );
            }
        }
    }

    sigmaMax.correctBoundaryConditions();
    sigmaMid.correctBoundaryConditions();
    sigmaMin.correctBoundaryConditions();
    sigmaMaxDir.correctBoundaryConditions();
    sigmaMidDir.correctBoundaryConditions();
    sigmaMinDir.correctBoundaryConditions();

    // Write fields
    Info<< "    Writing sigmaMax" << nl
        << "    Writing sigmaMid" << nl
        << "    Writing sigmaMin" << nl
        << "    Writing sigmaMaxDir" << nl
        << "    Writing sigmaMidDir" << nl
        << "    Writing sigmaMinDir" << endl;

    sigmaMax.write();
    sigmaMid.write();
    sigmaMin.write();
    sigmaMaxDir.write();
    sigmaMidDir.write();
    sigmaMinDir.write();

    Info<< "Principal stresses: max = " << gMax(mag(sigmaMax)()) << endl;
}


// ************************************************************************* //
