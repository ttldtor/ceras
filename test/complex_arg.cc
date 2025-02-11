#include "../include/ceras.hpp"
#include <iostream>

int main()
{
    using namespace ceras;

    {
        auto a = ceras::variable{ ones<float>( {2, 3} ) };
        auto b = ceras::variable{ ones<float>( {2, 3} ) };
        auto c = ceras::complex{ a, b };
        std::cout << "Testing complex arg with a=\n" << a.data() << std::endl;
        std::cout << "Testing complex arg with b=\n" << b.data() << std::endl;

        auto la = ceras::arg( c );
        auto& s = get_default_session<tensor<float>>();
        auto const& result = s.run( la );
        std::cout << "Resutl is " << result << std::endl;

        la.backward( ceras::ones_like( result ) );
        std::cout << "gradient with a=\n" << a.gradient() << std::endl;
        std::cout << "gradient with b=\n" << b.gradient() << std::endl;
    }

    return 0;
}

