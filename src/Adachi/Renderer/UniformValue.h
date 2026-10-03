#pragma once

// Uniform value that to be used for shader(material)'s uniform & global uniform buffer
#include <glad/gl.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <variant>
#include <vector>
#include <sstream>
#include <iomanip>

#include "Adachi/Diagnostics/Log.h"



namespace Adachi {

using UniformValue = std::variant<
    int,
    std::vector<int>,
    float,
    glm::vec2,
    glm::vec3,
    glm::vec4,
    glm::mat3,
    glm::mat4
    //Ref<Texture2D>
>;

template<
    glm::length_t Columns,
    glm::length_t Rows,
    typename T,
    glm::qualifier Q>
static std::string FormatMatrix(
    const glm::mat<Columns, Rows, T, Q>& matrix)
{
    std::ostringstream stream;

    stream << std::fixed << std::setprecision(4);
    stream << "[\n";

    for (glm::length_t row = 0; row < Rows; ++row)
    {
        stream << "  [ ";

        for (glm::length_t column = 0; column < Columns; ++column)
        {
            // GLM matrices use matrix[column][row].
            stream << std::setw(10)
                   << matrix[column][row];

            if (column + 1 < Columns)
                stream << ", ";
        }

        stream << " ]";

        if (row + 1 < Rows)
            stream << ',';

        stream << '\n';
    }

    stream << ']';
    return stream.str();
}

static void PrintUniform(
    std::string_view label,
    std::string_view name,
    const UniformValue& uniformValue)
{
    const std::string formattedValue = std::visit(
        []<typename T>(const T& value) -> std::string
        {
            using ValueType = std::remove_cvref_t<T>;

            if constexpr (std::is_same_v<ValueType, int>)
            {
                return std::to_string(value);
            }
            else if constexpr (std::is_same_v<ValueType, float>)
            {
                std::ostringstream stream;
                stream << value;
                return stream.str();
            }
            else if constexpr (
                std::is_same_v<ValueType, std::vector<int>>)
            {
                std::ostringstream stream;
                stream << '[';

                for (std::size_t i = 0; i < value.size(); ++i)
                {
                    if (i != 0)
                        stream << ", ";

                    stream << value[i];
                }

                stream << ']';
                return stream.str();
            }
            else if constexpr (
                std::is_same_v<ValueType, glm::mat3> ||
                std::is_same_v<ValueType, glm::mat4>)
            {
                return FormatMatrix(value);
            }
            else
            {
                // glm::vec2, glm::vec3 and glm::vec4
                return glm::to_string(value);
            }
        },
        uniformValue);

    ADACHI_CORE_INFO(
        "{} Uniform '{}' [variant index={}]:\n{}",
        label,
        name,
        uniformValue.index(),
        formattedValue);
}

template<typename T>
concept UniformValueType =
    std::same_as<std::remove_cvref_t<T>, int> ||
    std::same_as<std::remove_cvref_t<T>, std::vector<int>> ||
    std::same_as<std::remove_cvref_t<T>, float> ||
    std::same_as<std::remove_cvref_t<T>, glm::vec2> ||
    std::same_as<std::remove_cvref_t<T>, glm::vec3> ||
    std::same_as<std::remove_cvref_t<T>, glm::vec4> ||
    std::same_as<std::remove_cvref_t<T>, glm::mat3> ||
    std::same_as<std::remove_cvref_t<T>, glm::mat4>;

}