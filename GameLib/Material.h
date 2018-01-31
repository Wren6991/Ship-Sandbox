/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "Vectors.h"

#include <memory>
#include <optional>
#include <picojson/picojson.h>
#include <string>

struct Material
{
	//
	// Common Properties
	//

    std::string const Name;
    float const Strength;
    float const Mass;
    vec3f const Colour;
    bool const IsHull;

	//
	// Electrical properties - optional
	//

	struct _ElectricalProperties
	{
		enum class ElectricalElementType
		{
			Lamp,
			Cable,
			Generator
		};

		ElectricalElementType ElementType;
		float const Resistance;
		float const GeneratedVoltage;

		_ElectricalProperties(
			ElectricalElementType elementType,
			float resistance,
			float generatedVoltage)
			: ElementType(elementType)
			, Resistance(resistance)
			, GeneratedVoltage(generatedVoltage)
		{
		}
	};

	std::optional<_ElectricalProperties> ElectricalProperties;

public:

	static std::unique_ptr<Material> Create(picojson::object const & materialJson);

private:

	Material(
		std::string name,
		float strength,
		float mass,
		vec3f colour,
		bool isHull,
		std::optional<_ElectricalProperties> electricalProperties)
		: Name(name)
		, Strength(strength)
		, Mass(mass)
		, Colour(colour)
		, IsHull(isHull)
		, ElectricalProperties(electricalProperties)
	{
	}
};
