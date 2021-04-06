# Release Process

1. Create a PR "Preparing for release X.Y.Z" against master branch
    * Add section to `CHANGELOG.md` `<X.Y.Z> (YYYY-MM-DD)` with the list of changes
    * Change/verify vesion number in `CMakelists.txt`
2. Create a release "Release X.Y.Z" on Github
    * Create Tag `vX.Y.Z`
    * Copy `CHANGELOG.md` entry into the release notes
3. Create a PR "Back to development" against master branch
    * Update `<next_version> (unreleased)` section in the `CHANGELOG.md`
    * Update project version in CMakeLists.txt to the `<next version>`
