from conans import ConanFile

class TestbedConan(ConanFile):
  name = "Testbed"
  description = "Testbed"
  license = "None"
  url = "None"
  settings = "os", "arch", "compiler", "build_type"
  generators = [
    "cmake_paths",
  ]

  requires = [
    "GLEW/1.13.0@thirdparty/development",
  ]
