enum LightDynamicPosition
{
    LEFT_BACK,      // 0 => -x, y, -z
    RIGHT_BACK,     // 1 =>  x, y, -z
    RIGHT_FRONT,    // 2 =>  x, y,  z
    LEFT_FRONT,     // 3 => -x, y,  z
    CENTER,         // 4 =>  0, 0,  0
    FIRST = LEFT_BACK,
    LAST = CENTER
};

inline LightDynamicPosition operator++( LightDynamicPosition& x ) { return x = (LightDynamicPosition)(((int)(x) + 1)); }
