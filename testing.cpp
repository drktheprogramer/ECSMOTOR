#include <iostream>
#include <cstdint>
#include <type_traits>

struct c_health {};
struct c_shiedl {};
struct c_hitbox {};
struct c_render {};
struct c_move_i {};
struct c_move {};

void seetype(auto) { std::cout << __PRETTY_FUNCTION__ << std::endl; }


namespace GE {
    namespace Test {
        // CONSTANT, TRUE / FALSE TYPE
        //
        template<typename T,T VAL> //typename T: T es el nombre del tipo (con lo que nos vamos a referenciar), T VAL es una variable del tipo T
        // (si T es int VAL solo pueden ser numeros enteros, si T es bool VAL tiene el dominio en booleanos)
        struct CONSTANT { //las struct en metaprogramming son formas de hacer metafunciones, en este caso CONSTANT es una metafuncion que retorna un valor
            static constexpr T value () { return VAL; } //la funcion value es del tipo T y es estatica para no tener la necesidad de instanciar un objeto
            // CONSTANT, constexpr es un prefijo utilizado para que la expresion que le sigue sea evaluada en tiempo de compilacion
            // la funcion de tipo T (value) retorna la variable VAL que tambien es de tipo T, por la definicion de la plantilla
        };

        struct t_false : CONSTANT<bool, false> {}; //esta estructura t_false y/o t_true son "metainstancias" de CONSTANT, en este caso
        //estas "metainstancias" son "metaconstantes". La estructura t_false hereda de una instancia de CONSTANT con los argumenta de plantilla 
        //definidos como sigue: T = bool, VAL = false
        struct t_true : CONSTANT<bool, true> {};

        // IS_SAME
        //
        template<typename T, typename U>
        struct is_same : t_false {}; //la estructura is_same toma dos argumentos de plantilla 
        //que por defecto si T y U son distintos, da falso (o sea que hereda la estructura t_false)
        //notese que utiliza la declaracion de la metafuncion con un valor por defecto false

        //HELPER
        template<typename T, typename U>
        constexpr bool is_same_v = is_same<T, U>::value();//esta es una abreviatura para la metafuncion
        //is_same_v abrevia la llamada a la funcion value() dentro de la estructura is_same
        //que fue heredada de t_true o t_false segun el caso (que a su vez ambas heredan de CONSTANT)

        template<typename T>
        struct is_same<T, T> : t_true {}; //esta estructura is_same es una especializacion de la 
        //metafuncion is_same declarada mas arriba (esto es necesario para hacer una especializacion)
        //que devuelve el valor true solo si ambos argumentos pasados son iguales (o sea que hereda
        //de la estructura t_true)
        //notese que en la especializacion se necesita pasar los argumentos que queremos dejar definidos
        //ademas en la plantilla se debe colocar el nombre del tipo que no es constante en su definicion
        //(o sea que puede cambiar)

        // TYPE_ID
        //
        template<typename T>//metafuncion que devuelve tipos
        struct type_id {
            using type = T;//type es un alias para el tipo T pasado como argumento de plantilla a la estructura type_id
            //sin embargo es utilizado tambien para retornar el tipo del argumento con el cual se llama a la funcion type_id
            //un ejemplo podria ser static_assert(is_same_v<type_id<int>::type, int>); el cual daria true ya que
            //instanciamos a type_id con el argumento de plantilla T = int, y luego usamos el alias type (con ::type) para que nos
            //retorne el tipo con el que se instancio type_id, o sea int
        };

        // NTH_TYPE
        //
        //funcion con recursividad
        //devuelve el n-esimo tipo de una lista de tipos Ts
        template<std::size_t N, typename ...Ts>
        struct nth_type {
            static_assert(sizeof ...(Ts) != 0, "ERROR: TypeList with 0 arguments"); //excepcion por si nth_type se inicia sin tipos
        };
        //HELPER
        template<std::size_t N, typename ...Ts>
        using nth_type_t = nth_type<N, Ts...>::type;
        template<typename T ,typename ...Ts>
        struct nth_type<0, T, Ts...> : type_id<T> {};//condicion de fuga
        //cuando N = 0 entonce se ejecuta esta especializacion la cual usa el alias type = T, el cual seria el n-esimo elemento de la lista de tipos
        //Ts, o sea hereda de la estructura type_id instanciada con el tipo T, lo cuao devuelve este tipo T, analice la definicion de type_id
        template<std::size_t N, typename T, typename ...Ts>
        struct nth_type<N, T, Ts...> : type_id<typename nth_type<N-1, Ts...>::type> {// using type = nth_type<N-1, Ts...>::type
        };
        //recursividad, sabemos que con la especializacion de nth_type con los argumentos de plantilla <std::size_t N, typename T, typename ...Ts>
        //reduce un elemento de la lista de tipos Ts, por lo que en esta especializacion se utiliza un tipo que provienen de la llamada a 
        //una metainstancia de nth_type pero con argumentos de plantilla <N-1, Ts...>, recuerde que lo que buscamos es el tipo del elemento N de la
        //lista de tipos Ts, por lo que usamos la expresion ::type, (si ve dentro de los {} de la estructura puede ver como se definiria la 
        //estructura si nth_type no heredase de type_id). 
        //cuando instanciamos el type_id para heredarlo en esta especializacion de nth_type se nota que dentro de su argumento de plantilla aparece
        //el keyboard typename, lo que indica al compilador que lo que sigue es un nombre de tipo, se podria evitar usar este typename con el helper
        //nth_type_t con argumentos de plantilla <N-1, Ts...>

