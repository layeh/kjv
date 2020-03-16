# Use diatheke command line to obtain Darby version

rm(list=ls())
library(data.table)

# Loading the kjv.tsv data
d = fread("kjv.tsv")

get_darby = function(verse_index) {
# Function to get darby version by diatheke
    # Input is the verse_index: 1 John 3:1
    # Output is the Darby version to be included
    command = paste0("diatheke -b ChiUns -f plain -k ", verse_index)
    verse = system(command, intern=TRUE)
    return_verse = substring(verse[1], nchar(verse_index) + 3)
    # print(paste("-- converting:", verse_index)) ## just for debugging, may slow down converting process
    return(return_verse)
}

d$index = paste0(d$V1, " ", d$V4, ":", d$V5)
# d = d[1:10, ]
d$verse =sapply(d$index, get_darby)

# Save the converted version
d1 = d[, .(V1, V2, V3, V4, V5, verse)]
fwrite(d1, 'chiuns.tsv', col.names=F, sep="\t")
