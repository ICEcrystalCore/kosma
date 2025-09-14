make_vcpkg_target(KsGTest)

add_vcpkg_pack_for(KsGTest GTest)

add_vcpkg_libraries_for(KsGTest GTest::GTest GTest::Main)