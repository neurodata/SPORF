"""
py-RerF
"""


__version__ = "2.0.1"


def check_version():
    """
    Tells you if you have an old version of RerF.
    """
    import requests

    r = requests.get("https://pypi.python.org/pypi/rerf/json").json()
    r = r["info"]["version"]
    if r != __version__:
        print(
            "A newer version of rerf is available. "
            + "'pip install -U rerf' to update."
        )
    return r
