find_package(chimera QUIET)
aikido_check_package(chimera "Python bindings" "Chimera")

find_package(PythonInterp QUIET)
aikido_check_package(PYTHONINTERP "Python bindings" "Python interpreter")

find_package(PythonLibs QUIET)
aikido_check_package(PYTHONLIBS "Python bindings" "Python libraries")

aikido_check_package(Boost_PYTHON "Python bindings" "Boost.Python")
 
execute_process(COMMAND "${PYTHON_EXECUTABLE}" -c
  "from distutils.sysconfig import get_python_lib;\
  print(get_python_lib(plat_specific=True, prefix=''))"
  OUTPUT_VARIABLE PYTHON_SITE_PACKAGES
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

include_directories(SYSTEM
  ${Boost_INCLUDE_DIRS}
  ${DART_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
)
include_directories(
  "${PROJECT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

get_property(AIKIDO_LIBRARIES GLOBAL PROPERTY AIKIDO_LIBRARIES)

add_chimera_binding(TARGET "${PROJECT_NAME}_python"
  MODULE "${PROJECT_NAME}"
  CONFIGURATION "${CMAKE_CURRENT_SOURCE_DIR}/chimera.yml"
  NAMESPACES "aikido"
  SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/chimera.cpp"
  DEBUG
)
target_link_libraries("${PROJECT_NAME}_python"
  ${AIKIDO_LIBRARIES}
  ${Boost_PYTHON_LIBRARIES}
  ${DART_LIBRARIES}
  ${PYTHON_LIBRARIES}
)
set_target_properties("${PROJECT_NAME}_python" PROPERTIES
  OUTPUT_NAME "${PROJECT_NAME}"
  PREFIX ""
)

install(TARGETS "${PROJECT_NAME}_python"
  LIBRARY DESTINATION "${PYTHON_SITE_PACKAGES}"
)
