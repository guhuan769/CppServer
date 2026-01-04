install(
    TARGETS InlinePro_exe
    RUNTIME COMPONENT InlinePro_Runtime
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
