# Contributing

You are here to help on LumberJack?  First off, thank you!  Please read the following sections in order to know how to ask questions and how to work work on something.

## Get in touch

* Report bugs, suggest features or view the source code [on GitHub](https://github.com/neurodata/lumberjack)

## Organization

### Branches

* `master`: inline with current release on CRAN
* `staging`: development branch from which PRs should be made

## Making Changes

### Formatting

* Run your code through [styler](http://styler.r-lib.org/) auto-formater
* Avoid modifying formatting outside the scope of your pull request
* Use **TRUE** and **FALSE**, not **T** and **F**
* Check for unnecessary whitespace with `git diff --check` before committing

### Testing

We use the [testthat](https://github.com/r-lib/testthat) library for testing in R.  You can run your tests locally using `devtools::test()`.  Testthat tests are also run on [TravisCI](https://travis-ci.org/neurodata/lumberjack) automatically.

* New features need tests
* Tests should be fast, ideally each test should complete in under 5 seconds
* Mark longer running tests with 
* Bug fixes need [testthat](https://github.com/r-lib/testthat) functions (test the condition that was failing)

### Make your Pull Request

Congrats, you are ready for your PR!

* If adding a new feature, add a summary of your feature to the NEWS.md file.
* Create your pull request on GitHub

### Code review

Code reviews will take place for all Pull Requets.  It is our intention for code reviews to be a pleasant experience - we appreciate PRs and all the work that goes into them.  The purpose for the code review is to:

* Ensure working code
* Maintainable code
* Consistent style throughout the package

We aim to give feedback in a timely fashion

### Merge

When merging, choose the `squash and merge` type on GitHub

## Resources for best practices

* For additions to the R code, we will be following [Google's](https://google.github.io/styleguide/Rguide.xml) style guide
* [Comments on Style by H.W.](http://adv-r.had.co.nz/Style.html)
* [Advanced R by H.W.](http://adv-r.had.co.nz)
* [R packages by H.W.](http://r-pkgs.had.co.nz)
