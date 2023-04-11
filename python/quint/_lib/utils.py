import os
import platform
import re
import sys

from colorama import Fore, Style

if sys.version_info[0] < 3 or sys.version_info[1] <= 5:
    raise RuntimeError(
        "\nPlease restart with Python 3.6+\n" + "Current Python version:",
        sys.version_info
    )


def in_docker():
    if os.environ.get("QUINT_IN_DOCKER", "") == "":
        return False
    return True


def get_os_name():
    name = platform.platform()
    # in python 3.8, platform.platform() uses mac_ver() on macOS
    # it will return 'macOS-XXXX' instead of 'Darwin-XXXX'
    if name.lower().startswith('darwin') or name.lower().startswith('macos'):
        return 'osx'
    if name.lower().startswith('windows'):
        return 'win'
    if name.lower().startswith('linux'):
        return "linux"
    if 'bsd' in name.lower():
        return 'unix'
    assert False, f"Unknown platform name {name}"


def import_ti_python_core():
    if get_os_name() != 'win':
        # pylint: disable=E1101
        old_flags = sys.getdlopenflags()
        sys.setdlopenflags(2 | 8)
    else:
        pyddir = os.path.dirname(os.path.relpath(__file__))
        os.environ['PATH'] += os.pathsep + pyddir
    try:
        from quint._lib.core import quint_python as core
    except Exception as e:
        if isinstance(e, ImportError):
            print(Fore.YELLOW + "Share object quint_python import failed, "
                  "check this page for possible solutions:\n"
                  "https://quintlang.org/docs/install" + Fore.RESET)
            if get_os_name() == "win":
                e.msg = "\nConsider installing Microsoft Visual C++ Redistributable: " \
                        "https://aka.ms/vs/16/release/vc_redist.x64.exe "
        raise e from None

    if get_os_name() != 'win':
        sys.setdlopenflags(old_flags)
    lib_dir = os.path.join(package_root, '_lib', 'runtime')
    core.set_lib_dir(local_encode(lib_dir))
    return core


def local_encode(path):
    try:
        import locale
        return path.encode(locale.getdefaultlocale()[1])
    except (UnicodeEncodeError, TypeError):
        try:
            return path.encode(sys.getgilesystemencoding())
        except UnicodeEncodeError:
            try:
                return path.encode()
            except UnicodeEncodeError:
                return path


package_root = os.path.join(
    os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
)

qu_python_core = import_ti_python_core()

qu_python_core.set_python_package_dir(package_root)
