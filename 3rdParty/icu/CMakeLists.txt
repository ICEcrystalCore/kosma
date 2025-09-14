make_vcpkg_target(KsICU)

add_vcpkg_pack_for(KsICU ICU uc data)

add_vcpkg_libraries_for(KsICU ICU::uc ICU::data)