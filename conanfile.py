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
    "Qt/5.15.2",
    "OpenSubdiv/3.4.3@thirdparty/development",
    "USD/21.05"
  ]
