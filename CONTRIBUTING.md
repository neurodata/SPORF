# Contribution guidelines 

## Branches

* `master` is the branch with the most current release on CRAN.
* `staging` is the development branch before being pushed to CRAN.

## Making Changes

* Avoid working on the `master` branch.
* Create a branch of `staging` and create a pull request against `staging` when you are ready.
* Indentation: two spaces, no tabs.
* Use **TRUE** and **FALSE**, not **T** and **F**.
* [formatR](https://cran.r-project.org/web/packages/formatR/index.html) can help with cleaning up your code, after change default tab width to 2. 
* Check for unnecessary whitespace with `git diff --check` before committing.


## Resources for best practices

* For additions to the R code, we will be following [this style guide](https://google.github.io/styleguide/Rguide.xml) moving forward.
* [Comments on Style by H.W.](http://adv-r.had.co.nz/Style.html)
* [Advanced R by H.W.](http://adv-r.had.co.nz)
* [R packages by H.W.](http://r-pkgs.had.co.nz)
