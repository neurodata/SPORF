# packedForest submodule

## `git config` updates

The first command will show submodule status on the command `git status command`.  The second command will fetch the submodule from the commit hash by default when you run `git pull`.

```sh
git config --global status.submoduleSummary true
git config --global submodule.recurse true
```

## Initial checkout

This both initializes the submodule and checks it out

```sh
git submodule update --init
```

## Update submodule code

This will pull in the latest commits from the tracked submodule repo

```sh
git submodule update --remote --merge
```

Then commit and push the updated submodule


References:

https://medium.com/@porteneuve/mastering-git-submodules-34c65e940407