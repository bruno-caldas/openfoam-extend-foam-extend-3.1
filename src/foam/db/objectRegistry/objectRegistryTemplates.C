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

#include "objectRegistry.H"
#include "Time.H"

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
Foam::wordList
Foam::objectRegistry::names() const
{
    wordList objectNames(size());

    label count=0;
    for (const_iterator iter = begin(); iter != end(); ++iter)
    {
        if (isA<Type>(*iter()))
        {
            objectNames[count++] = iter()->name();
        }
    }

    objectNames.setSize(count);

    return objectNames;
}


template<class Type>
Foam::HashTable<const Type*>
Foam::objectRegistry::lookupClass() const
{
    HashTable<const Type*> objectsOfClass(size());

    for (const_iterator iter = begin(); iter != end(); ++iter)
    {
        if (isA<Type>(*iter()))
        {
            objectsOfClass.insert
            (
                iter()->name(),
                dynamic_cast<const Type*>(iter())
            );
        }
    }

    return objectsOfClass;
}


template<class Type>
bool Foam::objectRegistry::foundObject(const word& name) const
{
    const_iterator iter = find(name);

    if (iter != end())
    {
        const Type* vpsiPtr_ = dynamic_cast<const Type*>(iter());

        if (vpsiPtr_)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if (&parent_ != dynamic_cast<const objectRegistry*>(&time_))
        {
            return parent_.foundObject<Type>(name);
        }
        else
        {
            return false;
        }
    }
}


template<class Type>
const Type& Foam::objectRegistry::lookupObject(const word& name) const
{
    const_iterator iter = find(name);

    if (iter != end())
    {
        const Type* vpsiPtr_ = dynamic_cast<const Type*>(iter());

        if (vpsiPtr_)
        {
            return *vpsiPtr_;
        }

        FatalErrorIn("objectRegistry::lookupObject<Type>(const word&) const")
            << nl
            << "    lookup of " << name << " from objectRegistry "
            << this->name()
            << " successful\n    but it is not a " << Type::typeName
            << ", it is a " << iter()->type()
            << abort(FatalError);
    }
    else
    {
        if (&parent_ != dynamic_cast<const objectRegistry*>(&time_))
        {
            return parent_.lookupObject<Type>(name);
        }
        else
        {
            FatalErrorIn
            (
                "objectRegistry::lookupObject<Type>(const word&) const"
            )   << nl
                << "    request for " << Type::typeName
                << " " << name << " from objectRegistry " << this->name()
                << " failed\n    available objects of type " << Type::typeName
                << " are" << nl
                << names<Type>()
                << abort(FatalError);
        }
    }

    return *reinterpret_cast< const Type* >(0);
}


// ************************************************************************* //
