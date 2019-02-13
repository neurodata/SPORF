# packedForest submodule

## Initial checkout

This both initializes the submodule and checks it out

```sh
git submodule update --init
```

## `git config` updates

Fetch the submodule by default when you run `git pull` (without this it won't grab the submodule).

```sh
git config --global submodule.recurse true
```

Show submodule status on the command `git status command` (optional)

```sh
git config --global status.submoduleSummary true
```

## Update submodule

This will pull in the latest commits from the tracked submodule repo

```sh
git submodule update --remote --merge
```

To push the update, make a commit and push

### References

https://medium.com/@porteneuve/mastering-git-submodules-34c65e940407