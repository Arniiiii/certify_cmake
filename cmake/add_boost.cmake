include(${CMAKE_CURRENT_LIST_DIR}/getCPM.cmake)

# cpmaddpackage( NAME AddBoost.CMake VERSION 3.3 URL
# https://github.com/Arniiiii/AddBoost.cmake/archive/refs/tags/3.1.tar.gz)
cpmaddpackage( NAME AddBoost.CMake VERSION 3.3 URL
  GITHUB_REPOSITORY Arniiiii/AddBoost.cmake GIT_TAG testing)
