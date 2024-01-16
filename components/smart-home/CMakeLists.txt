idf_component_register(SRCS "smart-home.cpp" "smart-home-wrapper.cpp"
                    PRIV_REQUIRES esp_matter
                    INCLUDE_DIRS "include")

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 17)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
