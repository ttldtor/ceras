#include "../include/ceras.hpp"
#include <iostream>

int main()
{
    using namespace ceras;

    auto a = ceras::variable{ ceras::random<float>( {5, 5}, -1.0f, 1.0f ) };
    std::cout << "Testing exponential with a=\n" << a.data() << std::endl;

    auto la = ceras::exponential(a);
    auto& s = get_default_session<tensor<float>>();
    auto const& result = s.run( la );
    std::cout << "Resutl is " << result << std::endl;

    la.backward( ceras::ones_like( result ) );
    std::cout << "gradient with a=\n" << a.gradient() << std::endl;

    return 0;
}

