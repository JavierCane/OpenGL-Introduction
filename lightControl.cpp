// Control de luces

#ifndef _LIGHT_CONTROL
#define _LIGHT_CONTROL

class lightControl
{
public:
    static const int NUM_POSITIONS = 5;

    enum dynamicPosition{
        CENTER,         // 0 =>  0, 0,  0
        LEFT_BACK,      // 1 => -x, y, -z
        RIGHT_BACK,     // 2 =>  x, y, -z
        RIGHT_FRONT,    // 3 =>  x, y,  z
        LEFT_FRONT      // 4 => -x, y,  z
    };

    /*
    // Overload the LightPosition++ operator
    inline dynamicPosition& operator++( dynamicPosition& type, int )  // <--- note -- must be a reference
    {

        const int i = static_cast<int> ( type ) + 1;

        type = static_cast< dynamicPosition >( (i) % NUM_POSITIONS ) ;

        return type;
    }

    // Overload the LightPosition-- operator
    lightControl::dynamicPosition& operator--( dynamicPosition& type )  // <--- note -- must be a reference
    {
        const int i = static_cast<int>(type)-1;

        if ( i < 0 ) // Check whether to cycle to last item if number goes below 0
        {
            type = static_cast<dynamicPosition>( num_positions - 1 );
        }
        else // Else set it to current number -1
        {
            type = static_cast<dynamicPosition>( (i) % num_positions );
        }

        return type;
    }
    */

    /*
    // Overload the LightPosition++ operator
    inline dynamicPosition& operator++( dynamicPosition& type, int )  // <--- note -- must be a reference
    {
        const int i = static_cast<int> ( type ) + 1;

        type = static_cast<dynamicPosition>( (i) % num_positions );

        return type;
    }

    // Overload the LightPosition-- operator
    inline dynamicPosition& operator--( dynamicPosition& type, int )  // <--- note -- must be a reference
    {
        const int i = static_cast<int>(type)-1;

        if ( i < 0 ) // Check whether to cycle to last item if number goes below 0
        {
            type = static_cast<dynamicPosition>( num_positions - 1 );
        }
        else // Else set it to current number -1
        {
            type = static_cast<dynamicPosition>( (i) % num_positions );
        }

        return type;
    }
    */

    /**
      * Devuelve el siguiente valor del enum
      * Se asume que van a tener valores consecutivos
      */
    static dynamicPosition getNext( dynamicPosition dynamic_position )
    {
        const int i = static_cast<int> ( dynamic_position ) + 1;

        dynamic_position = static_cast<dynamicPosition>( (i) % NUM_POSITIONS );

        return dynamic_position;

    }

    /**
      * Devuelve si la coordenada x debe expresarse negativamente o no.
      * Devuelve un 1 con el signo pertinente para que se pueda multiplicar el resultado de esta operación por la coordenada x directamente
      */
    static float getXSign( dynamicPosition dynamic_position )
    {
        if( dynamic_position == LEFT_BACK || dynamic_position == LEFT_BACK )
        {
            return -1.0;
        }
        else
        {
            return 1.0;
        }
    }

    /**
      * Devuelve si la coordenada z debe expresarse negativamente o no.
      * Devuelve un 1 con el signo pertinente para que se pueda multiplicar el resultado de esta operación por la coordenada z directamente
      */
    static float getZSign( dynamicPosition dynamic_position )
    {
        if( dynamic_position == LEFT_BACK || dynamic_position == RIGHT_BACK )
        {
            return -1.0;
        }
        else
        {
            return 1.0;
        }
    }
};

#endif
