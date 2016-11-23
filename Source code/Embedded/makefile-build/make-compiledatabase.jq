# JQ script to generate a libtooling compilation database from fairly easily makefile-generated inputs.
# Copyright 2016 Sensics, Inc.
# SPDX-License-Id: Apache-2.0

# Utility: Get rid of trailing newlines, etc from cross-platformy things
def tidyStringFromFile: . | rtrimstr("\n") | rtrimstr("\r") | rtrimstr(" ");

[
# First, split into an array of sources
[. | tidyStringFromFile | split(";")[]]
# then, remove the .c suffix to get the filename stem (easier to do this in two steps)
# in an array of objects that also contain the filename
| map({
    filename: .,
    stem: . | rtrimstr(".c")
})
# now, turn the stem into an object file name, and add constant params.
| map({
    filename,
    objectFile: (.stem + ".o"),
    baseCmd: $params[0]|tidyStringFromFile,
    baseDir: $params[1]|tidyStringFromFile,
    buildDir: $params[2]|tidyStringFromFile,
    srcRoot: $params[3]|tidyStringFromFile,
})
| map({
    # Must add the base dir here since it might have had spaces that would confuse Make's simple lists.
    filename: (.srcRoot + "/" + .filename),
    objectFile: (.buildDir + "/" + .objectFile),
    baseDir,
    baseCmd
})

# For ease of substitution, map to variables for construction of output.
[] as {
    $filename,
    $objectFile,
    $baseDir,
    $baseCmd
} | {
    #directory: $params[1]|tidyStringFromFile,
    directory: $baseDir,
    #command: \($params[0]|tidyStringFromFile) -o \"" + $objectFile +"\" \""+ $filename +"\"",
    command: ($baseCmd + " -o \"" + $objectFile + "\" \"" + $filename + "\""),
    file: $filename
}
]
