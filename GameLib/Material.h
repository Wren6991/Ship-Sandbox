/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "GameException.h"
#include "Utils.h"
#include "Vectors.h"

#include <picojson/picojson.h>

#include <array>
#include <memory>
#include <optional>
#include <string>

struct Material
{
	//
	// Common Properties
	//

    std::string const Name;
    float const Strength;
    float const Mass;
    vec3f const StructuralColour;
    std::array<uint8_t, 3u> StructuralColourRgb;
    vec3f const RenderColour;    
    bool const IsHull;
    bool const IsRope;

	//
	// Electrical properties - optional
	//

	struct ElectricalProperties
	{
		enum class ElectricalElementType
		{
			Lamp,
			Cable,
			Generator
		};

        static ElectricalElementType StrToElectricalElementType(std::string const & str)
        {
            std::string lstr = Utils::ToLower(str);

            if (lstr == "lamp")
                return ElectricalElementType::Lamp;
            else if (lstr == "cable")
                return ElectricalElementType::Cable;
            else if (lstr == "generator")
                return ElectricalElementType::Generator;
            else
                throw GameException("Unrecognized ElectricalElementType \"" + str + "\"");
        }

		ElectricalElementType ElementType;
		float const Resistance;
		float const GeneratedVoltage;

		ElectricalProperties(
			ElectricalElementType elementType,
			float resistance,
			float generatedVoltage)
			: ElementType(elementType)
			, Resistance(resistance)
			, GeneratedVoltage(generatedVoltage)
		{
		}
	};

	std::optional<ElectricalProperties> Electrical;

    //
    // Sound properties - optional
    //

    struct SoundProperties
    {
        enum class SoundElementType
        {
            Cable,
            Glass,
            Metal,            
            Wood
        };

        static SoundElementType StrToSoundElementType(std::string const & str)
        {
            std::string lstr = Utils::ToLower(str);

            if (lstr == "cable")
                return SoundElementType::Cable;
            else if (lstr == "glass")
                return SoundElementType::Glass;
            else if (lstr == "metal")
                return SoundElementType::Metal;
            else if (lstr == "wood")
                return SoundElementType::Wood;
            else
                throw GameException("Unrecognized SoundElementType \"" + str + "\"");
        }

        SoundElementType ElementType;

        SoundProperties(
            SoundElementType elementType)
            : ElementType(elementType)
        {
        }
    };

    std::optional<SoundProperties> Sound;

public:

	static std::unique_ptr<Material> Create(picojson::object const & materialJson);

    Material(
        std::string name,
        float strength,
        float mass,
        std::array<uint8_t, 3u> structuralColourRgb,
        std::array<uint8_t, 3u> renderColourRgb,
		bool isHull,
        bool isRope,
		std::optional<ElectricalProperties> electricalProperties,
        std::optional<SoundProperties> soundProperties)
		: Name(std::move(name))
		, Strength(strength)
		, Mass(mass)
		, StructuralColour(RgbToVec(structuralColourRgb))
        , StructuralColourRgb(structuralColourRgb)
        , RenderColour(RgbToVec(renderColourRgb))
		, IsHull(isHull)
        , IsRope(isRope)
		, Electrical(std::move(electricalProperties))
        , Sound(std::move(soundProperties))
	{
	}

private:

    static uint8_t Material::Hex2Byte(std::string const & str);
    static std::array<uint8_t, 3u> Hex2RgbColour(std::string str);
    static vec3f RgbToVec(std::array<uint8_t, 3u> const & rgbColour);
};