        // POS_TYPE
        //
        template<typename T, typename ...Ts>
        struct pos_type { static_assert(sizeof ...(Ts) != 0); };//este static assert evita que la funcion sea cargada 
        //con una lista vacia
        //HELPER
        template<typename T, typename ...Ts>
        constexpr static std::size_t pos_type_v = pos_type<T, Ts...>::value;
        template<typename T, typename ...Ts>
        struct pos_type<T, T, Ts...> : CONSTANT<std::size_t, 0> {};//condicion de fuga, si el elemento primero es igual
        //al elemento que estamos buscando se le asigna la posicion 0
        template<typename T, typename U, typename ...Ts>
        struct pos_type<T, U, Ts...> : CONSTANT<std::size_t, 1 + pos_type_v<T, Ts...>>{};//caso de recursividad, si el elemento
        //primero de la lista no es el que estamos buscando entonces, se quitara un elemento de la lista y se mandara
        //la lista con un elemento menos a la llamada de funcion, la posicion que se le asigna es 1 + la cantidad de veces
        //que se vuelva a llamar a la funcion
        //observe esta forma de quitar un elemento de una lista de tipos, al hacer <T, Ts...> en la especializacion de la funcion
        //provoca que el primer elemento de la lista sea tomado como el nombre de tipo T

        // IF_TYPE
        //
        template<bool COND, typename TRUE, typename FALSE>
        struct IF_TYPE : type_id<FALSE> {};
        //HELPER
        template<bool COND, typename TRUE, typename FALSE>
        using IF_TYPE_t = IF_TYPE<COND, TRUE, FALSE>::type;
        template<typename TRUE, typename FALSE>
        struct IF_TYPE<true, TRUE, FALSE> : type_id<TRUE> {};

        // TYPELIST
        //
        template<typename ...Ts>
        struct TypeList { 
            static consteval std::size_t size() noexcept { return sizeof ... (Ts);}//consteval es una expresion que se evalua solo en tiempo de
            //compilacion, noexcept quiere decir no excepciones, sizeof ... es otra version de sizeof
            //La funcion size() devuelve el tamaño de la cantidad de argumentos dentro de la lista de plantillas de tipo Ts

            template<typename T>
            static consteval bool is_there() noexcept { return (false || ... || is_same_v<T, Ts>); }
            //lo que hay dentro de return es un expresion folder, o una carpeta de expresiones, en este caso sus ex´resiones son false
            // ... y la llamada a is_same, dentro de esta ultima podemos ver que llama al nombre de tipo T (de la metafuncion is_there, o sea su
            //argumento) y Ts (que daria el primer tipo de la lista de tipos Ts), es aqui donde la expresion ... toma sentido, ya que esta
            //"expande" la lista de tipos (Ts), (falta info) el proceso vendria a equivaler al metodo recursivo de especializar is_there
            //obligando a que usemos el siguiente elemento de la lista de tipos Ts
            //La funcion is_there indica si un elemento está dentro de la lista

            template<typename T>
            static consteval std::size_t pos() noexcept {
                static_assert(is_there<T>());
                return pos_type_v<T, Ts...>;
                /*
                for(std::size_t i{}; i < size()-1; ++i)
                {
                    using NTH = nth_type_t<i, Ts...>;
                    if(Test::is_same_v<T, NTH>)
                    {
                        return i;
                    }
                }
                return size()-1;
                */
            }

        };

    }//namespace Test
    
    


    // TYPETRAITS
    //
    template<typename TYPELIST>
    struct TypeTraits { 
        static consteval std::size_t size() noexcept{ return TYPELIST::size(); }//size() hace una llamada a Test::TypeList::size() pero 
        //siendo este TypeList una estructura instanciada (o sea un objeto/estructura lista creado a partir de TypeList) que es dado como
        //argumento a TypeTraits bajo el nombre de tipo TYPELIST

        template<typename TYPE>
        static consteval std::size_t id() noexcept {
            static_assert(TYPELIST::template is_there<TYPE>());
            //static_assert comprueba si el argumento pasado al nombre de tipo TYPE esta
            //dentro de la lista de tipos TYPELIST, dentro podemos ver como llama a la metafuncion is_there de la lista de tipos TYPELIST
            //(recuerde que TYPELIST es una estructura u objeto del tipo TypeList), pero al ser (is_there) una plantilla de funcion miembro
            //de la estructura TypeList, es una plantilla dependiente del tipo, por lo que debemos indicarselo al compilador mediante el 
            //keyboard: template.
            return TYPELIST::template pos_type_v<TYPE>;
        }

        /*template<typename TYPE>
        static consteval std::size_t mask() noexcept {}
        */

    };
}



 

int main()
{
    
    static_assert(GE::Test::is_same<int, int>::value());
    static_assert(GE::Test::is_same_v<int, int>);
    static_assert(GE::Test::CONSTANT<bool, true>::value()); //static_assert es una funcion que solo compila cuando el argumento dado es true
    //esta expresion tiene una estructura CONSTANT con argumentos definifos como sigue: T = bool, T VAL = true, y tiene sentido ya que
    //la variable de tipo bool (VAL) solo tiene dominio en los booleanos
}

//compilar g++ -Wall -Wpedantic -Wconversion -fsanitize=undefined -std=c++20 -c testing.cpp 