# cmake/CplusplusStandarAndLTO.cmake
# —————————————————————————————————————————————————————————————
# Configuración de standar and LTO opimization
# —————————————————————————————————————————————————————————————

# C++ standard y optimizaciones
#[[
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Activate LTO optimization
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
]]