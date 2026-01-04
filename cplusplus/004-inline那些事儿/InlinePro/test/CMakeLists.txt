# Parent project does not export its library target, so this CML implicitly
# depends on being added from it, i.e. the testing is done only from the build
# tree and is not feasible from an install location

project(InlineProTests LANGUAGES CXX)

# ---- Tests ----

add_executable(InlinePro_test source/InlinePro_test.cpp)
target_link_libraries(InlinePro_test PRIVATE InlinePro_lib)
target_compile_features(InlinePro_test PRIVATE cxx_std_11)

add_test(NAME InlinePro_test COMMAND InlinePro_test)

# ---- End-of-file commands ----

add_folders(Test)
