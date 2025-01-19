#include <boost/certify/https_verification.hpp>

#include <boost/certify/verification_utils.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/filesystem/directory.hpp>
#include <iostream>

int
main(int argc, char** argv, char** envp)
{
    if (argc != 2)
    {
        std::cout << "Amount of console arguments is not equal to 2. The "
                     "current amount is "
                  << argc << " .\n";
        return 1;
    }

    ::SSL_library_init();

    boost::certify::certificate_store store;
    store.set_default_paths();

    int count = 0;
    for (auto const& entry : boost::filesystem::directory_iterator{
           std::string(argv[1]) + "/success_chains/"})
    {
        boost::system::error_code ec;
        boost::certify::verify_chain(entry.path(), store, ec);
        if (ec)
            BOOST_ERROR((entry.path().string() + ": " + ec.message()).c_str());
        ++count;
    }
    BOOST_TEST(count > 0);

    return boost::report_errors();
}
