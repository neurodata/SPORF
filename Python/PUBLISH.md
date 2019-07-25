# To publish new version

## PyPI

This is to upload source to pypi.  To upload linux wheel binaries, see [manylinux](https://github.com/pypa/manylinux). 

1. Increment the version number in `rerf/__init__.py`

1. Create a ~/.pypirc file with the following content:

    ```ini
    [distutils]
    index-servers =
      neurodata
      neurodata-test

    [neurodata]
    repository=https://upload.pypi.org/legacy/
    username=xxxx
    password=xxxx

    [neurodata-test]
    repository=https://test.pypi.org/legacy/
    username=xxxx
    password=xxxx
    ```

1. Update packaging dependencies

    ```bash
    pip install --upgrade setuptools wheel
    pip install --upgrade twine
    ```

1. Test upload on test.pypi.org (only source)

    ```bash
    python setup.py sdist
    twine upload --repository neurodata-test dist/*
    ```

1. Test pip installable from test.pypi.org

    ```bash
    pip install --index-url https://test.pypi.org/simple/ --extra-index-url https://pypi.org/simple rerf==0.0.3.dev2
    ```

1. Create `dist` files

    1. On `Linux`, only upload source

        ```bash
        python setup.py sdist
        ```

    1. On `OSX` include `wheels`

        ```bash
        python setup.py sdist bdist_wheel
        ```

1. If successful, upload version to PyPi

    ```bash
    twine upload --repository neurodata dist/*
    ```

1. Test pip installable

    ```bash
    pip install rerf -U
    ```

1. Create new release on GitHub (tag)

## Docker

```bash
cd docker
docker build -t neurodata/rerf .
docker push neurodata/rerf
```
