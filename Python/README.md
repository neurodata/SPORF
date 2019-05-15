# Randomer Forest (RerF) Python Package

Documentation for RerF Python module can be found at [rerf.neurodata.io](https://rerf.neurodata.io).

## Install

See install [instructions](https://rerf.neurodata.io/install.html).

## Example

[Example.py](example.py) shows basic usage.

## Reference

Function references can be found in our [docs](https://rerf.neurodata.io/reference.html).

## Tests

We use [pytest](https://docs.pytest.org/en/latest/) for Python testing.

Run the tests from command line at the root of the repo (`RerF/`).

```sh
python -m pytest
```

## Publish new version

Increment the version number in `rerf/__init__.py`

create a ~/.pypirc file with the following content:

```ini
[distutils]
index-servers =
  pypi

[pypi]
username=xxxx
password=xxxx
```

Upload to PyPi

```bash
pip install --upgrade setuptools wheel
pip install --upgrade twine
python setup.py sdist bdist_wheel
twine upload dist/*
```

Pin version on GitHub releases