import io
from contextlib import redirect_stdout


def get_params(forest):
    params = {}

    f = io.StringIO()
    with redirect_stdout(f):
        forest.printParameters()
    out = f.getvalue()

    for line in out.splitlines():
        A = line.split(" -> ")
        params[A[0]] = A[1]

    return params
