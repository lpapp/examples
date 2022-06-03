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
    "Qt/5.15.2"
  ]
