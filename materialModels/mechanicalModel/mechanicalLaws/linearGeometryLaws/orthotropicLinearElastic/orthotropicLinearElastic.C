/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | foam-extend: Open Source CFD
   \\    /   O peration     |
    \\  /    A nd           | For copyright notice see file Copyright
     \\/     M anipulation  |
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

\*---------------------------------------------------------------------------*/

#include "orthotropicLinearElastic.H"
#include "addToRunTimeSelectionTable.H"
#include "transformField.H"
#include "transformGeometricField.H"
#include "fvc.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(orthotropicLinearElastic, 0);
    addToRunTimeSelectionTable
    (
        mechanicalLaw, orthotropicLinearElastic, linGeomMechLaw
    );
}


// * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * * //

void Foam::orthotropicLinearElastic::makeElasticC() const
{
    if (elasticCPtr_)
    {
        FatalErrorIn
        (
            "void Foam::orthotropicLinearElastic::makeElasticC()"
        )   << "pointer already set" << abort(FatalError);
    }

    // Set local fourth order tensor stiffness

    const dimensionedScalar J =
        (
            1.0 - nu12_*nu21_ - nu23_*nu32_ - nu31_*nu13_ - 2*nu21_*nu32_*nu13_
        )/(E1_*E2_*E3_);

    const dimensionedScalar A11 = (1.0 - nu23_*nu32_)/(J*E2_*E3_);
    const dimensionedScalar A22 = (1.0 - nu13_*nu31_)/(J*E1_*E3_);
    const dimensionedScalar A33 = (1.0 - nu21_*nu12_)/(J*E2_*E1_);
    const dimensionedScalar A12 = (nu12_ + nu32_*nu13_)/(J*E1_*E3_);
    const dimensionedScalar A31 = (nu31_ + nu21_*nu32_)/(J*E2_*E3_);
    const dimensionedScalar A23 = (nu23_ + nu21_*nu13_)/(J*E1_*E2_);
    const dimensionedScalar A44 =  2.0*G12_;
    const dimensionedScalar A55 =  2.0*G23_;
    const dimensionedScalar A66 =  2.0*G31_;

    // Set elasticC in local coordinate system and then we will rotate it to the
    // global coordinate system

    elasticCPtr_ =
        new volSymmTensor4thOrderField
        (
            IOobject
            (
                "elasticC",
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh(),
            dimensionedSymmTensor4thOrder
            (
                "localElasticC",
                dimPressure,
                symmTensor4thOrder
                (
                    A11.value(), A12.value(), A31.value(),
                    A22.value(), A23.value(),
                    A33.value(),
                    A44.value(),
                    A55.value(),
                    A66.value()
                )
            )
        );

    volSymmTensor4thOrderField& C = *elasticCPtr_;

    // Calculate rotating matrix from local directions to global directions
    const volTensorField matDir =
        matDirX_*matDirX_ + matDirY_*matDirY_ + matDirZ_*matDirZ_;

    // Rotate C from local directions to global directions
    C = transform(matDir, C);
}


const Foam::volSymmTensor4thOrderField&
Foam::orthotropicLinearElastic::elasticC() const
{
    if (!elasticCPtr_)
    {
        makeElasticC();
    }

    return *elasticCPtr_;
}


void Foam::orthotropicLinearElastic::makeElasticCf() const
{
    if (elasticCfPtr_)
    {
        FatalErrorIn
        (
            "void Foam::orthotropicLinearElastic::makeElasticCf()"
        )   << "pointer already set" << abort(FatalError);
    }

    // Set local fourth order tensor stiffness

    const dimensionedScalar J =
        (
            1.0 - nu12_*nu21_ - nu23_*nu32_ - nu31_*nu13_ - 2*nu21_*nu32_*nu13_
        )/(E1_*E2_*E3_);

    const dimensionedScalar A11 = (1.0 - nu23_*nu32_)/(J*E2_*E3_);
    const dimensionedScalar A22 = (1.0 - nu13_*nu31_)/(J*E1_*E3_);
    const dimensionedScalar A33 = (1.0 - nu21_*nu12_)/(J*E2_*E1_);
    const dimensionedScalar A12 = (nu12_ + nu32_*nu13_)/(J*E1_*E3_);
    const dimensionedScalar A31 = (nu31_ + nu21_*nu32_)/(J*E2_*E3_);
    const dimensionedScalar A23 = (nu23_ + nu21_*nu13_)/(J*E1_*E2_);
    const dimensionedScalar A44 =  2.0*G12_;
    const dimensionedScalar A55 =  2.0*G23_;
    const dimensionedScalar A66 =  2.0*G31_;

    // Set elasticC in local coordinate system and then we will rotate it to the
    // global coordinate system

    elasticCfPtr_ =
        new surfaceSymmTensor4thOrderField
        (
            IOobject
            (
                "elasticCf",
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh(),
            dimensionedSymmTensor4thOrder
            (
                "localElasticC",
                dimPressure,
                symmTensor4thOrder
                (
                    A11.value(), A12.value(), A31.value(),
                    A22.value(), A23.value(),
                    A33.value(),
                    A44.value(),
                    A55.value(),
                    A66.value()
                )
            )
        );

    surfaceSymmTensor4thOrderField& C = *elasticCfPtr_;

    // Calculate rotating matrix from local directions to global directions
    const volTensorField matDir =
        matDirX_*matDirX_ + matDirY_*matDirY_ + matDirZ_*matDirZ_;

    // Rotate C from local directions to global directions
    C = transform(fvc::interpolate(matDir), C);
}


const Foam::surfaceSymmTensor4thOrderField&
Foam::orthotropicLinearElastic::elasticCf() const
{
    if (!elasticCfPtr_)
    {
        makeElasticCf();
    }

    return *elasticCfPtr_;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// Construct from dictionary
Foam::orthotropicLinearElastic::orthotropicLinearElastic
(
    const word& name,
    const fvMesh& mesh,
    const dictionary& dict,
    const nonLinearGeometry::nonLinearType& nonLinGeom
)
:
    mechanicalLaw(name, mesh, dict, nonLinGeom),
    rho_(dict.lookup("rho")),
    E1_(dict.lookup("E1")),
    E2_(dict.lookup("E2")),
    E3_(dict.lookup("E3")),
    nu12_(dict.lookup("nu12")),
    nu23_(dict.lookup("nu23")),
    nu31_(dict.lookup("nu31")),
    nu21_(nu12_*E2_/E1_),
    nu32_(nu23_*E3_/E2_),
    nu13_(nu31_*E1_/E3_),
    G12_(dict.lookup("G12")),
    G23_(dict.lookup("G23")),
    G31_(dict.lookup("G31")),
    elasticCPtr_(NULL),
    elasticCfPtr_(NULL),
    matDirX_
    (
        IOobject
        (
            "materialDirectionsX",
             mesh.time().timeName(),
             mesh,
             IOobject::READ_IF_PRESENT,
             IOobject::NO_WRITE
        ),
        mesh,
        dimensionedVector
        (
            dict.lookupOrDefault<vector>("materialDirectionX", vector(1,0,0))
        )
    ),
    matDirY_
    (
        IOobject
        (
            "materialDirectionsY",
             mesh.time().timeName(),
             mesh,
             IOobject::READ_IF_PRESENT,
             IOobject::NO_WRITE
        ),
        mesh,
        dimensionedVector
        (
            dict.lookupOrDefault<vector>("materialDirectionY", vector(0,1,0))
        )
    ),
    matDirZ_
    (
        IOobject
        (
            "materialDirectionsZ",
             mesh.time().timeName(),
             mesh,
             IOobject::READ_IF_PRESENT,
             IOobject::NO_WRITE
        ),
        mesh,
        dimensionedVector
        (
            dict.lookupOrDefault<vector>("materialDirectionZ", vector(0,0,1))
        )
    )
{
    if (planeStress())
    {
        FatalErrorIn
        (
            "Foam::orthotropicLinearElastic::orthotropicLinearElastic\n"
            "(\n"
            "    const word& name,\n"
            "    const fvMesh& mesh,\n"
            "    const dictionary& dict\n"
            ")"
        )   << "Material law not implemented for planeStress!"
            << abort(FatalError);
    }

    // Check material properties are physical
    Info<< "Checking physical constraints on the orthotropic material "
        << "properties" << endl;

    // E and G should be greater than zero
    if
    (
        E1_.value() < 0.0 || E2_.value() < 0.0 || E3_.value() < 0.0
     || G12_.value() < 0.0 || G23_.value() < 0.0 || G31_.value() < 0.0
    )
    {
        FatalErrorIn
        (
            "Foam::orthotropicLinearElastic::orthotropicLinearElastic\n"
            "(\n"
            "    const word& name,\n"
            "    const fvMesh& mesh,\n"
            "    const dictionary& dict\n"
            ")"
        )   << "E1, E2, E3, G12, G23, G31 should all be greater than zero!"
            << abort(FatalError);
    }

    // Restriction on Poisson's ratio
    if
    (
        mag(nu12_.value()) >= sqrt(E1_.value()/E2_.value())
     || mag(nu23_.value()) >= sqrt(E2_.value()/E3_.value())
     || mag(nu31_.value()) >= sqrt(E3_.value()/E1_.value())
    )
    {
        FatalErrorIn
        (
            "Foam::orthotropicLinearElastic::orthotropicLinearElastic\n"
            "(\n"
            "    const word& name,\n"
            "    const fvMesh& mesh,\n"
            "    const dictionary& dict\n"
            ")"
        )   << "Unphysical Poisson's ratio!"
            << " mag(nu_ij) should be less sqrt(E_i/E_j)"
            << abort(FatalError);
    }

    if
    (
        1.0 - nu12_.value()*nu21_.value() - nu23_.value()*nu32_.value()
      - nu31_.value()*nu13_.value()
      - 2.0*nu21_.value()*nu32_.value()*nu13_.value()
     <= 0.0
    )
    {
        FatalErrorIn
        (
            "Foam::orthotropicLinearElastic::orthotropicLinearElastic\n"
            "(\n"
            "    const word& name,\n"
            "    const fvMesh& mesh,\n"
            "    const dictionary& dict\n"
            ")"
        )   << "Unphysical Poisson's ratio!"
            << " (1 - nu12*nu21 - nu23*nu32 "
            << "- nu31*nu13 - 2*nu21*nu32*nu13) should be greater than zero!"
            << abort(FatalError);
    }


    // Check the direction vectors
    if
    (
        min(mag(matDirX_)).value() < SMALL
     || min(mag(matDirY_)).value() < SMALL
     || min(mag(matDirZ_)).value() < SMALL
    )
    {
        FatalErrorIn
        (
            "Foam::orthotropicLinearElastic::orthotropicLinearElastic\n"
            "(\n"
            "    const word& name,\n"
            "    const fvMesh& mesh,\n"
            "    const dictionary& dict\n"
            ")"
        )   << "The direction vectors must not have zero length!"
            << abort(FatalError);
    }

    // Normalise the direction vectors
    matDirX_ /= mag(matDirX_);
    matDirY_ /= mag(matDirY_);
    matDirZ_ /= mag(matDirZ_);

    // Check the material direcction vectors are locally orthogonal
    if
    (
        max(matDirX_ & matDirY_).value() > SMALL
     || max(matDirX_ & matDirZ_).value() > SMALL
     || max(matDirY_ & matDirZ_).value() > SMALL
    )
    {
        FatalErrorIn
        (
            "Foam::orthotropicLinearElastic::orthotropicLinearElastic\n"
            "(\n"
            "    const word& name,\n"
            "    const fvMesh& mesh,\n"
            "    const dictionary& dict\n"
            ")"
        )   << "The direction vectors should be locally orthogonal!"
            << abort(FatalError);
    }
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::orthotropicLinearElastic::~orthotropicLinearElastic()
{
    deleteDemandDrivenData(elasticCPtr_);
    deleteDemandDrivenData(elasticCfPtr_);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::tmp<Foam::volScalarField> Foam::orthotropicLinearElastic::rho() const
{
    tmp<volScalarField> tresult
    (
        new volScalarField
        (
            IOobject
            (
                "rho",
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh(),
            rho_,
            zeroGradientFvPatchScalarField::typeName
        )
    );

    tresult().correctBoundaryConditions();

    return tresult;
}


Foam::tmp<Foam::volScalarField> Foam::orthotropicLinearElastic::impK() const
{
    // We could employ a diagTensor as the implicit stiffness; however, the
    // average of this diagTensor should achieve similar convergence
    return tmp<volScalarField>
    (
        new volScalarField
        (
            IOobject
            (
                "impK",
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            (
                elasticC().component(symmTensor4thOrder::XXXX)
              + elasticC().component(symmTensor4thOrder::YYYY)
              + elasticC().component(symmTensor4thOrder::ZZZZ)
            )/3.0
        )
    );
}


void Foam::orthotropicLinearElastic::correct(volSymmTensorField& sigma)
{
    if (incremental())
    {
        // Lookup gradient of displacement increment
        const volTensorField& gradDD =
            mesh().lookupObject<volTensorField>("grad(DD)");

        // Calculate stress
        sigma = sigma.oldTime() + (elasticC() && symm(gradDD));
    }
    else
    {
        // Lookup gradient of displacement
        const volTensorField& gradD =
            mesh().lookupObject<volTensorField>("grad(D)");

        // Calculate stress
        sigma = elasticC() && symm(gradD);
    }
}


void Foam::orthotropicLinearElastic::correct(surfaceSymmTensorField& sigma)
{
    if (incremental())
    {
        // Lookup gradient of displacement increment
        const surfaceTensorField& gradDD =
            mesh().lookupObject<surfaceTensorField>("grad(DD)f");

        // Calculate stress based on incremental form of Hooke's law
        sigma = sigma.oldTime() + (elasticCf() && symm(gradDD));
    }
    else
    {
        // Lookup gradient of displacement
        const surfaceTensorField& gradD =
            mesh().lookupObject<surfaceTensorField>("grad(D)f");

        // Calculate stress based on Hooke's law
        sigma = elasticCf() && symm(gradD);
    }
}


// ************************************************************************* //
