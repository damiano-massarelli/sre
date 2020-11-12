#pragma once
#include <type_traits>

/**
 * Describes the rendering domain in which a Material can be used.
 * Some objects need to be rendered in multiple domains. For examples, objects casting
 * shadows need to be rendered in the "main" domain as well as in the shadow mapping
 * domain. On the other hand, some objects do not support specific domains (e.g. props using a 
 * PropMaterial will only be rendered in the forward rendering domain).
 * Materials can specify the domains that they do not support using Material::unSupportedRenderDomains.
 */
enum class RenderDomain {
    NONE = 0,
    FORWARD_RENDERING = 1 << 0,
    SHADOW_MAPPING = 1 << 1,
    PBR = 1 << 2,
    ALL = ~0
};

/**
 * Describes which a particular render phase.
 * Can be used to specify which parts of the rendering pipeline should be carried out.
 */
enum class RenderPhase {
    NONE = 0,
    FORWARD_RENDERING = 1 << 0,
    SHADOW_MAPPING = 1 << 1,
    PBR = 1 << 2,
    PBR_RESOLVE_LIGHTS = 1 << 3,
    PARTICLES = 1 << 4,
    ALL = ~0
};

template <typename T>
using EnableBitwiseOperator = std::enable_if_t<std::is_same_v<T, RenderDomain>
    || std::is_same_v<T, RenderPhase>>;

template <typename Enum, typename = EnableBitwiseOperator<Enum>>
Enum operator|(Enum lhs, Enum rhs) {
    using underlying = std::underlying_type_t<Enum>;
    return static_cast<Enum>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

template <typename Enum, typename = EnableBitwiseOperator<Enum>>
Enum operator|=(Enum& lhs, Enum rhs) {
    lhs = lhs | rhs;
    return lhs;
}

template <typename Enum, typename = EnableBitwiseOperator<Enum>>
Enum operator&(Enum lhs, Enum rhs) {
    using underlying = std::underlying_type_t<Enum>;
    return static_cast<Enum>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

template <typename Enum, typename = EnableBitwiseOperator<Enum>>
Enum operator&=(Enum& lhs, Enum rhs) {
    lhs = lhs & rhs;
    return rhs;
}

template <typename Enum, typename = EnableBitwiseOperator<Enum>>
Enum operator~(Enum val) {
    return static_cast<Enum>(~static_cast<std::underlying_type_t<Enum>>(val));
}
