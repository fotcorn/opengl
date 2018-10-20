from conans import ConanFile, tools, Meson
import os

class ConanFileToolsTest(ConanFile):
    generators = 'pkg_config'
    requires = (
        'glew/2.1.0@bincrafters/stable',
        'glfw/3.2.1@bincrafters/stable',
        'glm/0.9.9.1@g-truc/stable',
    )
    settings = 'os', 'compiler', 'build_type'

    def build(self):
        meson = Meson(self)
        meson.configure()
