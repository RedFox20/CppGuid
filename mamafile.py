import mama
class CppGuid(mama.BuildTarget):
    local_workspace = 'build'

    def dependencies(self):
        pass

    def configure(self):
        if self.is_test_target():
            print("Enabled CppGuidTests for testing")
            self.add_cmake_options('CPPGUID_BUILD_TESTS=ON')

    def test(self, args):
        self.gdb('lib/CppGuidTests', src_dir=True)